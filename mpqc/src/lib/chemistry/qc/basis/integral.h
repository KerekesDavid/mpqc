
#ifndef _chemistry_qc_basis_integral_h
#define _chemistry_qc_basis_integral_h

#ifdef __GNUC__
#pragma interface
#endif

#include <util/state/state.h>
#include <chemistry/qc/basis/basis.h>
#include <chemistry/qc/basis/obint.h>
#include <chemistry/qc/basis/tbint.h>

class SymmetryOperation;
class RefPetiteList;
class RefSymmSCMatrix;
class ShellRotation;
class CartesianIter;
class RedundantCartesianIter;
class RedundantCartesianSubIter;
class SphericalTransformIter;
class PointBag_double;

SavableState_REF_fwddec(SCElementOp)
SavableState_REF_fwddec(SCElementOp3)


// some useful things to have that depend on the underlying integrals
// package

class Integral : public SavableState {
#   define CLASSNAME Integral
#   include <util/state/stated.h>
#   include <util/class/classda.h>
  protected:
    Integral();
    RefGaussianBasisSet bs1_;
    RefGaussianBasisSet bs2_;
    RefGaussianBasisSet bs3_;
    RefGaussianBasisSet bs4_;

    // the maximum number of bytes that should be used for
    // storing intermediates
    int storage_;

  public:
    Integral(StateIn&);
    Integral(const RefKeyVal&);
    
    void save_data_state(StateOut&);

    void set_storage(int i) { storage_=i; };

    RefPetiteList petite_list(const RefGaussianBasisSet&);
    ShellRotation shell_rotation(int am, SymmetryOperation&, int pure=0);

    virtual void set_basis(const RefGaussianBasisSet &b1,
                           const RefGaussianBasisSet &b2 = 0,
                           const RefGaussianBasisSet &b3 = 0,
                           const RefGaussianBasisSet &b4 = 0);

    ///////////////////////////////////////////////////////////////////////
    // the following must be defined in the specific integral package

    virtual CartesianIter * new_cartesian_iter(int) =0;
    virtual RedundantCartesianIter * new_redundant_cartesian_iter(int) =0;
    virtual RedundantCartesianSubIter *
                                 new_redundant_cartesian_sub_iter(int) =0;
    virtual SphericalTransformIter *
                              new_spherical_transform_iter(int, int=0) =0;
    
    virtual RefOneBodyInt overlap() =0;
    
    virtual RefOneBodyInt kinetic() =0;

    virtual RefOneBodyInt point_charge(const RefPointChargeData&) =0;

    // charges from the atom on the first center are used
    virtual RefOneBodyInt nuclear() = 0;

    virtual RefOneBodyInt efield_dot_vector(const RefEfieldDotVectorData&) =0;

    virtual RefOneBodyInt dipole(const RefDipoleData&) =0;

    virtual RefOneBodyDerivInt deriv() =0;
    
    virtual RefTwoBodyInt electron_repulsion() =0;
    
    virtual RefTwoBodyDerivInt electron_repulsion_deriv() =0;
};
SavableState_REF_dec(Integral);

#endif
