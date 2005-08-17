// 
// File:          MPQC_Chemistry_MoleculeViewer_Impl.hh
// Symbol:        MPQC.Chemistry_MoleculeViewer-v0.2
// Symbol Type:   class
// Babel Version: 0.10.2
// Description:   Server-side implementation for MPQC.Chemistry_MoleculeViewer
// 
// WARNING: Automatically generated; only changes within splicers preserved
// 
// babel-version = 0.10.2
// 

#ifndef included_MPQC_Chemistry_MoleculeViewer_Impl_hh
#define included_MPQC_Chemistry_MoleculeViewer_Impl_hh

#ifndef included_sidl_cxx_hh
#include "sidl_cxx.hh"
#endif
#ifndef included_MPQC_Chemistry_MoleculeViewer_IOR_h
#include "MPQC_Chemistry_MoleculeViewer_IOR.h"
#endif
// 
// Includes for all method dependencies.
// 
#ifndef included_Chemistry_Molecule_hh
#include "Chemistry_Molecule.hh"
#endif
#ifndef included_MPQC_Chemistry_MoleculeViewer_hh
#include "MPQC_Chemistry_MoleculeViewer.hh"
#endif
#ifndef included_gov_cca_CCAException_hh
#include "gov_cca_CCAException.hh"
#endif
#ifndef included_gov_cca_Services_hh
#include "gov_cca_Services.hh"
#endif
#ifndef included_sidl_BaseInterface_hh
#include "sidl_BaseInterface.hh"
#endif
#ifndef included_sidl_ClassInfo_hh
#include "sidl_ClassInfo.hh"
#endif


// DO-NOT-DELETE splicer.begin(MPQC.Chemistry_MoleculeViewer._includes)

#define USE_SOCKET 1
#if USE_SOCKET
#include "socket.h"
#endif // USE_SOCKET

// DO-NOT-DELETE splicer.end(MPQC.Chemistry_MoleculeViewer._includes)

namespace MPQC { 

  /**
   * Symbol "MPQC.Chemistry_MoleculeViewer" (version 0.2)
   */
  class Chemistry_MoleculeViewer_impl
  // DO-NOT-DELETE splicer.begin(MPQC.Chemistry_MoleculeViewer._inherits)

  /** Chemistry_MoleculeViewer_impl implements a component interface
      for molecular viewers.

      This is an implementation of a SIDL interface.
      The stub code is generated by the Babel tool.  Do not make
      modifications outside of splicer blocks, as these will be lost.
      This is a server implementation for a Babel class, the Babel
      client code is provided by the cca-chem-generic package.
   */

  // Put additional inheritance here...
  // DO-NOT-DELETE splicer.end(MPQC.Chemistry_MoleculeViewer._inherits)
  {

  private:
    // Pointer back to IOR.
    // Use this to dispatch back through IOR vtable.
    Chemistry_MoleculeViewer self;

    // DO-NOT-DELETE splicer.begin(MPQC.Chemistry_MoleculeViewer._implementation)

      gov::cca::Services services_;
      Chemistry::Molecule molecule_;
      int is_updated;
#if USE_SOCKET
      TCPClientConnection socket_;
#endif // USE_SOCKET

    // DO-NOT-DELETE splicer.end(MPQC.Chemistry_MoleculeViewer._implementation)

  private:
    // private default constructor (required)
    Chemistry_MoleculeViewer_impl() 
    {} 

  public:
    // sidl constructor (required)
    // Note: alternate Skel constructor doesn't call addref()
    // (fixes bug #275)
    Chemistry_MoleculeViewer_impl( struct MPQC_Chemistry_MoleculeViewer__object 
      * s ) : self(s,true) { _ctor(); }

    // user defined construction
    void _ctor();

    // virtual destructor (required)
    virtual ~Chemistry_MoleculeViewer_impl() { _dtor(); }

    // user defined destruction
    void _dtor();

    // static class initializer
    static void _load();

  public:

    /**
     * user defined non-static method.
     */
    void
    set_molecule (
      /* in */ ::Chemistry::Molecule molecule
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    void
    set_coor (
      /* in */ const ::std::string& coords
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    void
    run_gui() throw () 
    ;
    /**
     * user defined non-static method.
     */
    void
    draw() throw () 
    ;

    /**
     * Starts up a component presence in the calling framework.
     * @param Svc the component instance's handle on the framework world.
     * Contracts concerning Svc and setServices:
     * 
     * The component interaction with the CCA framework
     * and Ports begins on the call to setServices by the framework.
     * 
     * This function is called exactly once for each instance created
     * by the framework.
     * 
     * The argument Svc will never be nil/null.
     * 
     * Those uses ports which are automatically connected by the framework
     * (so-called service-ports) may be obtained via getPort during
     * setServices.
     */
    void
    setServices (
      /* in */ ::gov::cca::Services services
    )
    throw ( 
      ::gov::cca::CCAException
    );

  };  // end class Chemistry_MoleculeViewer_impl

} // end namespace MPQC

// DO-NOT-DELETE splicer.begin(MPQC.Chemistry_MoleculeViewer._misc)
// Put miscellaneous things here...
// DO-NOT-DELETE splicer.end(MPQC.Chemistry_MoleculeViewer._misc)

#endif
