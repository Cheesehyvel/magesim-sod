#pragma once

#include "apl_action.h"
#include "apl_condition.h"
#include "apl_item.h"
#include "apl_value.h"

#include <vector>

namespace APL
{

    struct APL {
        std::vector<Item> precombat;
        std::vector<Item> combat;
    };

}