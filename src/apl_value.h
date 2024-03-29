#pragma once

namespace APL
{

    enum ValueType {
        VALUE_TYPE_NONE,
        VALUE_TYPE_CONST,
        VALUE_TYPE_PLAYER_MANA,
        VALUE_TYPE_PLAYER_MANA_PERCENT,
        VALUE_TYPE_PLAYER_MANA_DEFICIT,
        VALUE_TYPE_PLAYER_MANA_DEFICIT_PERCENT,
        VALUE_TYPE_TALENT_COUNT,
        VALUE_TYPE_RUNE_EXISTS,
        VALUE_TYPE_COOLDOWN_EXISTS,
        VALUE_TYPE_COOLDOWN_REACT,
        VALUE_TYPE_COOLDOWN_DURATION,
        VALUE_TYPE_BUFF_EXISTS,
        VALUE_TYPE_BUFF_REACT,
        VALUE_TYPE_BUFF_STACKS,
        VALUE_TYPE_BUFF_DURATION,
        VALUE_TYPE_DEBUFF_EXISTS,
        VALUE_TYPE_DEBUFF_STACKS,
        VALUE_TYPE_DEBUFF_DURATION,
        VALUE_TYPE_STATE_TIME,
        VALUE_TYPE_STATE_TIME_PERCENT,
        VALUE_TYPE_STATE_DURATION,
        VALUE_TYPE_STATE_DURATION_PERCENT,
        VALUE_TYPE_STATE_IS_MOVING,
        VALUE_TYPE_STATE_IS_SILENCED,
        VALUE_TYPE_STATE_IS_INTERRUPTED,
        VALUE_TYPE_SPELL_TRAVEL_TIME,
        VALUE_TYPE_SPELL_CAST_TIME,
        VALUE_TYPE_SPELL_TRAVEL_CAST_TIME,
        VALUE_TYPE_SPELL_MANA_COST,
        VALUE_TYPE_SPELL_CAN_CAST,
        VALUE_TYPE_CONFIG_DISTANCE,
        VALUE_TYPE_CONFIG_REACTION_TIME,
        VALUE_TYPE_CONFIG_TARGET_LEVEL,
        VALUE_TYPE_CONFIG_PLAYER_LEVEL,
        VALUE_TYPE_CONFIG_DIFF_LEVEL,
    };

    struct Value {
        ValueType type = VALUE_TYPE_NONE;
        std::string str = "";
        double value = 0;
        int id = 0;
    };

}