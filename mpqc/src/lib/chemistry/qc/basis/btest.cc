//
// btest.cc --- test program
//
// Copyright (C) 1996 Limit Point Systems, Inc.
//
// Author: Edward Seidl <seidl@janed.com>
// Maintainer: LPS
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

#include <strstream.h>

#include <util/misc/formio.h>
#include <util/keyval/keyval.h>
#include <util/state/stateio.h>
#include <util/state/state_text.h>
#include <util/state/state_bin.h>
#include <chemistry/qc/basis/basis.h>
#include <chemistry/qc/basis/files.h>
#include <chemistry/qc/basis/petite.h>
#include <chemistry/qc/basis/symmint.h>
#include <chemistry/qc/intv3/intv3.h>
#include <chemistry/qc/basis/sobasis.h>
#include <chemistry/qc/basis/sointegral.h>
#include <chemistry/qc/basis/extent.h>

static void
do_so_shell_test(const RefSOBasis& sobas, const RefTwoBodySOInt &soer,
                 int i, int j, int k, int l)
{
  if (i>=soer->basis1()->nshell()
      ||j>=soer->basis2()->nshell()
      ||k>=soer->basis3()->nshell()
      ||l>=soer->basis4()->nshell()) return;

  int p,q,r,s;
  soer->compute_shell(i,j,k,l);
  const double *buf = soer->buffer();
  int np = sobas->nfunction(i);
  int nq = sobas->nfunction(j);
  int nr = sobas->nfunction(k);
  int ns = sobas->nfunction(l);
  int off = 0;
  cout << "SHELL ("<<i<<j<<"|"<<k<<l<<"):" << endl;
  for (p=0; p<np; p++) {
      for (q=0; q<nq; q++) {
          for (r=0; r<nr; r++) {
              cout << "      ("<<p<<q<<"|"<<r<<"*) =";
              for (s=0; s<ns; s++, off++) {
                  cout << scprintf(" % 10.6f",buf[off]);
                }
              cout << endl;
            }
        }
    }
}

static void
do_so_shell_test(const RefSOBasis& sobas, const RefOneBodySOInt &soov,
                 int i, int j)
{
  if (i>=soov->basis1()->nshell()
      ||j>=soov->basis2()->nshell()) return;

  int p,q;
  soov->compute_shell(i,j);
  const double *buf = soov->buffer();
  int np = sobas->nfunction(i);
  int nq = sobas->nfunction(j);
  int off = 0;
  cout << "SHELL ("<<i<<"|"<<j<<"):" << endl;
  for (p=0; p<np; p++) {
      cout << "      ("<<p<<"|"<<"*) =";
      for (q=0; q<nq; q++, off++) {
          cout << scprintf(" % 10.6f",buf[off]);
        }
      cout << endl;
    }
}

static void
do_so_test(const RefKeyVal &keyval,
           const RefIntegral& intgrl, const RefGaussianBasisSet &gbs)
{
  intgrl->set_basis(gbs);

  RefSOBasis sobas = new SOBasis(gbs, intgrl);
  sobas->print(cout << node0);

  RefTwoBodyInt aoer = intgrl->electron_repulsion();
  RefTwoBodySOInt soer = new TwoBodySOInt(aoer);

  RefOneBodyInt aoov = intgrl->overlap();
  RefOneBodySOInt soov = new OneBodySOInt(aoov);

  sobas = soer->basis();
  sobas->print(cout << node0);

  if (keyval->exists(":shell")) {
    do_so_shell_test(sobas, soer,
                     keyval->intvalue(":shell",0),
                     keyval->intvalue(":shell",1),
                     keyval->intvalue(":shell",2),
                     keyval->intvalue(":shell",3));
    do_so_shell_test(sobas, soov,
                     keyval->intvalue(":shell",0),
                     keyval->intvalue(":shell",1));
    }
  else {
      int i,j,k,l;
      cout << "SO Electron Repulsion:" << endl;
      for (i=0; i<sobas->nshell(); i++) {
          for (j=0; j<sobas->nshell(); j++) {
              for (k=0; k<sobas->nshell(); k++) {
                  for (l=0; l<sobas->nshell(); l++) {
                      do_so_shell_test(sobas, soer, i, j, k, l);
                    }
                }
            }
        }
      cout << "SO Overlap:" << endl;
      for (i=0; i<sobas->nshell(); i++) {
          for (j=0; j<sobas->nshell(); j++) {
              do_so_shell_test(sobas, soov, i, j);
            }
        }
    }
}

static void
test_overlap(const RefGaussianBasisSet& gbs, const RefGaussianBasisSet& gbs2,
             const RefIntegral& intgrl)
{
  intgrl->set_basis(gbs);

  // first form AO basis overlap
  RefSymmSCMatrix s(gbs->basisdim(), gbs->matrixkit());
  RefSCElementOp ov = new OneBodyIntOp(new OneBodyIntIter(intgrl->overlap()));
  s.assign(0.0);
  s.element_op(ov);
  ov=0;
  s.print("overlap");
      
  // now transform s to SO basis
  RefPetiteList pl = intgrl->petite_list();
  RefSymmSCMatrix sb = pl->to_SO_basis(s);
  sb.print("blocked s");
      
  // and back to AO basis
  s = pl->to_AO_basis(sb);
  s.print("reconstituted s");

  // form skeleton overlap
  ov = new OneBodyIntOp(new SymmOneBodyIntIter(intgrl->overlap(),pl));
  s.assign(0.0);
  s.element_op(ov);
  ov=0;
  s.print("overlap");

  // and symmetrize to get blocked overlap again
  sb.assign(0.0);
  pl->symmetrize(s,sb);
  sb.print("blocked again");

  s=0; sb=0;

  // now try overlap between two basis sets
  RefSCMatrix ssq(gbs2->basisdim(),gbs->basisdim(),gbs2->matrixkit());
  intgrl->set_basis(gbs2,gbs);

  ov = new OneBodyIntOp(new OneBodyIntIter(intgrl->overlap()));
  ssq.assign(0.0);
  ssq.element_op(ov);
  ssq.print("overlap sq");
  ov=0;

  RefPetiteList pl2 = intgrl->petite_list(gbs2);
  RefSCMatrix ssqb(pl2->AO_basisdim(), pl->AO_basisdim(), gbs->so_matrixkit());
  ssqb->convert(ssq);

  RefSCMatrix syms2 = pl2->aotoso().t() * ssqb * pl->aotoso();
  syms2.print("symm S2");
}

static void
test_eigvals(const RefGaussianBasisSet& gbs, const RefIntegral& intgrl)
{
  intgrl->set_basis(gbs);
  RefPetiteList pl = intgrl->petite_list();

  // form AO Hcore and evecs
  RefSymmSCMatrix hcore_ao(gbs->basisdim(), gbs->matrixkit());
  RefSCMatrix ao_evecs(gbs->basisdim(), gbs->basisdim(), gbs->matrixkit());
  RefDiagSCMatrix ao_evals(gbs->basisdim(), gbs->matrixkit());
  
  hcore_ao.assign(0.0);

  RefSCElementOp op = new OneBodyIntOp(new OneBodyIntIter(intgrl->kinetic()));
  hcore_ao.element_op(op);
  op=0;

  RefOneBodyInt nuc = intgrl->nuclear();
  nuc->reinitialize();
  op = new OneBodyIntOp(nuc);
  hcore_ao.element_op(op);
  op=0;
  
  hcore_ao.print("Hcore (AO)");
  
  hcore_ao.diagonalize(ao_evals, ao_evecs);
  ao_evecs.print("AO Evecs");
  ao_evals.print("AO Evals");

  // form SO Hcore and evecs
  RefSymmSCMatrix hcore_so(pl->SO_basisdim(), gbs->so_matrixkit());
  RefSCMatrix so_evecs(pl->SO_basisdim(), pl->SO_basisdim(),
                       gbs->so_matrixkit());
  RefDiagSCMatrix so_evals(pl->SO_basisdim(), gbs->so_matrixkit());
  
  // reuse hcore_ao to get skeleton Hcore
  hcore_ao.assign(0.0);

  op = new OneBodyIntOp(new SymmOneBodyIntIter(intgrl->kinetic(),pl));
  hcore_ao.element_op(op);
  op=0;

  nuc = intgrl->nuclear();
  nuc->reinitialize();
  op = new OneBodyIntOp(new SymmOneBodyIntIter(nuc,pl));
  hcore_ao.element_op(op);
  op=0;
  
  pl->symmetrize(hcore_ao, hcore_so);

  hcore_so.print("Hcore (SO)");
  
  hcore_so.diagonalize(so_evals, so_evecs);
  so_evecs.print("SO Evecs");
  so_evals.print("SO Evals");

  RefSCMatrix new_ao_evecs = pl->evecs_to_AO_basis(so_evecs);
  new_ao_evecs.print("AO Evecs again");

  //RefSCMatrix new_so_evecs = pl->evecs_to_SO_basis(ao_evecs);
  //new_so_evecs.print("SO Evecs again");

  pl->to_AO_basis(hcore_so).print("Hcore (AO) again");
}

void
checkerror(const char *name, int shell, int func,
           double numerical, double check)
{
  double mag = fabs(check);
  double err = fabs(numerical - check);
  cout << scprintf("%2s %2d %2d %12.8f %12.8f er = %6.4f",
                   name, shell, func,
                   numerical, check, err/mag) << endl;
  if (mag > 0.001) {
      if (err/mag > 0.05) {
          cout << scprintf("ERROR %2s %2d %2d %12.8f %12.8f er = %6.4f",
                           name, shell, func,
                           numerical, check, err/mag) << endl;
        }
    }
  else if (err > 0.02) {
      cout << scprintf("ERROR %2s %2d %2d %12.8f %12.8f ea = %16.14f",
                       name, shell, func, numerical, check, err) << endl;
    }
}

void
test_func_values(const RefGaussianBasisSet &gbs)
{
  cout << "testing basis function value gradient and hessian numerically"
       << endl;

  int nbasis = gbs->nbasis();
  double *b_val = new double[nbasis];
  double *b_val_plsx = new double[nbasis];
  double *b_val_mnsx = new double[nbasis];
  double *b_val_plsy = new double[nbasis];
  double *b_val_mnsy = new double[nbasis];
  double *b_val_plsz = new double[nbasis];
  double *b_val_mnsz = new double[nbasis];
  double *b_val_plsyx = new double[nbasis];
  double *b_val_mnsyx = new double[nbasis];
  double *b_val_plszy = new double[nbasis];
  double *b_val_mnszy = new double[nbasis];
  double *b_val_plszx = new double[nbasis];
  double *b_val_mnszx = new double[nbasis];
  double *g_val = new double[3*nbasis];
  double *h_val = new double[6*nbasis];

  const int x_ = 0;
  const int y_ = 1;
  const int z_ = 2;
  const int xx_ = 0;
  const int yx_ = 1;
  const int yy_ = 2;
  const int zx_ = 3;
  const int zy_ = 4;
  const int zz_ = 5;

  SCVector3 r;
  SCVector3 d;
  double delta = 0.001;
  SCVector3 dx(delta, 0., 0.);
  SCVector3 dy(0., delta, 0.);
  SCVector3 dz(0., 0., delta);
  SCVector3 dxy(delta, delta, 0.);
  SCVector3 dxz(delta, 0., delta);
  SCVector3 dyz(0., delta, delta);
  double deltax = 0.1;
  for (r.x()=0.0; r.x() < 1.0; r.x() += deltax) {
      deltax *= 2.;
      double deltay = 0.1;
      for (r.y()=0.0; r.y() < 1.0; r.y() += deltay) {
          deltay *= 2.;
          double deltaz = 0.1;
          for (r.z()=0.0; r.z() < 1.0; r.z() += deltaz) {
              deltaz *= 2.;
              cout << "R = " << r << endl;
              gbs->hessian_values(r, h_val, g_val, b_val);
              gbs->values(r + dx, b_val_plsx);
              gbs->values(r - dx, b_val_mnsx);
              gbs->values(r + dy, b_val_plsy);
              gbs->values(r - dy, b_val_mnsy);
              gbs->values(r + dz, b_val_plsz);
              gbs->values(r - dz, b_val_mnsz);
              gbs->values(r + dxy, b_val_plsyx);
              gbs->values(r - dxy, b_val_mnsyx);
              gbs->values(r + dyz, b_val_plszy);
              gbs->values(r - dyz, b_val_mnszy);
              gbs->values(r + dxz, b_val_plszx);
              gbs->values(r - dxz, b_val_mnszx);
              for (int i=0; i<nbasis; i++) {
                  int shell = gbs->function_to_shell(i);
                  int func = i - gbs->shell_to_function(shell);
                  double g_val_test[3];
                  double h_val_test[6];
                  int x = i*3+x_;
                  int y = i*3+y_;
                  int z = i*3+z_;
                  g_val_test[x_] = 0.5*(b_val_plsx[i] - b_val_mnsx[i])/delta;
                  g_val_test[y_] = 0.5*(b_val_plsy[i] - b_val_mnsy[i])/delta;
                  g_val_test[z_] = 0.5*(b_val_plsz[i] - b_val_mnsz[i])/delta;
                  int xx = i*6+xx_;
                  int yx = i*6+yx_;
                  int yy = i*6+yy_;
                  int zx = i*6+zx_;
                  int zy = i*6+zy_;
                  int zz = i*6+zz_;
                  h_val_test[xx_]
                      = (b_val_plsx[i] + b_val_mnsx[i] - 2. * b_val[i])
                        * 1./(delta*delta);
                  h_val_test[yy_]
                      = (b_val_plsy[i] + b_val_mnsy[i] - 2. * b_val[i])
                        * 1./(delta*delta);
                  h_val_test[zz_]
                      = (b_val_plsz[i] + b_val_mnsz[i] - 2. * b_val[i])
                        * 1./(delta*delta);
                  h_val_test[yx_]
                      = 0.5 * ((b_val_plsyx[i]+b_val_mnsyx[i]-2.*b_val[i])
                               * 1. / (delta * delta)
                               - h_val_test[xx_] - h_val_test[yy_]);
                  h_val_test[zx_]
                      = 0.5 * ((b_val_plszx[i]+b_val_mnszx[i]-2.*b_val[i])
                               * 1. / (delta * delta)
                               - h_val_test[xx_] - h_val_test[zz_]);
                  h_val_test[zy_]
                      = 0.5 * ((b_val_plszy[i]+b_val_mnszy[i]-2.*b_val[i])
                               * 1. / (delta * delta)
                               - h_val_test[zz_] - h_val_test[yy_]);
                  checkerror("x", shell, func, g_val_test[x_], g_val[x]);
                  checkerror("y", shell, func, g_val_test[y_], g_val[y]);
                  checkerror("z", shell, func, g_val_test[z_], g_val[z]);
                  checkerror("xx", shell, func, h_val_test[xx_], h_val[xx]);
                  checkerror("yy", shell, func, h_val_test[yy_], h_val[yy]);
                  checkerror("zz", shell, func, h_val_test[zz_], h_val[zz]);
                  checkerror("yx", shell, func, h_val_test[yx_], h_val[yx]);
                  checkerror("zx", shell, func, h_val_test[zx_], h_val[zx]);
                  checkerror("zy", shell, func, h_val_test[zy_], h_val[zy]);
                }
            }
        }
    }
  delete[] b_val;
  delete[] b_val_plsx;
  delete[] b_val_mnsx;
  delete[] b_val_plsy;
  delete[] b_val_mnsy;
  delete[] b_val_plsz;
  delete[] b_val_mnsz;
  delete[] b_val_plsyx;
  delete[] b_val_mnsyx;
  delete[] b_val_plszy;
  delete[] b_val_mnszy;
  delete[] b_val_plszx;
  delete[] b_val_mnszx;
  delete[] g_val;
  delete[] h_val;
}

void
do_extent_test(const RefGaussianBasisSet &gbs)
{
  int i, j;
  for (i=0; i<gbs->nshell(); i++) {
      gbs->shell(i).print();
      for (j=0; j<10; j++) {
          cout << " " << gbs->shell(i).monobound(0.1*j);
        }
      cout << endl;
      for (j=0; j<10; j++) {
          double threshold = pow(10.0, -j);
          //cout << " threshold = " << threshold << endl;
          cout << " " << gbs->shell(i).extent(threshold);
        }
      cout << endl;
    }

  RefShellExtent extent = new ShellExtent;
  extent->init(gbs);
  extent->print();
}

int
main(int, char *argv[])
{
  int i, j;

  char o[10000];
  ostrstream perlout(o,sizeof(o));

  char *filename = (argv[1]) ? argv[1] : SRCDIR "/btest.kv";
  
  RefKeyVal keyval = new ParsedKeyVal(filename);
  
  RefIntegral intgrl = new IntegralV3;

  int dooverlap = keyval->booleanvalue("overlap");
  int doeigvals = keyval->booleanvalue("eigvals");
  int dostate = keyval->booleanvalue("state");
  int doso = keyval->booleanvalue("so");
  int doatoms = keyval->booleanvalue("atoms");
  int dopetite = keyval->booleanvalue("petite");
  int dovalues = keyval->booleanvalue("values");
  int doextent = keyval->booleanvalue("extent");

  for (i=0; i<keyval->count("test"); i++) {
      RefGaussianBasisSet gbs = keyval->describedclassvalue("test", i);
      RefGaussianBasisSet gbs2 = keyval->describedclassvalue("test2", i);

      if (dooverlap) test_overlap(gbs,gbs2,intgrl);

      if (doeigvals) test_eigvals(gbs,intgrl);

      if (dostate) {
          StateOutBin out("btest.out");
          gbs.save_state(out);
          out.close();
          StateInBin in("btest.out");
          gbs.restore_state(in);
          gbs->print();
        }

      if (dopetite) {
          intgrl->set_basis(gbs);
          intgrl->petite_list()->print();
        }

      if (doso) {
          do_so_test(keyval, intgrl, gbs);
        }

      if (dovalues) {
          intgrl->set_basis(gbs);
          gbs->set_integral(intgrl);
          test_func_values(gbs);
        }

      if (doextent) {
          do_extent_test(gbs);
        }
    }

  if (doatoms) {
      const int nelem = 37;

      // Make H, C, and P molecules
      RefMolecule hmol = new Molecule();
      hmol->add_atom(AtomInfo::string_to_Z("H"),0,0,0);
      RefMolecule cmol = new Molecule();
      cmol->add_atom(AtomInfo::string_to_Z("C"),0,0,0);
      RefMolecule pmol = new Molecule();
      pmol->add_atom(AtomInfo::string_to_Z("P"),0,0,0);

      perlout << "%basissets = (" << endl;
      int nbasis = keyval->count("basislist");
      RefKeyVal nullkv = new AssignedKeyVal();
      for (i=0; i<nbasis; i++) {
          int first_element = 1;
          char *basisname = keyval->pcharvalue("basislist",i);
          perlout << "  \"" << basisname << "\" => (";
          BasisFileSet bfs(nullkv);
          RefKeyVal basiskv = bfs.keyval(nullkv, basisname);
          char elemstr[512];
          elemstr[0] = '\0';
          int last_elem_exists = 0;
          int n0 = 0;
          int n1 = 0;
          int n2 = 0;
          for (j=0; j<nelem; j++) {
              RefAssignedKeyVal atombaskv_a(new AssignedKeyVal());
              RefKeyVal atombaskv(atombaskv_a);
              char keyword[256];
              strcpy(keyword,":basis:");
              strcat(keyword,AtomInfo::name(j+1));
              strcat(keyword,":");
              strcat(keyword,basisname);
              if (basiskv->exists(keyword)) {
                  if (!first_element) {
                      perlout << ",";
                    }
                  else {
                      first_element = 0;
                    }
                  perlout << "\"" << AtomInfo::symbol(j+1) << "\"";
                  if (!last_elem_exists) {
                      if (elemstr[0] != '\0') strcat(elemstr,", ");
                      strcat(elemstr,AtomInfo::symbol(j+1));
                    }
                  else if (last_elem_exists == 2) {
                      strcat(elemstr,"-");
                    }
                  last_elem_exists++;
                  if (j+1 == 1) {
                      atombaskv_a->assign("name", basisname);
                      atombaskv_a->assign("molecule", hmol);
                      RefGaussianBasisSet gbs=new GaussianBasisSet(atombaskv);
                      n0 = gbs->nbasis();
                    }
                  if (j+1 == 6) {
                      atombaskv_a->assign("name", basisname);
                      atombaskv_a->assign("molecule", cmol);
                      RefGaussianBasisSet gbs=new GaussianBasisSet(atombaskv);
                      n1 = gbs->nbasis();
                    }
                  if (j+1 == 15) {
                      atombaskv_a->assign("name", basisname);
                      atombaskv_a->assign("molecule", pmol);
                      RefGaussianBasisSet gbs=new GaussianBasisSet(atombaskv);
                      n2 = gbs->nbasis();
                    }
                }
              else {
                  if (last_elem_exists > 1) {
                      if (last_elem_exists == 2) strcat(elemstr,", ");
                      strcat(elemstr, AtomInfo::symbol(j));
                    }
                  last_elem_exists = 0;
                }
            }
          perlout << ")";
          if (i != nbasis-1) perlout << "," << endl;
          perlout << endl;
          cout << "<tr><td><tt>" << basisname
               << "</tt><td>" << elemstr << "<td>";
          if (n0>0) cout << n0;
          cout << "<td>";
          if (n1>0) cout << n1;
          cout << "<td>";
          if (n2>0) cout << n2;
          cout << endl;
          delete[] basisname;
        }

      perlout << ")" << endl << ends;

      char *perlout_s = perlout.str();
      cout << perlout_s;
    }

  return 0;
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
