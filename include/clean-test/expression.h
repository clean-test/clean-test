// Copyright (c) m8mble 2020.
// SPDX-License-Identifier: BSL-1.0

#pragma once

#include "expression/Aborts.h"
#include "expression/Add.h"
#include "expression/Affirm.h"
#include "expression/And.h"
#include "expression/BitwiseAnd.h"
#include "expression/BitwiseNot.h"
#include "expression/BitwiseOr.h"
#include "expression/BitwiseXor.h"
#include "expression/Close.h"
#include "expression/Comma.h"
#include "expression/Distance.h"
#include "expression/Divide.h"
#include "expression/Equal.h"
#include "expression/Greater.h"
#include "expression/GreaterEqual.h"
#include "expression/Less.h"
#include "expression/LessEqual.h"
#include "expression/Lift.h"
#include "expression/Literals.h"
#include "expression/Modulo.h"
#include "expression/Multiply.h"
#include "expression/Negate.h"
#include "expression/Not.h"
#include "expression/NotEqual.h"
#include "expression/Or.h"
#include "expression/Star.h"
#include "expression/Subtract.h"
#include "expression/Throws.h"
#include "expression/Tolerance.h"

namespace clean_test {

#ifdef CLEANTEST_HAS_ABORT_SUPPORT
using expression::aborts;
using expression::debug_aborts;
#endif
using expression::distance;
using expression::is_close;
using expression::lift;
using expression::throws;
using expression::tolerance;

namespace literals {
using namespace expression::literals;
}

}
