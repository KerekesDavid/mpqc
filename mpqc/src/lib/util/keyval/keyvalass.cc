//
// keyvalass.cc
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

#include <util/keyval/keyval.h>
#include <util/keyval/keyvalImplMap.h>

AssignedKeyVal::AssignedKeyVal()
{
  _map = new MAPCTOR;
}

AssignedKeyVal::~AssignedKeyVal()
{
  delete _map;
}

int
AssignedKeyVal::key_exists(const char * key)
{
  KeyValKeyword k(key); 
  int result = _map->contains(k);
  if (!result) {
      seterror(UnknownKeyword);
    }
  else {
      seterror(OK);
    }
  return result;
}

RefKeyValValue
AssignedKeyVal::key_value(const char * key, const KeyValValue &def)
{
  KeyValKeyword k(key); 
  if (exists(key)) {
      seterror(OK);
      return _map->operator[](k);
    }
  else {
      seterror(UnknownKeyword);
      return 0;
    }
}

void
AssignedKeyVal::assign(const char*key,const RefKeyValValue& val)
{
  KeyValKeyword k(key);
  _map->operator[](k) = val;
}
void
AssignedKeyVal::assign(const char*key,double val)
{
  assign(key,new KeyValValuedouble(val));
}
void
AssignedKeyVal::assignboolean(const char*key,int val)
{
  assign(key,new KeyValValueboolean(val));
}
void
AssignedKeyVal::assign(const char*key,float val)
{
  assign(key,new KeyValValuefloat(val));
}

void
AssignedKeyVal::assign(const char*key,char val)
{
  assign(key,new KeyValValuechar(val));
}
void
AssignedKeyVal::assign(const char*key,int val)
{
  assign(key,new KeyValValueint(val));
}
void
AssignedKeyVal::assign(const char*key,const char* val)
{
  assign(key,new KeyValValuepchar(val));
}
void
AssignedKeyVal::assign(const char*key,const RefDescribedClass&val)
{
  assign(key,new KeyValValueRefDescribedClass(val));
}

void
AssignedKeyVal::clear()
{
  _map->clear();
}

/////////////////////////////////////////////////////////////////////////////

// Local Variables:
// mode: c++
// c-file-style: "CLJ"
// End:
