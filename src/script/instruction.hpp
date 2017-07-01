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
/////////////////////////////////////////////////////////////////////

#ifndef LITESCRIPT_LITESCRIPT_HPP

#include "../litescript.hpp"

#else

#ifndef LITESCRIPT_INSTRUCTION_HPP
#define LITESCRIPT_INSTRUCTION_HPP

namespace LiteScript {

    // Reduced instruction
    struct Instruction {

        // The instruction code
        unsigned short code;

    };

}

#endif //LITESCRIPT_INSTRUCTION_HPP

#endif
