//
// coor.cc
//
// Copyright (C) 1996 Limit Point Systems, Inc.
//
// Author: Curtis Janssen <cljanss@limitpt.com>
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

#ifdef __GNUC__
#pragma implementation
#endif

#include <math.h>

#include <util/misc/formio.h>
#include <util/state/stateio.h>
#include <math/scmat/matrix.h>
#include <chemistry/molecule/molecule.h>
#include <chemistry/molecule/coor.h>
#include <chemistry/molecule/simple.h>
#include <chemistry/molecule/localdef.h>

#include <util/container/bitarray.h>
#include <util/container/avlset.h>

////////////////////////////////////////////////////////////////////////////

#define CLASSNAME IntCoor
#define PARENTS public SavableState
#include <util/state/statei.h>
#include <util/class/classia.h>

#define CLASSNAME SetIntCoor
#define PARENTS public SavableState
#define HAVE_CTOR
#define HAVE_KEYVAL_CTOR
#define HAVE_STATEIN_CTOR
#include <util/state/statei.h>
#include <util/class/classi.h>

#define CLASSNAME IntCoorGen
#define VERSION 2
#define PARENTS public SavableState
#define HAVE_KEYVAL_CTOR
#define HAVE_STATEIN_CTOR
#include <util/state/statei.h>
#include <util/class/classi.h>

#define CLASSNAME SumIntCoor
#define PARENTS public IntCoor
#define HAVE_KEYVAL_CTOR
#define HAVE_STATEIN_CTOR
#include <util/state/statei.h>
#include <util/class/classi.h>

#define CLASSNAME MolecularCoor
#define PARENTS public SavableState
#include <util/state/statei.h>
#include <util/class/classia.h>

///////////////////////////////////////////////////////////////////////////
// members of IntCoor

double IntCoor::bohr_conv = 0.52917706;
double IntCoor::radian_conv = 180.0/3.14159265358979323846;

SavableState_REF_def(IntCoor);
ARRAY_def(RefIntCoor);

void *
IntCoor::_castdown(const ClassDesc*cd)
{
  void* casts[1];
  casts[0] = SavableState::_castdown(cd);
  return do_castdowns(casts,cd);
}

IntCoor::IntCoor(const char *re):
  label_(0), value_(0.0)
{
  if (!re) re = "noname";
  label_=new char[strlen(re)+1]; strcpy(label_,re);
}

IntCoor::IntCoor(const IntCoor& c):
  label_(0)
{
  value_ = c.value_;
  if (c.label_) label_ = strcpy(new char[strlen(c.label_)+1],c.label_);
}

IntCoor::IntCoor(const RefKeyVal&keyval)
{
  label_ = keyval->pcharvalue("label");
  value_ = keyval->doublevalue("value");

  char* unit = keyval->pcharvalue("unit");
  if (unit) {
      if (!strcmp(unit, "bohr")) {
        }
      else if (!strcmp(unit, "angstrom")) {
          value_ /= bohr_conv;
        }
      else if (!strcmp(unit, "radian")) {
        }
      else if (!strcmp(unit, "degree")) {
          value_ *= M_PI/180.0;
        }
      else {
          cerr << node0 << indent
               << "IntCoor::IntCoor(KeyVal): unknown unit = \""
               << unit << "\"\n";
          abort();
        }
      delete[] unit;
    }
}

IntCoor::IntCoor(StateIn& si):
  SavableState(si)
{
  si.get(value_);
  si.getstring(label_);
}

IntCoor::~IntCoor()
{
  if (label_) delete[] label_;
}

void
IntCoor::save_data_state(StateOut& so)
{
  so.put(value_);
  so.putstring(label_);
}

const char*
IntCoor::label() const
{
  return label_;
}

double
IntCoor::value() const
{
  return value_;
}

void
IntCoor::set_value(double v)
{
  value_ = v;
}

void
IntCoor::print(ostream &o) const
{
  print_details(0,o);
}

void
IntCoor::print_details(const RefMolecule &mol, ostream& os) const
{
  os.setf(ios::fixed,ios::floatfield);
  os.precision(10);
  os.setf(ios::left,ios::adjustfield);
  os.width(10);

  os << node0 << indent
     << scprintf("%-5s \"%10s\" %15.10f\n",ctype(),label(),preferred_value());
}

double
IntCoor::preferred_value() const
{
  return value_;
}

///////////////////////////////////////////////////////////////////////////
// members of SetIntCoor

SavableState_REF_def(SetIntCoor);

void *
SetIntCoor::_castdown(const ClassDesc*cd)
{
  void* casts[1];
  casts[0] = SavableState::_castdown(cd);
  return do_castdowns(casts,cd);
}

SetIntCoor::SetIntCoor()
{
}

SetIntCoor::SetIntCoor(const RefKeyVal& keyval)
{
  int n = keyval->count();

  RefIntCoorGen gen = keyval->describedclassvalue("generator");

  if (gen.null() && !n) {
      cerr << node0 << indent << "SetIntCoor::SetIntCoor: bad input\n";
      abort();
    }

  if (gen.nonnull()) {
      // Make sure that gen doesn't delete me before my reference
      // count gets incremented.
      this->reference();
      gen->generate(this);
      // Now it is safe to decrement my reference count back down to zero.
      this->dereference();
    }

  for (int i=0; i<n; i++) {
      coor_.push_back(keyval->describedclassvalue(i));
    }
}

SetIntCoor::SetIntCoor(StateIn& s):
  SavableState(s)
{
  int n;
  s.get(n);

  RefIntCoor tmp;
  for (int i=0; i<n; i++) {
      tmp.restore_state(s);
      coor_.push_back(tmp);
    }
}

SetIntCoor::~SetIntCoor()
{
}

void
SetIntCoor::save_data_state(StateOut& s)
{
  int n = coor_.size();
  s.put(n);

  for (int i=0; i<n; i++) {
      coor_[i].save_state(s);
    }
}

void
SetIntCoor::add(const RefIntCoor& coor)
{
  coor_.push_back(coor);
}

void
SetIntCoor::add(const RefSetIntCoor& coor)
{
  for (int i=0; i<coor->n(); i++) {
      coor_.push_back(coor->coor(i));
    }
}

void
SetIntCoor::pop()
{
  coor_.pop_back();
}

int
SetIntCoor::n() const
{
  return coor_.size();
}

RefIntCoor
SetIntCoor::coor(int i) const
{
  return coor_[i];
}

// compute the bmatrix by finite displacements
void
SetIntCoor::fd_bmat(const RefMolecule& mol,RefSCMatrix& fd_bmatrix)
{
  RefSCMatrixKit kit = fd_bmatrix.kit();

  fd_bmatrix.assign(0.0);
  
  int i;
  Molecule& m = * mol.pointer();

  const double cart_disp = 0.01;

  RefSCDimension dn3(fd_bmatrix.coldim());
  RefSCDimension dnc(fd_bmatrix.rowdim());
  int n3 = dn3.n();
  int nc = dnc.n();
  RefSCVector internal(dnc,kit);
  RefSCVector internal_p(dnc,kit);
  RefSCVector internal_m(dnc,kit);

  // the internal coordinates
  update_values(mol);
  for (i=0; i<nc; i++) {
      internal(i) = coor_[i]->value();
    }

  // the finite displacement bmat
  for (i=0; i<n3; i++) {
      // the plus displacement
      m.r(i/3,i%3) += cart_disp;
      update_values(mol);
      int j;
      for (j=0; j<nc; j++) {
          internal_p(j) = coor_[j]->value();
        }
      // the minus displacement
      m.r(i/3,i%3) -= 2.0*cart_disp;
      update_values(mol);
      for (j=0; j<nc; j++) {
          internal_m(j) = coor_[j]->value();
        }
      // reset the cartesian coordinate to its original value
      m.r(i/3,i%3) += cart_disp;

      // construct the entries in the finite displacement bmat
      for (j=0; j<nc; j++) {
          fd_bmatrix(j,i) = (internal_p(j)-internal_m(j))/(2.0*cart_disp);
        }
    }
}

void
SetIntCoor::bmat(const RefMolecule& mol, RefSCMatrix& bmat)
{
  bmat.assign(0.0);

  int i, ncoor = n();

  RefSCVector bmatrow(bmat.coldim(),bmat.kit());
  // send the rows of the b matrix to each of the coordinates
  for (i=0; i<ncoor; i++) {
      bmatrow.assign(0.0);
      coor_[i]->bmat(mol,bmatrow);
      bmat.assign_row(bmatrow,i);
    }
}

void
SetIntCoor::guess_hessian(RefMolecule& mol,RefSymmSCMatrix& hessian)
{
  int ncoor = hessian.n();

  hessian.assign(0.0);
  for (int i=0; i<ncoor; i++) {
      hessian(i,i) = coor_[i]->force_constant(mol);
    }
}

void
SetIntCoor::print_details(const RefMolecule &mol, ostream& os) const
{
  int i;

  for(i=0; i<coor_.size(); i++) {
      coor_[i]->print_details(mol,os);
    }
}

void
SetIntCoor::update_values(const RefMolecule&mol)
{
  for (int i=0; i<coor_.size(); i++) {
      coor_[i]->update_value(mol);
    }
}

void
SetIntCoor::values_to_vector(const RefSCVector&v)
{
  for (int i=0; i<coor_.size(); i++) {
      v(i) = coor_[i]->value();
    }
}  

void
SetIntCoor::clear()
{
  coor_.clear();
}

///////////////////////////////////////////////////////////////////////////
// members of SumIntCoor

void *
SumIntCoor::_castdown(const ClassDesc*cd)
{
  void* casts[1];
  casts[0] = IntCoor::_castdown(cd);
  return do_castdowns(casts,cd);
}

SumIntCoor::SumIntCoor(const char* label):
  IntCoor(label)
{
}

SumIntCoor::SumIntCoor(const RefKeyVal&keyval):
  IntCoor(keyval)
{
  static const char* coor = "coor";
  static const char* coef = "coef";
  int n = keyval->count(coor);
  int ncoef = keyval->count(coef);
  if (n != ncoef || !n) {
      cerr << node0 << indent << "SumIntCoor::SumIntCoor: bad input\n";
      abort();
    }

  for (int i=0; i<n; i++) {
      double coe = keyval->doublevalue(coef,i);
      RefIntCoor coo = keyval->describedclassvalue(coor,i);
      add(coo,coe);
    }
}

SumIntCoor::SumIntCoor(StateIn&s):
  IntCoor(s)
{
  int n;
  s.get(n);

  coef_.set_length(n);
  coor_.set_length(n);
  for (int i=0; i<n; i++) {
      s.get(coef_[i]);
      coor_[i].restore_state(s);
    }
}

SumIntCoor::~SumIntCoor()
{
}

void
SumIntCoor::save_data_state(StateOut&s)
{
  int n = coef_.size();
  IntCoor::save_data_state(s);
  s.put(coef_.size());

  for (int i=0; i<n; i++) {
      s.put(coef_[i]);
      coor_[i].save_state(s);
    }
}

int
SumIntCoor::n()
{
  return coef_.size();
}

void
SumIntCoor::add(RefIntCoor&coor,double coef)
{
  // if a sum is added to a sum, unfold the nested sum
  SumIntCoor* scoor = SumIntCoor::castdown(coor.pointer());
  if (scoor) {
      int l = scoor->coor_.size();
      for (int i=0; i<l; i++) {
          add(scoor->coor_[i],coef * scoor->coef_[i]);
        }
    }
  else {
      int l = coef_.size();
      for (int i=0; i<l; i++) {
          if (coor_[i]->equivalent(coor)) {
              coef_[i] += coef;
              return;
            }
        }
      coef_.resize(l+1);
      coor_.resize(l+1);
      coef_[l] = coef;
      coor_[l] = coor;
    }
}

int
SumIntCoor::equivalent(RefIntCoor&c)
{
  return 0;
}

// this normalizes and makes the biggest coordinate positive
void
SumIntCoor::normalize()
{
  int i;
  int n = coef_.size();
  double norm = 0.0;

  double biggest = 0.0;
  for (i=0; i<n; i++) {
      norm += coef_[i] * coef_[i];
      if (fabs(biggest) < fabs(coef_[i])) biggest = coef_[i];
    }
  norm = (biggest < 0.0? -1.0:1.0)/sqrt(norm);

  for (i=0; i<n; i++) {
      coef_[i] = coef_[i]*norm;
    }
}

double
SumIntCoor::preferred_value() const
{
  return value_;
}

const char*
SumIntCoor::ctype() const
{
  return "SUM";
}

void
SumIntCoor::print_details(const RefMolecule &mol, ostream& os) const
{
  int initial_indent = SCFormIO::getindent(os);
  int i;

  os << node0 << indent
     << scprintf("%-5s %10s %14.10f\n",ctype(),
                 (label()?label():""), preferred_value());

  for(i=0; i<coor_.size(); i++) {
      os << node0 << incindent
         << indent << scprintf("%14.10f ",coef_[i]);

      SCFormIO::setindent(os, SCFormIO::getindent(os) + 15);
      os << node0 << skipnextindent;
      coor_[i]->print_details(mol,os);
      SCFormIO::setindent(os, initial_indent);
    }
}

// the SumIntCoor should be normalized before this is called.
double
SumIntCoor::force_constant(RefMolecule&molecule)
{
  double fc = 0.0;
  
  for (int i=0; i<n(); i++) {
      fc += coef_[i] * coef_[i] * coor_[i]->force_constant(molecule);
    }

  return fc;
}

void
SumIntCoor::update_value(const RefMolecule&molecule)
{
  int i, l = n();

  value_ = 0.0;
  for (i=0; i<l; i++) {
      coor_[i]->update_value(molecule);
#if OLD_BMAT
      if (StreSimpleCo::castdown(coor_[i]))
        value_ += coef_[i] * StreSimpleCo::castdown(coor_[i])->angstrom();
      else
#endif        
      value_ += coef_[i] * coor_[i]->value();
    }
}

void
SumIntCoor::bmat(const RefMolecule&molecule,RefSCVector&bmat,double coef)
{
  int i, l = n();
  
  for (i=0; i<l; i++) {
      coor_[i]->bmat(molecule,bmat,coef*coef_[i]);
    }
}

///////////////////////////////////////////////////////////////////////////
// members of MolecularCoor

SavableState_REF_def(MolecularCoor);

void *
MolecularCoor::_castdown(const ClassDesc*cd)
{
  void* casts[1];
  casts[0] = SavableState::_castdown(cd);
  return do_castdowns(casts,cd);
}

MolecularCoor::MolecularCoor(RefMolecule&mol):
  molecule_(mol)
{
  debug_ = 0;
  matrixkit_ = SCMatrixKit::default_matrixkit();
  dnatom3_ = new SCDimension(3*molecule_->natom());
}

MolecularCoor::MolecularCoor(const RefKeyVal&keyval)
{
  molecule_ = keyval->describedclassvalue("molecule");

  if (molecule_.null()) {
      cerr << node0 << indent
           << "MolecularCoor(const RefKeyVal&keyval): molecule not found\n";
      abort();
    }

  debug_ = keyval->intvalue("debug");

  matrixkit_ = keyval->describedclassvalue("matrixkit");
  dnatom3_ = keyval->describedclassvalue("natom3");

  if (matrixkit_.null()) matrixkit_ = SCMatrixKit::default_matrixkit();

  if (dnatom3_.null()) dnatom3_ = new SCDimension(3*molecule_->natom());
  else if (dnatom3_->n() != 3 * molecule_->natom()) {
      cerr << node0 << indent << "MolecularCoor(KeyVal): bad dnatom3 value\n";
      abort();
    }
}

MolecularCoor::MolecularCoor(StateIn&s):
  SavableState(s)
{
  debug_ = 0;
  matrixkit_ = SCMatrixKit::default_matrixkit();
  molecule_.restore_state(s);
  dnatom3_.restore_state(s);
}

MolecularCoor::~MolecularCoor()
{
}

void
MolecularCoor::save_data_state(StateOut&s)
{
  molecule_.save_state(s);
  dnatom3_.save_state(s);
}

int
MolecularCoor::nconstrained()
{
  return 0;
}

// The default action is to never change the coordinates.
RefNonlinearTransform
MolecularCoor::change_coordinates()
{
  return 0;
}

int
MolecularCoor::to_cartesian(const RefSCVector&internal)
{
  return to_cartesian(molecule_, internal);
}

///////////////////////////////////////////////////////////////////////////
// members of IntCoorGen

SavableState_REF_def(SetIntCoor);

void *
IntCoorGen::_castdown(const ClassDesc*cd)
{
  void* casts[1];
  casts[0] = SavableState::_castdown(cd);
  return do_castdowns(casts,cd);
}

IntCoorGen::IntCoorGen(const RefMolecule& mol,
                       int nextra_bonds, int *extra_bonds)
{
  molecule_ = mol;
  nextra_bonds_ = nextra_bonds;
  extra_bonds_ = extra_bonds;
  radius_scale_factor_ = 1.1;
  linear_bend_thres_ = cos(5.0*M_PI/360.0);
  linear_tors_thres_ = cos(5.0*M_PI/360.0);
  linear_bends_ = 0;
  linear_lbends_ = 1;
  linear_tors_ = 0;
  linear_stors_ = 1;
}

IntCoorGen::IntCoorGen(const RefKeyVal& keyval)
{
  molecule_ = keyval->describedclassvalue("molecule");

  radius_scale_factor_ = keyval->doublevalue("radius_scale_factor");
  if (keyval->error() != KeyVal::OK) radius_scale_factor_ = 1.1;

  // degrees
  linear_bend_thres_ = keyval->doublevalue("linear_bend_threshold");
  if (keyval->error() != KeyVal::OK)
      linear_bend_thres_ = 5.0;

  // entered in degrees; stored as cos(theta)
  linear_tors_thres_ = keyval->doublevalue("linear_tors_threshold");
  if (keyval->error() != KeyVal::OK)
      linear_tors_thres_ = 5.0;

  linear_bends_ = keyval->booleanvalue("linear_bend");
  if (keyval->error() != KeyVal::OK) linear_bends_ = 0;

  linear_lbends_ = keyval->booleanvalue("linear_lbend");
  if (keyval->error() != KeyVal::OK) linear_lbends_ = 1;

  linear_tors_ = keyval->booleanvalue("linear_tors");
  if (keyval->error() != KeyVal::OK) linear_tors_ = 0;

  linear_stors_ = keyval->booleanvalue("linear_stors");
  if (keyval->error() != KeyVal::OK) linear_stors_ = 1;

  // the extra_bonds list is given as a vector of atom numbers
  // (atom numbering starts at 1)
  nextra_bonds_ = keyval->count("extra_bonds");
  nextra_bonds_ /= 2;
  if (nextra_bonds_) {
      extra_bonds_ = new int[nextra_bonds_*2];
      for (int i=0; i<nextra_bonds_*2; i++) {
          extra_bonds_[i] = keyval->intvalue("extra_bonds",i);
          if (keyval->error() != KeyVal::OK) {
              cerr << node0 << indent
                   << "IntCoorGen:: keyval CTOR: problem reading "
                   << "\"extra_bonds:" << i << "\"\n";
              abort();
            }
        }
    }
  else {
      extra_bonds_ = 0;
    }
}

IntCoorGen::IntCoorGen(StateIn& s):
  SavableState(s)
{
  molecule_.restore_state(s);
  s.get(linear_bends_);
  if (s.version(static_class_desc()) >= 2) {
      s.get(linear_lbends_);
    }
  s.get(linear_tors_);
  s.get(linear_stors_);
  s.get(linear_bend_thres_);
  s.get(linear_tors_thres_);
  s.get(nextra_bonds_);
  s.get(extra_bonds_);
  s.get(radius_scale_factor_);
}

IntCoorGen::~IntCoorGen()
{
  if (extra_bonds_) delete[] extra_bonds_;
}

void
IntCoorGen::save_data_state(StateOut& s)
{
  molecule_.save_state(s);
  s.put(linear_bends_);
  s.put(linear_lbends_);
  s.put(linear_tors_);
  s.put(linear_stors_);
  s.put(linear_bend_thres_);
  s.put(linear_tors_thres_);
  s.put(nextra_bonds_);
  s.put(extra_bonds_,2*nextra_bonds_);
  s.put(radius_scale_factor_);
}

void
IntCoorGen::print(ostream& out) const
{
  out << node0 << indent << "IntCoorGen:" << endl << incindent
      << indent << "linear_bends = " << linear_bends_ << endl
      << indent << "linear_lbends = " << linear_lbends_ << endl
      << indent << "linear_tors = " << linear_tors_ << endl
      << indent << "linear_stors = " << linear_stors_ << endl
      << indent << scprintf("linear_bend_threshold = %f\n",linear_bend_thres_)
      << indent << scprintf("linear_tors_threshold = %f\n",linear_tors_thres_)
      << indent << scprintf("radius_scale_factor = %f\n",radius_scale_factor_)
      << indent << "nextra_bonds = " << nextra_bonds_ << endl
      << decindent;
}

static
find_bonds(Molecule &m, BitArrayLTri &bonds,
           double radius_scale_factor_)
{
  int i, j;
  for(i=0; i < m.natom(); i++) {
    double at_rad_i = m.atominfo()->atomic_radius(m.Z(i));
    SCVector3 ri(m.r(i));

    for(j=0; j < i; j++) {
      double at_rad_j = m.atominfo()->atomic_radius(m.Z(j));
      SCVector3 rj(m.r(j));

      if (ri.dist(rj)
          < radius_scale_factor_*(at_rad_i+at_rad_j))
        bonds.set(i,j);
      }
    }

  // check for groups of atoms bound to nothing
  AVLSet<int> boundatoms;
  AVLSet<int> newatoms, nextnewatoms;
  // start out with atom 0
  newatoms.insert(0);
  AVLSet<int>::iterator iatom;
  int warning_printed = 0;
  while (newatoms.length() > 0) {
    while (newatoms.length() > 0) {
      // newatoms gets merged into boundatoms
      for (iatom=newatoms.begin(); iatom!=newatoms.end(); iatom++) {
        boundatoms.insert(*iatom);
        }
      // set nextnewatoms to atoms bound to boundatoms that are not already
      // in boundatoms
      nextnewatoms.clear();
      for (iatom=newatoms.begin(); iatom!=newatoms.end(); iatom++) {
        int atom = *iatom;
        for (i=0; i<m.natom(); i++) {
          if (bonds(i,atom) && !boundatoms.contains(i)) {
            nextnewatoms.insert(i);
            }
          }
        }
      // set newatoms to nextnewatoms to start off the next iteration
      newatoms.clear();
      for (iatom=nextnewatoms.begin(); iatom!=nextnewatoms.end(); iatom++) {
        newatoms.insert(*iatom);
        }
      }

    if (boundatoms.length() != m.natom()) {
      if (!warning_printed) {
        warning_printed = 1;
        cout << node0
             << indent << "WARNING: two unbound groups of atoms" << endl
             << indent << "         consider using extra_bonds input" << endl
             << endl;
        }
      // find an unbound group
      double nearest_dist;
      int nearest_bound = -1, nearest_unbound = -1;
      for(i=0; i < m.natom(); i++) {
        if (!boundatoms.contains(i)) {
          SCVector3 ri(m.r(i));
          for (iatom=boundatoms.begin(); iatom!=boundatoms.end(); iatom++) {
            SCVector3 rj(m.r(*iatom));
            double d = ri.dist(rj);
            if (nearest_unbound == -1 || d < nearest_dist) {
              nearest_dist = d;
              nearest_bound = *iatom;
              nearest_unbound = i;
              }
            }
          }
        }
      if (nearest_bound == -1) {
        cout << node0 << indent
             << "ERROR: impossible error generating coordinates"
             << endl;
        abort();
        }
      // add all bound atoms within a certain distance of nearest_unbound
      // --- should really do this for all atoms that nearest_unbound
      // --- is already bound to, too
      SCVector3 rnearest_unbound(m.r(nearest_unbound));
      for (iatom=boundatoms.begin(); iatom!=boundatoms.end(); iatom++) {
        SCVector3 r(m.r(*iatom));
        if (*iatom == nearest_bound
            || rnearest_unbound.dist(r) < 1.1 * nearest_dist) {
          cout << node0 << indent
               << "         adding bond between "
               << *iatom+1 << " and " << nearest_unbound+1 << endl;
          bonds.set(*iatom,nearest_unbound);
          }
        }
      newatoms.insert(nearest_unbound);
      }
    }
}

void
IntCoorGen::generate(const RefSetIntCoor& sic)
{
  int i;
  Molecule& m = *molecule_.pointer();

  // let's go through the geometry and find all the close contacts
  // bonds is a lower triangle matrix of 1's and 0's indicating whether
  // there is a bond between atoms i and j

  BitArrayLTri bonds(m.natom(),m.natom());

  for (i=0; i<nextra_bonds_; i++) {
    bonds.set(extra_bonds_[i*2]-1,extra_bonds_[i*2+1]-1);
  }

  find_bonds(m, bonds, radius_scale_factor_);
      
  // compute the simple internal coordinates by type
  add_bonds(sic,bonds,m);
  add_bends(sic,bonds,m);
  add_tors(sic,bonds,m);
  add_out(sic,bonds,m);

  cout << node0 << endl << indent
       << "IntCoorGen: generated " << sic->n() << " coordinates." << endl;
}

///////////////////////////////////////////////////////////////////////////
// auxillary functions of IntCoorGen

/*
 * the following are translations of functions written by Gregory Humphreys
 * at the NIH
 */

/*
 * for each bonded pair, add an entry to the simple coord list
 */

void
IntCoorGen::add_bonds(const RefSetIntCoor& list, BitArrayLTri& bonds, Molecule& m)
{
  int i,j,ij;
  int labelc=0;
  char label[80];

  for(i=ij=0; i < m.natom(); i++) {
    for(j=0; j <= i; j++,ij++) {
      if(bonds[ij]) {
        labelc++;
        sprintf(label,"s%d",labelc);
        list->add(new Stre(label,j+1,i+1));
        }
      }
    }
  }

/*
 * return 1 if all three atoms are nearly on the same line.
 */

// returns fabs(cos(theta_ijk))
double
IntCoorGen::cos_ijk(Molecule& m, int i, int j, int k)
{
  SCVector3 a, b, c;
  int xyz;
  for (xyz=0; xyz<3; xyz++) {
      a[xyz] = m.r(i,xyz);
      b[xyz] = m.r(j,xyz);
      c[xyz] = m.r(k,xyz);
    }
  SCVector3 ab = a - b;
  SCVector3 cb = c - b;
  return fabs(ab.dot(cb)/(ab.norm()*cb.norm()));
}

void
IntCoorGen::add_bends(const RefSetIntCoor& list, BitArrayLTri& bonds, Molecule& m)
{
  int i,j,k;
  int labelc=0;
  char label[80];

  int n = m.natom();

  double thres = cos(linear_bend_thres_*M_PI/180.0);

  for(i=0; i < n; i++) {
    SCVector3 ri(m.r(i));
    for(j=0; j < n; j++) {
      if(bonds(i,j)) {
        SCVector3 rj(m.r(j));
        for(k=0; k < i; k++) {
          if(bonds(j,k)) {
            SCVector3 rk(m.r(k));
            int is_linear = (cos_ijk(m,i,j,k) >= thres);
            if (linear_bends_ || !is_linear) {
              labelc++;
              sprintf(label,"b%d",labelc);
              list->add(new Bend(label,k+1,j+1,i+1));
              }
            if (linear_lbends_ && is_linear) {
              // find a unit vector roughly perp to the bonds
              SCVector3 u;
              // first try to find another atom, that'll help keep one of
              // the coordinates totally symmetric in some cases
              int most_perp_atom = -1;
              double cos_most_perp = thres;
              for (int l=0; l < n; l++) {
                if (l == i || l == j || l == k) continue;
                double tmp = cos_ijk(m,i,j,l);
                if (tmp < cos_most_perp) {
                  cos_most_perp = tmp;
                  most_perp_atom = l;
                  }
                }
              if (most_perp_atom != -1) {
                SCVector3 rmpa(m.r(most_perp_atom));
                u = rj-rmpa;
                u.normalize();
                }
              else {
                SCVector3 b1, b2;
                b1 = ri-rj;
                b2 = rk-rj;
                u = b1.perp_unit(b2);
                }
              labelc++;
              sprintf(label,"b%d",labelc);
              list->add(new LinIP(label,k+1,j+1,i+1,u));
              labelc++;
              sprintf(label,"b%d",labelc);
              list->add(new LinOP(label,k+1,j+1,i+1,u));
              }
	    }
          }
	}
      }
    }
  }

/*
 * for each pair of bends which share a common bond, add a torsion
 */

/*
 * just look at the heavy-atom skeleton. return true if i is a terminal
 * atom.
 */

int
IntCoorGen::hterminal(Molecule& m, BitArrayLTri& bonds, int i)
{
  int nh=0;
  for (int j=0; j < m.natom(); j++)
    if (bonds(i,j) && m.Z(j) > 1) nh++;
  return (nh==1);
}

void
IntCoorGen::add_tors(const RefSetIntCoor& list, BitArrayLTri& bonds, Molecule& m)
{
  int i,j,k,l;
  int labelc=0;
  char label[80];

  int n = m.natom();

  double thres = cos(linear_tors_thres_*M_PI/180.0);

  for(j=0; j < n; j++) {
    for(k=0; k < j; k++) {
      if(bonds(j,k)) {
        for(i=0; i < n; i++) {
          if(k==i) continue;

         // no hydrogen torsions, ok?
	  if (m.Z(i) == 1 && !hterminal(m,bonds,j)) continue;

          if (bonds(j,i)) {
            int is_linear = 0;
	    if (cos_ijk(m,i,j,k)>=thres) is_linear = 1;

            for (l=0; l < n; l++) {
              if (l==j || l==i) continue;

             // no hydrogen torsions, ok?
	      if (m.Z(l) == 1 && !hterminal(m,bonds,k))
                continue;

              if (bonds(k,l)) {
		if(cos_ijk(m,j,k,l)>=thres) is_linear = 1;

                if (is_linear && linear_stors_) {
                    labelc++;
                    sprintf(label,"st%d",labelc);
                    list->add(new ScaledTors(label,l+1,k+1,j+1,i+1));
                  }
                if (!is_linear || linear_tors_) {
                    labelc++;
                    sprintf(label,"t%d",labelc);
                    list->add(new Tors(label,l+1,k+1,j+1,i+1));
                  }
		}
	      }
	    }
          }
	}
      }
    }
  }

void
IntCoorGen::add_out(const RefSetIntCoor& list, BitArrayLTri& bonds, Molecule& m)
{
  int i,j,k,l;
  int labelc=0;
  char label[80];

  int n = m.natom();

 // first find all tri-coordinate atoms
  for(i=0; i < n; i++) {
    if(bonds.degree(i)!=3) continue;

   // then look for terminal atoms connected to i
    for(j=0; j < n; j++) {
      if(bonds(i,j) && bonds.degree(j)==1) {

        for(k=0; k < n; k++) {
          if(k!=j && bonds(i,k)) {
            for(l=0; l < k; l++) {
              if(l!=j && bonds(i,l)) {
		labelc++;
		sprintf(label,"o%d",labelc);
		list->add(new Out(label,j+1,i+1,l+1,k+1));
		}
	      }
	    }
          }
	}
      }
    }
  }

int
IntCoorGen::nearest_contact(int i, Molecule& m)
{
  double d=-1.0;
  int n=0;

  SCVector3 ri(m.r(i));
  
  for (int j=0; j < m.natom(); j++) {
    SCVector3 rj(m.r(j));
    double td = ri.dist(rj);
    if (j==i)
      continue;
    else if (d < 0 || td < d) {
      d = td;
      n = j;
    }
  }
  
  return n;
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ-CONDENSED"
// End:
