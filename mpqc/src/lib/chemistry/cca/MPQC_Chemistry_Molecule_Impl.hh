// 
// File:          MPQC_Chemistry_Molecule_Impl.hh
// Symbol:        MPQC.Chemistry_Molecule-v0.2
// Symbol Type:   class
// Babel Version: 0.8.6
// Description:   Server-side implementation for MPQC.Chemistry_Molecule
// 
// WARNING: Automatically generated; only changes within splicers preserved
// 
// babel-version = 0.8.6
// 

#ifndef included_MPQC_Chemistry_Molecule_Impl_hh
#define included_MPQC_Chemistry_Molecule_Impl_hh

#ifndef included_sidl_cxx_hh
#include "sidl_cxx.hh"
#endif
#ifndef included_MPQC_Chemistry_Molecule_IOR_h
#include "MPQC_Chemistry_Molecule_IOR.h"
#endif
// 
// Includes for all method dependencies.
// 
#ifndef included_MPQC_Chemistry_Molecule_hh
#include "MPQC_Chemistry_Molecule.hh"
#endif
#ifndef included_Physics_PointGroup_hh
#include "Physics_PointGroup.hh"
#endif
#ifndef included_Physics_Units_hh
#include "Physics_Units.hh"
#endif
#ifndef included_sidl_BaseInterface_hh
#include "sidl_BaseInterface.hh"
#endif
#ifndef included_sidl_ClassInfo_hh
#include "sidl_ClassInfo.hh"
#endif
#ifndef included_gov_cca_Services_hh
#include "gov_cca_Services.hh"
#endif


// DO-NOT-DELETE splicer.begin(MPQC.Chemistry_Molecule._includes)
#include <chemistry/molecule/molecule.h>
// DO-NOT-DELETE splicer.end(MPQC.Chemistry_Molecule._includes)

namespace MPQC { 

  /**
   * Symbol "MPQC.Chemistry_Molecule" (version 0.2)
   */
  class Chemistry_Molecule_impl
  // DO-NOT-DELETE splicer.begin(MPQC.Chemistry_Molecule._inherits)
  // Put additional inheritance here...
  // DO-NOT-DELETE splicer.end(MPQC.Chemistry_Molecule._inherits)
  {

  private:
    // Pointer back to IOR.
    // Use this to dispatch back through IOR vtable.
    Chemistry_Molecule self;

    // DO-NOT-DELETE splicer.begin(MPQC.Chemistry_Molecule._implementation)
      double net_charge;
      sc::Ref< sc::Molecule> mol;
    // DO-NOT-DELETE splicer.end(MPQC.Chemistry_Molecule._implementation)

  private:
    // private default constructor (required)
    Chemistry_Molecule_impl() {} 

  public:
    // sidl constructor (required)
    // Note: alternate Skel constructor doesn't call addref()
    // (fixes bug #275)
    Chemistry_Molecule_impl( struct MPQC_Chemistry_Molecule__object * s ) : 
      self(s,true) { _ctor(); }

    // user defined construction
    void _ctor();

    // virtual destructor (required)
    virtual ~Chemistry_Molecule_impl() { _dtor(); }

    // user defined destruction
    void _dtor();

  public:

    /**
     * user defined non-static method.
     */
    void
    initialize_pointer (
      /*in*/ void* ptr
    )
    throw () 
    ;


    /**
     * Obtain Services handle, through which the 
     * component communicates with the framework. 
     * This is the one method that every CCA Component
     * must implement. The component will be called
     * with a nil/null Services pointer when it is
     * to shut itself down.
     */
    void
    setServices (
      /*in*/ ::gov::cca::Services services
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    void
    initialize (
      /*in*/ int32_t natom
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    ::Physics::Units
    get_units() throw () 
    ;
    /**
     * user defined non-static method.
     */
    int64_t
    get_n_atom() throw () 
    ;
    /**
     * user defined non-static method.
     */
    int64_t
    get_atomic_number (
      /*in*/ int64_t atomnum
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    void
    set_atomic_number (
      /*in*/ int64_t atomnum,
      /*in*/ int64_t atomic_number
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    double
    get_net_charge() throw () 
    ;
    /**
     * user defined non-static method.
     */
    void
    set_net_charge (
      /*in*/ double charge
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    double
    get_cart_coor (
      /*in*/ int64_t atomnum,
      /*in*/ int32_t xyz
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    void
    set_cart_coor (
      /*in*/ int64_t atomnum,
      /*in*/ int32_t xyz,
      /*in*/ double val
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    ::std::string
    get_atomic_label (
      /*in*/ int64_t atomnum
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    void
    set_atomic_label (
      /*in*/ int64_t atomnum,
      /*in*/ const ::std::string& label
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    ::Physics::PointGroup
    get_symmetry() throw () 
    ;
    /**
     * user defined non-static method.
     */
    ::sidl::array<double>
    get_coor() throw () 
    ;
    /**
     * user defined non-static method.
     */
    void
    set_coor (
      /*in*/ ::sidl::array<double> x
    )
    throw () 
    ;

  };  // end class Chemistry_Molecule_impl

} // end namespace MPQC

// DO-NOT-DELETE splicer.begin(MPQC.Chemistry_Molecule._misc)
// Put miscellaneous things here...
// DO-NOT-DELETE splicer.end(MPQC.Chemistry_Molecule._misc)

#endif
