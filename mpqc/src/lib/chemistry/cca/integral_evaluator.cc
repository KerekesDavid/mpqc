#include "integral_evaluator.h"
using namespace MpqcCca;

template< typename eval_type, typename computer_type >
void
IntegralEvaluator< eval_type, computer_type >::
IntegralEvaluator::add_evaluator ( void* eval, IntegralDescr desc ) 
{
  eval_type* eval_ptr;   
  eval_ptr = dynamic_cast<eval_type*>(eval);
  pair<eval_type*,IntegralDescr> p(eval_ptr,desc);
  evals_.pushback( p );
}

template< typename eval_type, typename computer_type >
void
IntegralEvaluator< eval_type, computer_type >::
IntegralEvaluator::set_basis ( vector<Molecular> ) 
{
  /*
  cca_bs1_ = bs1;
  cca_bs2_ = bs2;

  bs1_ = basis_cca_to_sc( cca_bs1_ );
  if( bs1.isSame(bs2) ) 
    bs2_.assign_pointer( bs1_.pointer() );
  else 
    bs2_ = basis_cca_to_sc( cca_bs2_ );
  max_nshell2_ = bs1_->max_ncartesian_in_shell() *
    bs2_->max_ncartesian_in_shell();
  maxam_ = max( bs1_->max_angular_momentum(), bs2_->max_angular_momentum() ); 
  */
}


template< typename eval_type, typename computer_type >
void*
IntegralEvaluator< eval_type, computer_type >::
IntegralEvaluator::get_buffer ( IntegralDescr desc ) 
{
  for( int i=0; i<evals_.size(); ++i)
    if( desc.get_type() == evals_[i].second->get_type() &&
	desc.get_deriv_lvl() == evals_[i].second->get_deriv_lvl() )
      return (void*) evals_[i].first->get_buffer();
  return NULL;
}


template< typename eval_type, typename computer_type >
Chemistry::QC::GaussianBasis::DerivCenters
IntegralEvaluator< eval_type, computer_type >::
IntegralEvaluator::get_deriv_centers ()
{
  // later
}


template< typename eval_type, typename computer_type >
Chemistry::QC::GaussianBasis::CompositeIntegralDescr
IntegralEvaluator< eval_type, computer_type >::
IntegralEvaluator::get_descriptor ()
{
  CompositeIntegralDescr cdesc;
  for( int i=0; i<evals_.size(); ++i)
    cdesc.add_descr( evals_[i].second );
  return cdesc;
}


template< typename eval_type, typename computer_type >
void
IntegralEvaluator< eval_type, computer_type >::
IntegralEvaluator::compute ( computer_type* computer ) 
{
  /*
  if( int_type_ == one_body )
    eval_->compute_shell( shellnum1, shellnum2 );
  else if( int_type_ == one_body_deriv ) {
  
    sc_deriv_centers_.clear();
    deriv_centers_.clear();

    if( deriv_atom_ == -1) {
      deriv_eval_->compute_shell( shellnum1, shellnum2, sc_deriv_centers_ );

      if(sc_deriv_centers_.has_omitted_center())
        deriv_centers_.add_omitted( sc_deriv_centers_.omitted_center(),
                                    sc_deriv_centers_.omitted_atom() );
      for( int i=0; i<sc_deriv_centers_.n() ; ++i)
        deriv_centers_.add_center( sc_deriv_centers_.center(i),
                                   sc_deriv_centers_.atom(i) );
    }
    else
      deriv_eval_->compute_shell( shellnum1, shellnum2, deriv_atom_ );

  }
  else 
    throw ProgrammingError("bad evaluator type",
                           __FILE__,__LINE__);
  */

  for( int i=0; i<evals_.size(); ++i) 
    computer->compute( evals_[i].first );
  
  /*
  sc::GaussianShell* s1 = &( bs1_->shell(shellnum1) );
  sc::GaussianShell* s2 = &( bs2_->shell(shellnum2) );
  int nfunc = s1->nfunction() * s2->nfunction();

  if( reorder_ ) reorder( shellnum1, shellnum2 );
  */
}


template< typename eval_type, typename computer_type >
sidl::array<double>
IntegralEvaluator< eval_type, computer_type >::
IntegralEvaluator::compute_array ( computer_type* computer ) 
{
  /*
  compute( shellnum1, shellnum2, deriv_level, deriv_atom );

  int lower[1] = {0};
  int upper[1]; upper[0] = max_nshell2_-1;
  int stride[1] = {1};
  sidl_buffer_.borrow( const_cast<double*>(sc_buffer_), 1, 
                       lower, upper, stride);
  return sidl_buffer_;
  */
}


template< typename eval_type, typename computer_type >
void
IntegralEvaluator< eval_type, computer_type >::
IntegralEvaluator::initialize_reorder() 
{
  /*
  if( int_type_ == one_body )
    temp_buffer_ = new double[max_nshell2_];
  else if( int_type_ == one_body_deriv )
    temp_buffer_ = new double[max_nshell2_*3];

  reorder_ = new int*[maxam_+1];
  reorder_[0] = new int[1];
  reorder_[0][0] = 0;

  for( int i=1; i<=maxam_; ++i) {

    sc::CartesianIter *v3iter = integral_->new_cartesian_iter(i);
    MPQC::CartesianIterCCA iter(i);
    MPQC::CartesianIterCCA *ccaiter = &iter;
    ccaiter->start();
    int ncf = ccaiter->n();
    
    reorder_[i] = new int[ncf];
    v3iter->start();
    for( int j=0; j<ncf; ++j) {
      ccaiter->start();
      for( int k=0; k<ncf; ++k) {
        if( v3iter->a() == ccaiter->a() &&
            v3iter->b() == ccaiter->b() &&
            v3iter->c() == ccaiter->c() ) {
          reorder_[i][j] = k;
          k=ncf; //break k loop
        }
        else ccaiter->next();
      }
      v3iter->next();
    }
  }
  */
}


template< typename eval_type, typename computer_type >
void
IntegralEvaluator< eval_type, computer_type >::
IntegralEvaluator::reorder( int shellnum1, int shellnum2)
{
  /*
  sc::GaussianShell* s1 = &( bs1_->shell(shellnum1) );
  sc::GaussianShell* s2 = &( bs2_->shell(shellnum2) );
  int nc1 = s1->ncontraction();
  int nc2 = s2->ncontraction();

  int reorder_needed=0;
  for (int i=0; i<nc1; ++i) {
    if( s1->am(i) == 1) reorder_needed=1;
    else if( s1->am(i) > 1 && s1->is_cartesian(i) ) reorder_needed=1;
  }
  if (!reorder_needed)
    for (int i=0; i<nc2; ++i) {
      if( s2->am(i) == 1) reorder_needed=1;
      else if( s2->am(i) > 1 && s2->is_cartesian(i) ) reorder_needed=1;
    }
  if( !reorder_needed && int_type_ == one_body ) return;

  int nfunc = s1->nfunction() * s2->nfunction();
  if( int_type_ == one_body_deriv )
    for( int i=0; i<nfunc*3; ++i)
      temp_buffer_[i] = sc_buffer_[i];
  else
    for( int i=0; i<nfunc; ++i)
      temp_buffer_[i] = sc_buffer_[i];

  // a derivative buffer is composed of nfunc triplets (dx,dy,dz)
  // which must be repacked into 3 (dx,dy,dz) shell doublets of nfunc entries
  int deriv_offset;
  if( int_type_ == one_body  )
    reorder_doublet( s1, s2, nc1, nc2, 0 );
  else if( int_type_ == one_body_deriv )
    reorder_doublet( s1, s2, nc1, nc2 );
  */
}


template< typename eval_type, typename computer_type >
void
IntegralEvaluator< eval_type, computer_type >::
IntegralEvaluator::reorder_doublet( sc::GaussianShell* s1, 
				    sc::GaussianShell* s2,
				    int nc1, int nc2, int is_deriv )
{
  /*
  int index=0, con2_offset=0, local2_offset, 
      c1_base, c2_base;

  int temp;
  con2_offset = s2->nfunction();

  int s1_is_cart, s2_is_cart, nfunc, s1_nfunc, s2_nfunc; 
  nfunc = s1->nfunction() * s2->nfunction();

  c1_base = 0;
  for( int c1=0; c1<nc1; ++c1 ) {

    //c1_base = index;
    if(c1>0) c1_base += s1->nfunction(c1-1) * con2_offset;

    s1_is_cart = s1->is_cartesian(c1);
    s1_nfunc = s1->nfunction(c1);

    for( int fc1=0; fc1<s1_nfunc; ++fc1 ) {

      if( s1_is_cart )
        c2_base = c1_base + reorder_[s1->am(c1)][fc1] * con2_offset;
      else
        c2_base = c1_base + fc1 * con2_offset;

      local2_offset = 0;
      for( int c2=0; c2<nc2; ++c2 ) {
        if( c2>0 ) local2_offset += s2->nfunction(c2-1);
        s2_is_cart = s2->is_cartesian(c2);
        s2_nfunc = s2->nfunction(c2);

        if(!is_deriv) {
          if( s2_is_cart )
            for( int fc2=0; fc2<s2_nfunc; ++fc2 ) {
              buf_[ c2_base + local2_offset + reorder_[s2->am(c2)][fc2] ]
                = temp_buffer_[index];
              ++index;
            }
          else
            for( int fc2=0; fc2<s2_nfunc; ++fc2 ) {
              buf_[ c2_base + local2_offset + fc2 ] = temp_buffer_[index];
              ++index;
            }
        }
        else {
          if( s2_is_cart )
            for( int fc2=0; fc2<s2_nfunc; ++fc2 ) {
              for(int di=0; di<3; ++di) {
                buf_[ c2_base + local2_offset + reorder_[s2->am(c2)][fc2] + 
		      di*nfunc ]
                  = temp_buffer_[index];
                ++index;
              }
            }
          else
            for( int fc2=0; fc2<s2_nfunc; ++fc2 ) {
              for(int di=0; di<3; ++di) {
                buf_[ c2_base + local2_offset + fc2 + di*nfunc ] = 
		  temp_buffer_[index];
                ++index;
              }
            }
        }

      }
    }
  }
  */

}