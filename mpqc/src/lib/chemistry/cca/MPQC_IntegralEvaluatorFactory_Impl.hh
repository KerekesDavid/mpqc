// 
// File:          MPQC_IntegralEvaluatorFactory_Impl.hh
// Symbol:        MPQC.IntegralEvaluatorFactory-v0.2
// Symbol Type:   class
// Babel Version: 0.9.8
// Description:   Server-side implementation for MPQC.IntegralEvaluatorFactory
// 
// WARNING: Automatically generated; only changes within splicers preserved
// 
// babel-version = 0.9.8
// 

#ifndef included_MPQC_IntegralEvaluatorFactory_Impl_hh
#define included_MPQC_IntegralEvaluatorFactory_Impl_hh

#ifndef included_sidl_cxx_hh
#include "sidl_cxx.hh"
#endif
#ifndef included_MPQC_IntegralEvaluatorFactory_IOR_h
#include "MPQC_IntegralEvaluatorFactory_IOR.h"
#endif
// 
// Includes for all method dependencies.
// 
#ifndef included_Chemistry_Molecule_hh
#include "Chemistry_Molecule.hh"
#endif
#ifndef included_Chemistry_QC_GaussianBasis_ContractionTransform_hh
#include "Chemistry_QC_GaussianBasis_ContractionTransform.hh"
#endif
#ifndef included_Chemistry_QC_GaussianBasis_IntegralEvaluator2_hh
#include "Chemistry_QC_GaussianBasis_IntegralEvaluator2.hh"
#endif
#ifndef included_Chemistry_QC_GaussianBasis_IntegralEvaluator3_hh
#include "Chemistry_QC_GaussianBasis_IntegralEvaluator3.hh"
#endif
#ifndef included_Chemistry_QC_GaussianBasis_IntegralEvaluator4_hh
#include "Chemistry_QC_GaussianBasis_IntegralEvaluator4.hh"
#endif
#ifndef included_Chemistry_QC_GaussianBasis_Molecular_hh
#include "Chemistry_QC_GaussianBasis_Molecular.hh"
#endif
#ifndef included_MPQC_IntegralEvaluatorFactory_hh
#include "MPQC_IntegralEvaluatorFactory.hh"
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


// DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory._includes)
#include "cca.h"
#include "dc/babel/babel-cca/server/ccaffeine_TypeMap.hh"
#include "dc/babel/babel-cca/server/ccaffeine_ports_PortTranslator.hh"
#include "util/IO.h"
#include "jc++/jc++.h"
#include "jc++/util/jc++util.h"
#include "parameters/parametersStar.h"
#include "port/portInterfaces.h"
#include "port/supportInterfaces.h"
// DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory._includes)

namespace MPQC { 

  /**
   * Symbol "MPQC.IntegralEvaluatorFactory" (version 0.2)
   */
  class IntegralEvaluatorFactory_impl
  // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory._inherits)
  // Put additional inheritance here...
  // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory._inherits)
  {

  private:
    // Pointer back to IOR.
    // Use this to dispatch back through IOR vtable.
    IntegralEvaluatorFactory self;

    // DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory._implementation)
    gov::cca::Services services_;
    Chemistry::Molecule molecule_;
    std::string basis_name_;
    StringParameter *package_param_;
    ConfigurableParameterPort* setup_parameters(
      ConfigurableParameterFactory *cpf);
    // DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory._implementation)

  private:
    // private default constructor (required)
    IntegralEvaluatorFactory_impl() {} 

  public:
    // sidl constructor (required)
    // Note: alternate Skel constructor doesn't call addref()
    // (fixes bug #275)
    IntegralEvaluatorFactory_impl( struct MPQC_IntegralEvaluatorFactory__object 
      * s ) : self(s,true) { _ctor(); }

    // user defined construction
    void _ctor();

    // virtual destructor (required)
    virtual ~IntegralEvaluatorFactory_impl() { _dtor(); }

    // user defined destruction
    void _dtor();

  public:


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
      /*in*/ ::gov::cca::Services services
    )
    throw ( 
      ::gov::cca::CCAException
    );

    /**
     * user defined non-static method.
     */
    void
    set_basis_name (
      /*in*/ const ::std::string& basis_name
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    ::std::string
    get_basis_name() throw () 
    ;
    /**
     * user defined non-static method.
     */
    void
    set_molecular (
      /*in*/ ::Chemistry::QC::GaussianBasis::Molecular molbasis
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    ::Chemistry::QC::GaussianBasis::Molecular
    get_molecular (
      /*in*/ int64_t center
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    void
    set_molecule (
      /*in*/ ::Chemistry::Molecule mol
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
    ::Chemistry::QC::GaussianBasis::IntegralEvaluator2
    get_integral_evaluator2 (
      /*in*/ const ::std::string& label,
      /*in*/ int64_t max_deriv,
      /*in*/ ::Chemistry::QC::GaussianBasis::Molecular bs1,
      /*in*/ ::Chemistry::QC::GaussianBasis::Molecular bs2
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    ::Chemistry::QC::GaussianBasis::IntegralEvaluator3
    get_integral_evaluator3 (
      /*in*/ const ::std::string& label,
      /*in*/ int64_t max_deriv,
      /*in*/ ::Chemistry::QC::GaussianBasis::Molecular bs1,
      /*in*/ ::Chemistry::QC::GaussianBasis::Molecular bs2,
      /*in*/ ::Chemistry::QC::GaussianBasis::Molecular bs3
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    ::Chemistry::QC::GaussianBasis::IntegralEvaluator4
    get_integral_evaluator4 (
      /*in*/ const ::std::string& label,
      /*in*/ int64_t max_deriv,
      /*in*/ ::Chemistry::QC::GaussianBasis::Molecular bs1,
      /*in*/ ::Chemistry::QC::GaussianBasis::Molecular bs2,
      /*in*/ ::Chemistry::QC::GaussianBasis::Molecular bs3,
      /*in*/ ::Chemistry::QC::GaussianBasis::Molecular bs4
    )
    throw () 
    ;

    /**
     * user defined non-static method.
     */
    ::Chemistry::QC::GaussianBasis::ContractionTransform
    get_contraction_transform() throw () 
    ;
  };  // end class IntegralEvaluatorFactory_impl

} // end namespace MPQC

// DO-NOT-DELETE splicer.begin(MPQC.IntegralEvaluatorFactory._misc)
// Put miscellaneous things here...
// DO-NOT-DELETE splicer.end(MPQC.IntegralEvaluatorFactory._misc)

#endif
