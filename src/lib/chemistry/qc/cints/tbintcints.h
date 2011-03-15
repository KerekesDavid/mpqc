//
// tbint.h
//
// Copyright (C) 2001 Edward Valeev
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

#ifndef _chemistry_qc_cints_tbint_h
#define _chemistry_qc_cints_tbint_h

#include <chemistry/qc/basis/tbint.h>
#include <chemistry/qc/cints/int2e.h>

namespace sc {

/** This implements electron repulsion integrals in the IntCints library. */
class TwoBodyIntCints : public TwoBodyInt {

    TwoBodyOperSet::type int2etype_;
    Ref<TwoBodyOperSetDescr> descr_;

  protected:
    Ref<Int2eCints> int2ecints_;

  public:
    TwoBodyIntCints(Integral*integral,
                 const Ref<GaussianBasisSet>&b1,
                 const Ref<GaussianBasisSet>&b2,
                 const Ref<GaussianBasisSet>&b3,
                 const Ref<GaussianBasisSet>&b4,
                 size_t storage, TwoBodyOperSet::type int2etype);
    ~TwoBodyIntCints();

    TwoBodyOperSet::type type() const { return int2etype_; }
    const Ref<TwoBodyOperSetDescr>& descr() const { return descr_; }

    int log2_shell_bound(int,int,int,int);
    void compute_shell(int,int,int,int);

    size_t used_storage() const { return int2ecints_->storage_used(); }
    void set_integral_storage(size_t storage);

    const double *buffer(TwoBodyOper::type te_type) const {
      return int2ecints_->buffer( descr_->opertype(te_type) );
    }
};

/** This implements electron repulsion derivative integrals in the IntV3
    library. */
class TwoBodyDerivIntCints : public TwoBodyDerivInt {
  protected:
    Ref<Int2eCints> int2ecints_;

  public:
    TwoBodyDerivIntCints(Integral*integral,
                      const Ref<GaussianBasisSet>&b1,
                      const Ref<GaussianBasisSet>&b2,
                      const Ref<GaussianBasisSet>&b3,
                      const Ref<GaussianBasisSet>&b4,
                      size_t storage, TwoBodyOperSet::type int2etype);
    ~TwoBodyDerivIntCints();

    int log2_shell_bound(int,int,int,int);
    void compute_shell(int,int,int,int,DerivCenters&);

    size_t used_storage() const { return int2ecints_->storage_used(); }
};

}

#endif

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End: