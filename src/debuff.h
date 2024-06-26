#pragma once

#include <string>

namespace debuff
{
    enum ID : int
    {
        IMPROVED_SCORCH = 22959,
        WINTERS_CHILL = 12579,
        PYROBLAST = 11366,
        LIVING_BOMB = 400613,
        MALEVOLENCE = 449920,
    };

    // Prolonged ownership by Event, State and Unit. TODO: Do Event and Unit really need the same copy (i.e. pointer)?
    struct Debuff
    {
        ID id;
        std::string name = "";
        double duration = 0;
        double t_expires = 0;
        int stacks = 1;
        int max_stacks = 1;
        bool hidden = false;

        int addStack()
        {
            if (stacks < max_stacks)
                ++stacks;
            else
                return 0;

            return stacks;
        }

    };

    class ImprovedScorch : public Debuff
    {
    public:
        ImprovedScorch()
        {
            id = IMPROVED_SCORCH;
            name = "Improved Scorch";
            duration = 30;
            max_stacks = 5;
        }
    };

    class WintersChill : public Debuff
    {
    public:
        WintersChill()
        {
            id = WINTERS_CHILL;
            name = "Winter's Chill";
            duration = 15;
            max_stacks = 5;
        }
    };

    class Pyroblast : public Debuff
    {
    public:
        Pyroblast()
        {
            id = PYROBLAST;
            name = "Pyroblast";
            duration = 12;
        }
    };

    class LivingBomb : public Debuff
    {
    public:
        LivingBomb()
        {
            id = LIVING_BOMB;
            name = "Living Bomb";
            duration = 12;
        }
    };

    class Malevolence : public Debuff
    {
    public:
        Malevolence()
        {
            id = MALEVOLENCE;
            name = "Malevolence";
            duration = 30;
        }
    };

}