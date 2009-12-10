//
// pt2r12.h
//
// Copyright (C) 2009 Edward Valeev
//
// Author: Edward Valeev <evaleev@vt.edu>
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

#ifdef __GNUG__
#pragma interface
#endif

#ifndef _mpqc_src_lib_chemistry_qc_mbptr12_pt2r12_h
#define _mpqc_src_lib_chemistry_qc_mbptr12_pt2r12_h

#include <chemistry/qc/wfn/wfn.h>
#include <chemistry/qc/mbptr12/spin.h>
#include <chemistry/qc/mbptr12/r12wfnworld.h>
#include <chemistry/qc/mbptr12/r12int_eval.h>
#include <chemistry/qc/mbptr12/rdm.h>

namespace sc {

  /// PT2R12: a universal second-order R12 correction
  class PT2R12 : public Wavefunction {
    public:
      /** A KeyVal constructor is used to generate a RDM<R>
          object from the input. The full list of keywords
          that are accepted is below.

          <table border="1">

          <tr><td>%Keyword<td>Type<td>Default<td>Description

          <tr><td><tt>reference</tt><td>Wavefunction<td>none<td>the Wavefunction object

          <tr><td><tt>rdm2</tt><td>RDM<Two><td>none<td>the RDM<Two> object that provides the 2-RDM. It must
          be constructed from the object provided by the <tt>reference</tt> keyword.

          </table>
       */
      PT2R12(const Ref<KeyVal> &keyval);
      PT2R12(StateIn &s);
      ~PT2R12();
      void save_data_state(StateOut &s);

      void compute();
      void print(std::ostream& os =ExEnv::out0());
      RefSymmSCMatrix density();
      int nelectron();
      int spin_polarized();
      int value_implemented() const { return 1; }

      /// PT2R12 is an R12 Wavefunction
      const Ref<R12WavefunctionWorld>& r12world() const { return r12world_; }

    private:
      size_t memory_r12_;
      Ref<Wavefunction> reference_;
      Ref< RDM<Two> > rdm2_;
      Ref<R12IntEval> r12eval_;
      Ref<R12WavefunctionWorld> r12world_;
      unsigned int nfzc_;
      bool omit_virtuals_;

      bool tpdm_from_opdms_;
      int debug_;

      /// 1-RDM
      RefSymmSCMatrix rdm1(SpinCase1 spin);
      /// 2-RDM
      RefSymmSCMatrix rdm2(SpinCase2 spin);
      /// 2-RDM cumulant
      RefSymmSCMatrix lambda2(SpinCase2 spin);
      /// gg block of 2-RDM (@sa R12IntEval::gg_space() )
      RefSymmSCMatrix rdm2_gg(SpinCase2 spin);
      /// gg block of 2-RDM cumulant (@sa R12IntEval::gg_space() )
      RefSymmSCMatrix lambda2_gg(SpinCase2 spin);
      /// geminal coefficient matrix
      RefSCMatrix C(SpinCase2 S);

      RefSCMatrix V_genref_projector2(SpinCase2 pairspin);
      RefSCMatrix V_transformed_by_C(SpinCase2 pairspin);
      RefSymmSCMatrix X_transformed_by_C(SpinCase2 pairspin);
      RefSymmSCMatrix B_transformed_by_C(SpinCase2 pairspin);
      /// computes the projected contribution to the energy.
      double compute_DC_energy_GenRefansatz2();

      /// This function computes the "old" General_PT2R12 correction, i.e. the one invoking projector 1.
      double energy_PT2R12_projector1(SpinCase2 pairspin);
      double energy_PT2R12_projector2(SpinCase2 pairspin);

      /// Returns Hcore in MO basis
      RefSymmSCMatrix hcore_mo();
      RefSymmSCMatrix hcore_mo(SpinCase1 spin);
      /// molecular integrals in chemist's notation
      RefSymmSCMatrix moints();  // closed shell case
      RefSCMatrix moints(SpinCase2 pairspin);
      /// This is the g (in Dirac notation) that should be used.
      RefSCMatrix g(SpinCase2 pairspin);
      /**
       * general-reference Fock matrix (cf. eqn. (71b) of W. Kutzelnigg, D. Mukherjee, J. Chem Phys. 107 (1997) p. 432)
       * if opdm_ in R12WavefunctionWorld is set, otherwise it returns the ordinary Fock operator.
       */
      RefSCMatrix f(SpinCase1 spin);
      /*
       * phi truncated in lambda: terms with three-particle lambda's or higher or terms with
       * squares (or higher) of two-particle lambda's are neglected.
       */
      RefSCMatrix phi(SpinCase2 pairspin);
      /*
       * phi truncated in lambda: terms with three-particle lambda's or higher or terms with
       * squares (or higher) of two-particle lambda's are neglected.
       *
       * \sa PT2R12::phi()
       *  this version does not use 2-pdm, but a 2-body cumulant (2-lambda)
       */
      RefSymmSCMatrix phi_cumulant(SpinCase2 pairspin);

      /// computes the energy using the Hylleraas matrix
      /// @param hmat Hylleraas matrix
      /// @param pairspin SpinCase2
      /// @param print_pair_energies if true, will print pair energies. The default is true.
      double compute_energy(const RefSCMatrix &hmat,
                            SpinCase2 pairspin,
                            bool print_pair_energies = true);

  };

} // end of namespace sc

#endif // end of header guard


// Local Variables:
// mode: c++
// c-file-style: "CLJ-CONDENSED"
// End: