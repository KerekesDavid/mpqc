// 
// File:          MPQC_GaussianBasis_Molecular_Impl.hh
// Symbol:        MPQC.GaussianBasis_Molecular-v0.2
// Symbol Type:   class
// Babel Version: 0.9.8
// Description:   Server-side implementation for MPQC.GaussianBasis_Molecular
// 
// WARNING: Automatically generated; only changes within splicers preserved
// 
// babel-version = 0.9.8
// 

#ifndef included_MPQC_GaussianBasis_Molecular_Impl_hh
#define included_MPQC_GaussianBasis_Molecular_Impl_hh

#ifndef included_sidl_cxx_hh
#include "sidl_cxx.hh"
#endif
#ifndef included_MPQC_GaussianBasis_Molecular_IOR_h
#include "MPQC_GaussianBasis_Molecular_IOR.h"
#endif
// 
// Includes for all method dependencies.
// 
#ifndef included_Chemistry_Molecule_hh
#include "Chemistry_Molecule.hh"
#endif
#ifndef included_Chemistry_QC_GaussianBasis_AngularType_hh
#include "Chemistry_QC_GaussianBasis_AngularType.hh"
#endif
#ifndef included_Chemistry_QC_GaussianBasis_Atomic_hh
#include "Chemistry_QC_GaussianBasis_Atomic.hh"
#endif
#ifndef included_MPQC_GaussianBasis_Molecular_hh
#include "MPQC_GaussianBasis_Molecular.hh"
#endif
#ifndef included_sidl_BaseInterface_hh
#include "sidl_BaseInterface.hh"
#endif
#ifndef included_sidl_ClassInfo_hh
#include "sidl_ClassInfo.hh"
#endif


// DO-NOT-DELETE splicer.begin(MPQC.GaussianBasis_Molecular._includes)
#include <chemistry/qc/basis/basis.h>
#include <Chemistry_Chemistry_Molecule.hh>
#include <MPQC_GaussianBasis_Atomic_Impl.hh>
using namespace std;
using namespace Chemistry::QC::GaussianBasis;
using namespace Chemistry;
using namespace sc;
// DO-NOT-DELETE splicer.end(MPQC.GaussianBasis_Molecular._includes)

namespace MPQC { 

  /**
   * Symbol "MPQC.GaussianBasis_Molecular" (version 0.2)
   */
  class GaussianBasis_Molecular_impl
  // DO-NOT-DELETE splicer.begin(MPQC.GaussianBasis_Molecular._inherits)
  // Put additional inheritance here...
  // DO-NOT-DELETE splicer.end(MPQC.GaussianBasis_Molecular._inherits)
  {

  private:
    // Pointer back to IOR.
    // Use this to dispatch back through IOR vtable.
    GaussianBasis_Molecular self;

    // DO-NOT-DELETE splicer.begin(MPQC.GaussianBasis_Molecular._implementation)
    GaussianBasisSet *gbs_ptr_;
    Ref<GaussianBasisSet> sc_gbs_;
    string label_;
    AngularType angular_type_;
    Chemistry_Molecule molecule_;
    MPQC::GaussianBasis_Atomic *atomic_array_;
    int natom_;
    // DO-NOT-DELETE splicer.end(MPQC.GaussianBasis_Molecular._implementation)

  private:
    // private default constructor (required)
    GaussianBasis_Molecular_impl() {} 

  public:
    // sidl constructor (required)
    // Note: alternate Skel constructor doesn't call addref()
    // (fixes bug #275)
    GaussianBasis_Molecular_impl( struct MPQC_GaussianBasis_Molecular__object * 
      s ) : self(s,true) { _ctor(); }

    // user defined construction
    void _ctor();

    // virtual destructor (required)
    virtual ~GaussianBasis_Molecular_impl() { _dtor(); }

    // user defined destruction
    void _dtor();

  public:

    /**
     * user defined non-static method.
     */
    void
    initialize (
      /*in*/ void* scbasis,
      /*in*/ const ::std::string& label
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    void*
    sc_gbs_pointer() throw () 
    ;
    /**
     * user defined non-static method.
     */
    ::std::string
    get_label() throw () 
    ;
    /**
     * user defined non-static method.
     */
    int64_t
    get_n_basis() throw () 
    ;
    /**
     * user defined non-static method.
     */
    int64_t
    get_n_shell() throw () 
    ;
    /**
     * user defined non-static method.
     */
    int64_t
    get_max_angular_momentum() throw () 
    ;
    /**
     * user defined non-static method.
     */
    ::Chemistry::QC::GaussianBasis::AngularType
    get_angular_type() throw () 
    ;
    /**
     * user defined non-static method.
     */
    ::Chemistry::QC::GaussianBasis::Atomic
    get_atomic (
      /*in*/ int64_t atomnum
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    ::Chemistry::Molecule
    get_molecule() throw () 
    ;
    /**
     * user defined non-static method.
     */
    void
    print_molecular() throw () 
    ;
  };  // end class GaussianBasis_Molecular_impl

} // end namespace MPQC

// DO-NOT-DELETE splicer.begin(MPQC.GaussianBasis_Molecular._misc)
// Put miscellaneous things here...
// DO-NOT-DELETE splicer.end(MPQC.GaussianBasis_Molecular._misc)

#endif
