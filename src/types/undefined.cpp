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

#include "undefined.hpp"

LiteScript::_Type_UNDEFINED LiteScript::_type_undefined;

LiteScript::_Type_UNDEFINED::_Type_UNDEFINED() : Type("UNDEFINED") {}

LiteScript::Object LiteScript::_Type_UNDEFINED::CreateObject() {
    return Object(_type_undefined, 0);
}

std::string LiteScript::_Type_UNDEFINED::ToString(const LiteScript::Object& obj) const {
    return "undefined";
}
