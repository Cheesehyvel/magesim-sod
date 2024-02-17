#pragma once

namespace APL
{

    enum ActionType {
        ACTION_TYPE_NONE,
        ACTION_TYPE_SPELL,
        ACTION_TYPE_BUFF,
        ACTION_TYPE_CUSTOM,
        ACTION_TYPE_WAIT,
        ACTION_TYPE_SEQUENCE,
    };

    struct Action {
        ActionType type = ACTION_TYPE_NONE;
        std::string str = "";
        double value = 0;
        int id = 0;
        std::vector<Action> sequence;
    };

}