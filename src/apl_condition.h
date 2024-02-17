#pragma once

#include "apl_value.h"

#include <vector>

namespace APL
{

    enum ConditionType {
        CONDITION_TYPE_NONE,
        CONDITION_TYPE_AND,
        CONDITION_TYPE_OR,
        CONDITION_TYPE_CMP,
        CONDITION_TYPE_NOT,
        CONDITION_TYPE_FALSE,
        CONDITION_TYPE_TRUE,
    };

    enum ConditionOp {
        CONDITION_OP_NONE,
        CONDITION_OP_EQ,
        CONDITION_OP_NEQ,
        CONDITION_OP_GT,
        CONDITION_OP_GTE,
        CONDITION_OP_LT,
        CONDITION_OP_LTE,
    };

    struct Condition {
        ConditionType type = CONDITION_TYPE_NONE;
        ConditionOp op = CONDITION_OP_NONE;
        std::vector<Condition> conditions;
        std::vector<Value> values;
    };

}