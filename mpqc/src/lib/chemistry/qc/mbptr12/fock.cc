//
// fock.cc
//
// Copyright (C) 2004 Edward Valeev
//
// Author: Edward Valeev <edward.valeev@chemistry.gatech.edu>
// Maintainer: EV
//
// This file is part of the SC Toolkit.
//
// The SC Toolkit is free software; you can redistribute it and/or modify
// it under the terms of the GNU Library General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// The SC Toolkit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public License
// along with the SC Toolkit; see the file COPYING.LIB.  If not, write to
// the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
//
// The U.S. Government is granted a limited license as per AL 91-7.
//

#ifdef __GNUC__
#pragma implementation
#endif

#include <stdexcept>
#include <util/class/scexception.h>
#include <chemistry/qc/basis/petite.h>
#include <chemistry/qc/basis/symmint.h>
#include <chemistry/qc/basis/gaussbas.h>
#include <chemistry/qc/basis/uncontract.h> 
#include <chemistry/qc/mbptr12/r12int_eval.h>
#include <chemistry/qc/mbptr12/mbptr12.h>
#include <chemistry/qc/mbptr12/print.h>

using namespace std;
using namespace sc;

#define TEST_FOCKBUILD 0
#define NEW_HCORE 1

RefSCMatrix
R12IntEval::fock(const Ref<MOIndexSpace>& bra_space,
                  const Ref<MOIndexSpace>& ket_space,
                  SpinCase1 spin,
                  double scale_J, double scale_K)
{
  if (bra_space->rank() == 0 || ket_space->rank() == 0)
    return 0;
  
  Ref<SingleRefInfo> refinfo = r12info()->refinfo();
  const Ref<GaussianBasisSet> bs1 = bra_space->basis();
  const Ref<GaussianBasisSet> bs2 = ket_space->basis();
  const bool bs1_eq_bs2 = (bs1 == bs2);
  int nshell1 = bs1->nshell();
  int nshell2 = bs2->nshell();

  RefSCMatrix vec1t = bra_space->coefs().t();
  RefSCMatrix vec2 = ket_space->coefs();

#if NEW_HCORE
  RefSCDimension aodim1 = vec1t.coldim();
  RefSCDimension aodim2 = vec2.rowdim();
  Ref<SCMatrixKit> sokit = bs1->so_matrixkit();

  // cast Wavefunction to MBPT2
  MBPT2 *mp2wfn = dynamic_cast<MBPT2*>(r12info()->wfn());
  if (mp2wfn == 0) {
      throw ProgrammingError(
          "r12info()->wfn() was not an MBPT2 object",
          __FILE__, __LINE__, class_desc());
  }
  // Form the DK correction in the current basis using the momentum
  // basis of the reference wavefunction.  The momentum basis in the
  // reference should be a superset of hcore_basis
  Ref<GaussianBasisSet> p_basis = mp2wfn->ref()->momentum_basis();
  Ref<GaussianBasisSet> hcore_basis;
  Ref<GaussianBasisSetSum> bs1_plus_bs2;
  if (bs1_eq_bs2) {
      hcore_basis = bs1;
    }
  else {
    bs1_plus_bs2 = new GaussianBasisSetSum(bs1,bs2);
    hcore_basis = bs1_plus_bs2->bs12();
  }

  RefSymmSCMatrix hsymm
      = mp2wfn->ref()->core_hamiltonian_for_basis(hcore_basis,p_basis);


  // convert hsymm to the AO basis
  Ref<Integral> localints = r12info_->integral()->clone();
  localints->set_basis(hcore_basis,hcore_basis);
  Ref<PetiteList> hcore_pl = localints->petite_list();
  RefSymmSCMatrix hsymm_ao = hcore_pl->to_AO_basis(hsymm);
  hsymm = 0;

  RefSCMatrix h(aodim1, aodim2, sokit);
  if (bs1_eq_bs2) {
      h.assign(0.0);
      h.accumulate(hsymm_ao);
  }
  else {
    RefSCMatrix hrect_ao(hsymm_ao.dim(), hsymm_ao.dim(), sokit);
    hrect_ao.assign(0.0);
    hrect_ao.accumulate(hsymm_ao);

    // extract the bs1 by bs2 block:
    //   loop over all bs1 fblocks
    //     loop over all bs2 fblocks
    //       copy block to h
    //     end loop
    //   end loop
    const int nbf = hcore_basis->nbasis();
    const int nfblock = bs1_plus_bs2->nfblock();
    for(int rb=0; rb<nfblock; ++rb) {
      const int rf_start12 = bs1_plus_bs2->fblock_to_function(rb);
      if (bs1_plus_bs2->function_to_basis(rf_start12) != 1)
      continue;
      const int rf_end12 = rf_start12 + bs1_plus_bs2->fblock_size(rb) - 1;

      const int rf_start1 = bs1_plus_bs2->function12_to_function(rf_start12);
      const int rf_end1 = bs1_plus_bs2->function12_to_function(rf_end12);

      for(int cb=0; cb<nfblock; ++cb) {
        const int cf_start12 = bs1_plus_bs2->fblock_to_function(cb);
        if (bs1_plus_bs2->function_to_basis(cf_start12) != 2)
        continue;
        const int cf_end12 = cf_start12 + bs1_plus_bs2->fblock_size(cb) - 1;

        const int cf_start2 = bs1_plus_bs2->function12_to_function(cf_start12);
        const int cf_end2 = bs1_plus_bs2->function12_to_function(cf_end12);

        // assign row-/col-subblock to h
        h.assign_subblock(hrect_ao, rf_start1, rf_end1, cf_start2, cf_end2, rf_start12, cf_start12);
      }
    }
    
  }


#else // ! NEW_HCORE

  if (r12info()->wfn()->dk() > 0) {
      throw ProgrammingError(
	  "cannot use reference with dk > 0 with old mp2r12 fock algorithm",
	  __FILE__, __LINE__, class_desc());
  }

  Ref<Integral> localints = r12info_->integral()->clone();
  localints->set_basis(bs1,bs2);

  Ref<OneBodyInt> h_ints = localints->hcore();

  // form AO moment matrices
  RefSCDimension aodim1 = vec1t.coldim();
  RefSCDimension aodim2 = vec2.rowdim();
  Ref<SCMatrixKit> aokit = bs1->so_matrixkit();
  RefSCMatrix h(aodim1, aodim2, aokit);
  h.assign(0.0);
  
  for(int sh1=0; sh1<nshell1; sh1++) {
    int bf1_offset = bs1->shell_to_function(sh1);
    int nbf1 = bs1->shell(sh1).nfunction();

    int sh2max;
    if (bs1_eq_bs2)
      sh2max = sh1;
    else
      sh2max = nshell2-1;
    
    for(int sh2=0; sh2<=sh2max; sh2++) {
      int bf2_offset = bs2->shell_to_function(sh2);
      int nbf2 = bs2->shell(sh2).nfunction();
      
      h_ints->compute_shell(sh1,sh2);
      const double *hintsptr = h_ints->buffer();

      int bf1_index = bf1_offset;
      for(int bf1=0; bf1<nbf1; bf1++, bf1_index++, hintsptr+=nbf2) {
	int bf2_index = bf2_offset;
	const double *ptr = hintsptr;
	int bf2max;
        if (bs1_eq_bs2 && sh1 == sh2)
          bf2max = bf1;
        else
	  bf2max = nbf2-1;
	for(int bf2=0; bf2<=bf2max; bf2++, bf2_index++, ptr++) {

	  h.set_element(bf1_index, bf2_index, *ptr);

        }
      }
    }
  }

  // Symmetrize matrices, if necessary
  if (bs1_eq_bs2) {
    const int nbasis = bs1->nbasis();
    for(int bf1=0; bf1<nbasis; bf1++)
      for(int bf2=0; bf2<=bf1; bf2++) {
        h(bf2,bf1) = h(bf1,bf2);
      }
  }

  h_ints = 0;

#endif // NEW_HCORE

  // finally, transform
  RefSCMatrix F = vec1t * h * vec2;
  if (debug_ >= DefaultPrintThresholds::allN2)
    F.print("Core Hamiltonian contribution");
  // and clean up a bit
  h = 0;
  
  const bool spin_unrestricted = spin_polarized();
  
#if TEST_FOCKBUILD
  RefSCMatrix Ftest;
  if (!spin_unrestricted)
    Ftest = F.clone();
#endif
  
  //
  // add coulomb and exchange parts
  //
  const double J_prefactor = (spin_unrestricted ? 1.0 : 2.0);
  for(int s=0; s<nspincases1(); s++) {
    const SpinCase1 sc = static_cast<SpinCase1>(s);
    if (occ(sc)->rank() == 0)
      continue;
    if (scale_J != 0.0) {
      RefSCMatrix J = coulomb_(occ(sc),bra_space,ket_space);
      J.scale(J_prefactor*scale_J);
      if (debug_ >= DefaultPrintThresholds::allN2)
        J.print("Coulomb contribution");
      F.accumulate(J); J = 0;
    }
  }
  if (scale_K != 0.0) {
    RefSCMatrix K = exchange_(occ(spin),bra_space,ket_space);
    K.scale(-1.0*scale_K);
    if (debug_ >= DefaultPrintThresholds::allN2)
      K.print("Exchange contribution");
    F.accumulate(K); K = 0;
  }
  
  if (debug_ >= DefaultPrintThresholds::allN2) {
    F.print("Fock matrix");
  }
  
#if TEST_FOCKBUILD
  if (scale_J == 1.0 && scale_K == 1.0) {
    Ref<GaussianBasisSet> obs = occ(Alpha)->basis();
    Ref<FockContribution> fc = new CLHFContribution(bs1,bs2,obs,obs);
    
    // transform the density matrix to the AO basis
    RefSymmSCMatrix D_ao = reference()->ao_density();
    RefSCMatrix G_ao = Ftest.kit()->matrix(aodim1,aodim2);
    fc->set_fmat(0, G_ao);
    fc->set_pmat(0, D_ao);
    
    const Ref<Integral>& integral = r12info()->integral();
    const unsigned int nthreads = r12info()->thr()->nthread();
    Ref<TwoBodyInt>* tbints = new Ref<TwoBodyInt>[nthreads];
    for(unsigned int t=0; t<nthreads; ++t)
      tbints[t] = integral->electron_repulsion();
    
    // WARNING should replace with something reasonable
    const double accuracy = 1.0e-15;
    Ref<FockBuild> fb = new FockBuild(fc,accuracy,tbints,
                                      bs1, bs2, obs, obs,
                                      r12info()->msg(), r12info()->thr(),
                                      integral);
    fb->build();
    ExEnv::out0() << indent << scprintf("%20.0f integrals\n",
    fb->contrib()->nint());
    fb = 0;
    
    // WARNING Need to symmetrize G
    
    for(unsigned int t=0; t<nthreads; ++t)
      tbints[t] = 0;
    delete[] tbints;
  }
#endif
  
  return F;
}



RefSCMatrix
R12IntEval::Delta_DKH_(const Ref<MOIndexSpace>& bra_space,
                          const Ref<MOIndexSpace>& ket_space,
                          SpinCase1 spin) 
{

  ExEnv::out0() << indent<< "florian: Entering Delta_DKH" <<incindent << endl;
  Ref<SingleRefInfo> refinfo = r12info()->refinfo();
  const Ref<GaussianBasisSet> bs1 = bra_space->basis();
  const Ref<GaussianBasisSet> bs2 = ket_space->basis();
  const bool bs1_eq_bs2 = (bs1 == bs2);
  int nshell1 = bs1->nshell();
  int nshell2 = bs2->nshell();
  
  RefSCMatrix vec1t = bra_space->coefs().t();
  RefSCMatrix vec2 = ket_space->coefs();
  
  RefSCDimension aodim1 = vec1t.coldim();
  RefSCDimension aodim2 = vec2.rowdim();
  Ref<SCMatrixKit> sokit = bs1->so_matrixkit();

  // cast Wavefunction to MBPT2
  MBPT2 *mp2wfn = dynamic_cast<MBPT2*>(r12info()->wfn());
  if (mp2wfn == 0) {
      throw ProgrammingError(
          "r12info()->wfn() was not an MBPT2 object",
          __FILE__, __LINE__, class_desc());
  }
  int dk = r12info()->refinfo()->ref()->dk();
  if (dk != 2) {
       throw ProgrammingError(
 	  "Delta_DKH can only be used with dk = 2",
 	  __FILE__, __LINE__, class_desc());
  }

  Ref<SCF> ref = mp2wfn->ref();
  
  Ref<GaussianBasisSet> hcore_basis; 
  Ref<GaussianBasisSetSum> bs1_plus_bs2;
  if (bs1_eq_bs2) {
      hcore_basis = bs1;
    }
  else {
    bs1_plus_bs2 = new GaussianBasisSetSum(bs1,bs2);
    hcore_basis = bs1_plus_bs2->bs12();
  }

  // Form the DK correction in the current basis using the momentum
  // basis of the reference wavefunction.  The momentum basis in the
  // reference should be a superset of hcore_basis

  // be precise, it's a critical step
  //  Ref<GaussianBasisSet> uncontract_p = new UncontractedBasisSet(ref->momentum_basis());   
  Ref<GaussianBasisSet> uncontract_p = ref->momentum_basis(); // for consistency
  Ref<GaussianBasisSet> p_basis = uncontract_p+hcore_basis;
   
  // compute the relativistic core Hamiltonian  
  RefSymmSCMatrix hsymm = ref->core_hamiltonian_for_basis(hcore_basis,p_basis);
  
  // compute T+V+mass-velocity
  RefSymmSCMatrix TVmv = tvp_(dk,hcore_basis,p_basis); 

  // compute Darwin term in coordinate space
  mp2wfn->integral()->set_basis(hcore_basis);

  const int natom=mp2wfn->molecule()->natom();
  GaussianBasisSet::ValueData* vdata1 = new GaussianBasisSet::ValueData(hcore_basis,mp2wfn->integral());
  const int nbasis1 = hcore_basis->nbasis();
  const double c = 137.0359895; // speed of light in a vacuum in a.u.
  const double darwin_prefac=3.14159265358979323846/(c*c*2.0);
  RefSymmSCMatrix Darwin(hsymm.dim(), sokit);
  Darwin.assign(0.0);
  double* values1=new double[nbasis1];
  
  for (int iatom=0; iatom<natom; iatom++){  

	  SCVector3 R_iatom=mp2wfn->molecule()->r(iatom);
	  // this puts values of basis functions evaluated at R_iatom into values1
	  hcore_basis->values(R_iatom,vdata1,values1);
	  const double prefac=darwin_prefac*mp2wfn->molecule()->charge(iatom);
		  
	  for (int ibasis=0; ibasis<hcore_basis->nbasis(); ibasis++){
		  for (int jbasis=0; jbasis<=ibasis; jbasis++){

			  const double d=values1[ibasis]*values1[jbasis]*prefac;
			  Darwin.accumulate_element(ibasis,jbasis,d);	
		  }
	  }	
  }
  delete[] values1;
  delete vdata1;
 
  TVmv.accumulate(Darwin);
  Darwin=0;
  // calculate delta_DKH = H_DKH - H_tvp to get the difference Hamiltonian
  TVmv.scale(-1.0);
  hsymm.accumulate(TVmv);
  
  // from now on proceed as in fock
  
  // convert hsymm to the AO basis
  Ref<Integral> localints = r12info_->integral()->clone();
  localints->set_basis(hcore_basis,hcore_basis);
  Ref<PetiteList> hcore_pl = localints->petite_list();
  RefSymmSCMatrix hsymm_ao = hcore_pl->to_AO_basis(hsymm);
  hsymm = 0;

  RefSCMatrix h(aodim1, aodim2, sokit);
  
  if (bs1_eq_bs2) {
    h.assign(0.0);
    h.accumulate(hsymm_ao);
  } else {
    RefSCMatrix hrect_ao(hsymm_ao.dim(), hsymm_ao.dim(), sokit);
    hrect_ao.assign(0.0);
    hrect_ao.accumulate(hsymm_ao);
    
    // extract the bs1 by bs2 block:
    //   loop over all bs1 fblocks
    //     loop over all bs2 fblocks
    //       copy block to h
    //     end loop
    //   end loop
    const int nbf = hcore_basis->nbasis();
    const int nfblock = bs1_plus_bs2->nfblock();
    for (int rb=0; rb<nfblock; ++rb) {
      const int rf_start12 = bs1_plus_bs2->fblock_to_function(rb);
      if (bs1_plus_bs2->function_to_basis(rf_start12) != 1)
        continue;
      const int rf_end12 = rf_start12 + bs1_plus_bs2->fblock_size(rb) - 1;
      
      const int rf_start1 = bs1_plus_bs2->function12_to_function(rf_start12);
      const int rf_end1 = bs1_plus_bs2->function12_to_function(rf_end12);
      
      for (int cb=0; cb<nfblock; ++cb) {
        const int cf_start12 = bs1_plus_bs2->fblock_to_function(cb);
        if (bs1_plus_bs2->function_to_basis(cf_start12) != 2)
          continue;
        const int cf_end12 = cf_start12 + bs1_plus_bs2->fblock_size(cb) - 1;
        
        const int cf_start2 = bs1_plus_bs2->function12_to_function(cf_start12);
        const int cf_end2 = bs1_plus_bs2->function12_to_function(cf_end12);
        
        // assign row-/col-subblock to h
        h.assign_subblock(hrect_ao, rf_start1, rf_end1, cf_start2, cf_end2,
                          rf_start12, cf_start12);
      }
    }
    
  }

  
  // finally, transform
  RefSCMatrix F = vec1t * h * vec2;
  if (debug_ >= DefaultPrintThresholds::allN2)
    F.print("Core Hamiltonian contribution");
  // and clean up a bit
  h = 0;
  
  const bool spin_unrestricted = spin_polarized();
  
  if (debug_ >= DefaultPrintThresholds::allN2) {
    F.print("Fock matrix");
  }
  ExEnv::out0() << decindent << indent << "florian: Leaving Delta_DKH" << endl;
  return F;
}



RefSymmSCMatrix
R12IntEval::tvp_(int dk,const Ref<GaussianBasisSet> &bas,
                          const Ref<GaussianBasisSet> &p_bas)
{
  // cast Wavefunction to MBPT2
  MBPT2 *mp2wfn = dynamic_cast<MBPT2*>(r12info()->wfn());
#define DK_DEBUG 0

  ExEnv::out0() << indent << "florian: Entering tvp_"<< incindent << endl;
  if (mp2wfn->atom_basis().nonnull()) {
    throw FeatureNotImplemented("atom_basis given and dk > 0",
                                __FILE__, __LINE__, class_desc());
  }
  if (dk != 2) {
    throw FeatureNotImplemented("dk must be 2 for using with R12",
                                __FILE__, __LINE__, class_desc());
  }
  if (dk > 0 && mp2wfn->gradient_needed()) {
    throw FeatureNotImplemented("gradients not available for dk",
                                __FILE__, __LINE__, class_desc());
  }

  // The one electron integrals will be computed in the momentum basis.
  // Use mbptr12's integrals.
  mp2wfn->integral()->set_basis(p_bas);
  
  Ref<PetiteList> p_pl = mp2wfn->integral()->petite_list();

  RefSCDimension p_so_dim = p_pl->SO_basisdim();
  RefSCDimension p_ao_dim = p_pl->AO_basisdim();
  Ref<SCMatrixKit> p_kit = p_bas->matrixkit();
  Ref<SCMatrixKit> p_so_kit = p_bas->so_matrixkit();

  // Compute the overlap in the momentum basis.
  RefSymmSCMatrix S_skel(p_ao_dim, p_kit);
  S_skel.assign(0.0);
  Ref<SCElementOp> hc =
    new OneBodyIntOp(new SymmOneBodyIntIter(mp2wfn->integral()->overlap(), p_pl));
  S_skel.element_op(hc);
  hc=0;
  RefSymmSCMatrix S(p_so_dim, p_so_kit);
  p_pl->symmetrize(S_skel,S);

  ExEnv::out0() << indent
                << "The momentum basis is:"
                << std::endl;
  ExEnv::out0() << incindent;
  p_bas->print_brief(ExEnv::out0());
  ExEnv::out0() << decindent;

  ExEnv::out0() << indent
                << "Orthogonalizing the momentum basis"
                << std::endl;
  Ref<OverlapOrthog> p_orthog
    = new OverlapOrthog(mp2wfn->orthog_method(), S, p_so_kit,
                        mp2wfn->lindep_tol(), debug_);

  RefSCDimension p_oso_dim = p_orthog->orthog_dim();

  // form skeleton Hcore in the momentum basis
  RefSymmSCMatrix T_skel(p_ao_dim, p_kit);
  T_skel.assign(0.0);

  hc =
    new OneBodyIntOp(new SymmOneBodyIntIter(mp2wfn->integral()->kinetic(), p_pl));
  T_skel.element_op(hc);
  hc=0;

  // finish constructing the kinetic energy integrals,
  // for which the skeleton is in hao
  RefSymmSCMatrix T(p_so_dim, p_so_kit);
  p_pl->symmetrize(T_skel,T);
  T_skel = 0;

  // Transform T into an orthogonal basis
  RefSymmSCMatrix T_oso(p_oso_dim, p_so_kit);
  T_oso.assign(0.0);
  T_oso.accumulate_transform(p_orthog->basis_to_orthog_basis(),T);

  // diagonalize the T integrals to get a momentum basis
  RefDiagSCMatrix Tval(p_oso_dim, p_so_kit);
  RefSCMatrix Tvec(p_oso_dim, p_oso_dim, p_so_kit);
  // Tvec * Tval * Tvec.t() = T_oso
  T_oso.diagonalize(Tval,Tvec);

  T_oso = 0;

#if DK_DEBUG
  T.print("T");
  Tval.print("Tval");
#endif

  // Compute the kinetic energy and the mass-velocity term
  RefSymmSCMatrix kinetic_energy(p_oso_dim, p_so_kit);
  RefSymmSCMatrix mass_velocity(p_oso_dim, p_so_kit);
  kinetic_energy.assign(0.0);
  mass_velocity.assign(0.0);
  const double c = 137.0359895; // speed of light in a vacuum in a.u.
  //  const double c = 137.035999679;  // speed of light according to CODATA (2006)
  //  const double c = 137.03599911;  // speed of light according to CRC
  int noso = p_oso_dim.n();
  for (int i=0; i<noso; i++) {
    double T_val = Tval(i);
    // momentum basis sets with near linear dependencies may
    // have T_val approximately equal to zero.  These can be
    // negative, which will cause a SIGFPE in sqrt.
    if (T_val < DBL_EPSILON) T_val = 0.0;
    const double p = sqrt(2.0*T_val);
    const double p2 = p*p;
    kinetic_energy(i,i) = 0.5*p2;
    mass_velocity(i,i)  = p2*p2 * (-1.0)/(8.0*c*c);
  }

  // Construct the transform from the coordinate to the momentum
  // representation in the momentum basis
  RefSCMatrix so_to_p
    = Tvec.t() * p_orthog->basis_to_orthog_basis();

#if DK_DEBUG
  so_to_p.print("so_to_p");
#endif

  // compute the V integrals
  Ref<OneBodyInt> V_obi = mp2wfn->integral()->nuclear();
  V_obi->reinitialize();
  hc = new OneBodyIntOp(new SymmOneBodyIntIter(V_obi, p_pl));
  RefSymmSCMatrix V_skel(p_ao_dim, p_kit);
  V_skel.assign(0.0);
  V_skel.element_op(hc);
  V_obi = 0;
  hc = 0;
  RefSymmSCMatrix V(p_so_dim, p_so_kit);
  p_pl->symmetrize(V_skel,V);
  V_skel = 0;

#if DK_DEBUG
  V.print("V");
#endif

  // transform V to the momentum basis
  RefSymmSCMatrix V_pbas(p_oso_dim, p_so_kit);
  V_pbas.assign(0.0);
  V_pbas.accumulate_transform(so_to_p, V);

  // calculate tvp
  RefSymmSCMatrix tvp(p_oso_dim, p_so_kit);
  tvp.assign(0.0);
  tvp.accumulate(kinetic_energy);
  tvp.accumulate(mass_velocity);
  tvp.accumulate(V_pbas);

  V_pbas = 0;
  kinetic_energy=0;
  mass_velocity=0;

  // form the momentum basis hamiltonian
  RefSymmSCMatrix h_pbas(p_oso_dim, p_so_kit);
  h_pbas.assign(0.0);
  h_pbas.accumulate(tvp);

#if DK_DEBUG
  h_pbas.print("h_pbas");
#endif

  // Construct the transform from the momentum representation to the
  // coordinate representation in the momentum basis
  RefSCMatrix p_to_so
    = p_orthog->basis_to_orthog_basis_inverse() * Tvec;

  // Construct the transform from the momentum basis to the
  // coordinate basis.
  mp2wfn->integral()->set_basis(bas,p_bas);
  Ref<PetiteList> pl = mp2wfn->integral()->petite_list();
  RefSCMatrix S_ao_p(pl->AO_basisdim(), p_ao_dim, p_kit);
  S_ao_p.assign(0.0);
  hc = new OneBodyIntOp(mp2wfn->integral()->overlap());
  S_ao_p.element_op(hc);
  hc=0;
  // convert s_ao_p into the so ao and so p basis
  RefSCMatrix blocked_S_ao_p(pl->AO_basisdim(), p_pl->AO_basisdim(), p_so_kit);
  blocked_S_ao_p->convert(S_ao_p);
  RefSCMatrix S_ao_p_so_l = pl->sotoao() * blocked_S_ao_p;
  RefSCMatrix S_ao_p_so = S_ao_p_so_l * p_pl->aotoso();
  S_ao_p_so_l = 0;

  // transform h_pbas back to the so basis
  RefSymmSCMatrix h_dk_so(pl->SO_basisdim(),bas->so_matrixkit());
  h_dk_so.assign(0.0);
  h_dk_so.accumulate_transform(S_ao_p_so
                               *p_orthog->overlap_inverse()
                               *p_to_so, h_pbas);

  mp2wfn->integral()->set_basis(mp2wfn->basis());
  Ref<PetiteList> bas_pl = mp2wfn->integral()->petite_list();

  // Check to see if the momentum basis spans the coordinate basis.  The
  // following approach seems reasonable, but a more careful mathematical
  // analysis would be desirable.
  double S_ao_projected_trace
    = (S_ao_p_so * p_orthog->overlap_inverse() * S_ao_p_so.t()).trace()
    / pl->SO_basisdim()->n();
  double S_ao_trace = mp2wfn->overlap().trace() / bas_pl->SO_basisdim()->n();
  ExEnv::out0() << indent
                << "Tr(orbital basis overlap)/N = "
                << S_ao_trace
                << std::endl;
  ExEnv::out0() << indent
                << "Tr(orbital basis overlap projected into momentum basis)/N = "
                << S_ao_projected_trace
                << std::endl;
  if (fabs(S_ao_projected_trace-S_ao_trace)>mp2wfn->lindep_tol()) {
    ExEnv::out0() << indent
                  << "WARNING: the momentum basis does not span the orbital basis"
                  << std::endl;
  }

#if DK_DEBUG
  S_ao_p_so.print("S_ao_p_so");
  p_to_so.print("p_to_so");
  //(p_to_so*so_to_p).print("p_to_so*so_to_p");
  (S_ao_p_so*S.gi()*p_to_so).print("S_ao_p_so*S.gi()*p_to_so");
  (T+V).print("T+V");
  h_dk_so.print("h_dk_so");
#endif

  ExEnv::out0() << decindent << indent << "florian: Leaving tvp_" << endl;
  return h_dk_so;
}


///////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
