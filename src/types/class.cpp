/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/*
    Copyright (C) 2017 LePtitDev
    All rights reserved.

    This software may be modified and distributed under the terms
    of the BSD license.  See the LICENSE file for details.

    Author: Arthur Ferré <leptitdev.com>
*/
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#include "class.hpp"

LiteScript::_Type_CLASS LiteScript::_type_class;

LiteScript::_Type_CLASS::_Type_CLASS() : Type("CLASS") {}

LiteScript::Object & LiteScript::_Type_CLASS::AssignObject(Object &obj) {
    obj.Reassign(*this, sizeof(Class));
    std::allocator<Class> allocator;
    allocator.construct(&obj.GetData<Class>());
    return obj;
}