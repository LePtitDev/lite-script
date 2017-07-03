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

#ifndef LITESCRIPT_LITESCRIPT_HPP
#define LITESCRIPT_LITESCRIPT_HPP

#include <cmath>
#include <cstring>
#include <cstdlib>

#include <iostream>
#include <bitset>
#include <array>
#include <vector>
#include <string>
#include <memory>
#include <sstream>

namespace LiteScript {

    class Type;
    class Object;
    class Variable;
    class Memory;
    struct Instruction;
    class State;

}

#include "nullable.hpp"

#include "memory/type.hpp"
#include "memory/object.hpp"
#include "memory/variable.hpp"
#include "memory/memory.hpp"

#include "script/instruction.hpp"
#include "script/state.hpp"

#include "types/internal.hpp"

#endif //LITESCRIPT_LITESCRIPT_HPP
