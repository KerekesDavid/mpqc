//
// transform.cc
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

#include <stdlib.h>
#include <util/render/transform.h>

DescribedClass_REF_def(Transform);
#define CLASSNAME Transform
#define HAVE_KEYVAL_CTOR
#define PARENTS public DescribedClass
#include <util/class/classi.h>
void *
Transform::_castdown(const ClassDesc*cd)
{
  void* casts[1];
  casts[0] = DescribedClass::_castdown(cd);
  return do_castdowns(casts,cd);
}

Transform::Transform(const RefKeyVal& keyval)
{
  transform_ = identity3D();
  if (keyval->exists("translate")) {
      if (keyval->count("translate") != 3) {
          ExEnv::err() << "Transform: error in translation" << endl;
          abort();
        }
      translate(keyval->doublevalue("translate",0),
                keyval->doublevalue("translate",1),
                keyval->doublevalue("translate",2));
    }
  if (keyval->exists("rotate")) {
      if (keyval->count("rotate:axis") != 3
          || !keyval->exists("rotate:angle")) {
          ExEnv::err() << "Transform: error in rotation" << endl;
          abort();
        }
      vec3 axis(keyval->doublevalue("rotate:axis",0),
                keyval->doublevalue("rotate:axis",1),
                keyval->doublevalue("rotate:axis",2));
      rotate(axis, keyval->doublevalue("rotate:angle"));
    }
  if (keyval->exists("scale")) {
      double scalefactor = keyval->doublevalue("scale");
      scale(scalefactor);
    }
}

Transform::~Transform()
{
}

void
Transform::translate(double x, double y, double z)
{
  vec3 r(x,y,z);
  translate(r);
}

void
Transform::translate(const vec3& r)
{
  transform_ = translation3D(r) * transform_;
}

void
Transform::rotate(const vec3& axis, double angle)
{
  transform_ = rotation3D(axis, angle) * transform_;
}

void
Transform::scale(double scalefactor)
{
  transform_ = scaling3D(vec3(scalefactor,scalefactor,scalefactor))
             * transform_;
    }

void
Transform::print(ostream& os) const
{
  os << "Transform" << endl;
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
