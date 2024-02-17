#pragma once

#include "apl_action.h"
#include "apl_condition.h"

namespace APL
{

    struct Item {
        Condition condition;
        Action action;
    };

}