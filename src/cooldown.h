#pragma once

#include <algorithm>

namespace cooldown
{

    enum ID : int
    {
        NONE = 0,
        POTION = 1,
        TRINKET1 = 3,
        TRINKET2 = 4,
        TRINKET_SHARED = 5,
        EVOCATION = 12051,
        COLD_SNAP = 11958,
        BERSERKING = 20554,
        ICY_VEINS = 12472,
        FIRE_BLAST = 42873,
        BLAST_WAVE = 42945,
        CONE_OF_COLD = 42931,
        COMBUSTION = 29977,
        ARCANE_SURGE = 425124,
        ARCANE_POWER = 12042,
        PRESENCE_OF_MIND = 12043,
        POWER_INFUSION = 10060,
        MANA_GEM = 42987
        // Trinkets
    };


    // Owned by Action and Unit.  Needs to be a pointer because child classes can be bigger.
    struct Cooldown
    {
        ID id;
        double duration;
        double t_gained = 0;

        Cooldown(ID _id = NONE, double _duration = 0) : id(_id), duration(std::max(0.0, _duration)) {}
    };

    class TrinketShared : public Cooldown
    {

    public:
        TrinketShared(double _duration = 20)
        {
            id = TRINKET_SHARED;
            duration = _duration;
        }

    };

    class Potion : public Cooldown
    {

    public:
        Potion(double _duration = 60)
        {
            id = POTION;
            duration = _duration;
        }

    };

    class IcyVeins : public Cooldown
    {

    public:
        IcyVeins()
        {
            id = ICY_VEINS;
            duration = 180;
        }

    };

    class ArcaneSurge : public Cooldown
    {

    public:
        ArcaneSurge()
        {
            id = ARCANE_SURGE;
            duration = 120;
        }

    };

    class ArcanePower : public Cooldown
    {

    public:
        ArcanePower()
        {
            id = ARCANE_POWER;
            duration = 180;
        }

    };

    class PresenceOfMind : public Cooldown
    {

    public:
        PresenceOfMind()
        {
            id = PRESENCE_OF_MIND;
            duration = 190;
        }

    };

    class FireBlast : public Cooldown
    {

    public:
        FireBlast(int imp_fire_blast = 0)
        {
            id = FIRE_BLAST;
            duration = 8 - imp_fire_blast * 0.5;
        }

    };

    class ConeOfCold : public Cooldown
    {

    public:
        ConeOfCold()
        {
            id = CONE_OF_COLD;
            duration = 10;
        }

    };

    class BlastWave : public Cooldown
    {

    public:
        BlastWave()
        {
            id = BLAST_WAVE;
            duration = 45;
        }

    };

    class Combustion : public Cooldown
    {

    public:
        Combustion()
        {
            id = COMBUSTION;
            duration = 180;
        }

    };

    class Berserking : public Cooldown
    {

    public:
        Berserking()
        {
            id = BERSERKING;
            duration = 180;
        }

    };

    class ColdSnap : public Cooldown
    {

    public:
        ColdSnap()
        {
            id = COLD_SNAP;
            duration = 600;
        }

    };

    class Evocation : public Cooldown
    {

    public:
        Evocation()
        {
            id = EVOCATION;
            duration = 480;
        }

    };

    class PowerInfusion : public Cooldown
    {

    public:
        PowerInfusion()
        {
            id = POWER_INFUSION;
            duration = 180;
        }

    };

    class ManaGem : public Cooldown
    {

    public:
        ManaGem()
        {
            id = MANA_GEM;
            duration = 120;
        }

    };

}
