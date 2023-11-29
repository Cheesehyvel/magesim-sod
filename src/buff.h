#pragma once

#include "stats.h"

#include <string>
#include <sstream>

namespace buff
{
enum ID : int
{
    ARCANE_BLAST = 30451,
    ARCANE_SURGE = 425124,
    ARCANE_POWER = 12042,
    PRESENCE_OF_MIND = 12043,
    ICY_VEINS = 12472,
    COMBUSTION = 29977,
    EVOCATION = 12051,
    POWER_INFUSION = 10060,
    INNERVATE = 29166,
    CLEARCAST = 12536,
    MANA_TIDE = 39609,
    BERSERKING = 20554,
    BRAIN_FREEZE = 57761,
    FINGERS_OF_FROST = 44544,
    GHOST_FINGERS = 4454411,
    TEMPORAL_BEACON = 400735,
    TEMPORAL_BEACON_PARTY = 4007350, // Fake id
    TANGLED_CAUSALITY = 432069,
    MQG = 23723,
};


// Prolonged ownership by Event and Unit
// TODO: Do Event and Unit really need the same copy (i.e. pointer)?
struct Buff
{
    ID id;
    std::string name;
    double duration;
    int stacks = 1;
    int stack_increment = 1;
    int max_stacks = 1;
    bool stack_refresh = true;
    bool show_refresh = false;
    bool hidden = false;
    bool snapshot = false;
    double cost = 0;
    double t_gained = 0;
    double t_refreshed = 0;
    Stats stats;

    int addStack();
    int addStacks(int add);
    int removeStack();
};

class ArcaneBlast : public Buff
{

public:
    ArcaneBlast();

};

class ArcaneSurge : public Buff
{

public:
    ArcaneSurge();

};

class ArcanePower : public Buff
{

public:
    ArcanePower();

};

class PresenceOfMind : public Buff
{

public:
    PresenceOfMind();

};

class IcyVeins : public Buff
{

public:
    IcyVeins();

};

class Combustion : public Buff
{

public:
    Combustion();
};

class Evocation : public Buff
{

public:
    Evocation();
};

class Clearcast : public Buff
{

public:
    Clearcast();
};

class PowerInfusion : public Buff
{

public:
    PowerInfusion();
};

class Innervate : public Buff
{

public:
    Innervate();

};

class ManaTide : public Buff
{

public:
    ManaTide();
};

class Berserking : public Buff
{

public:
    Berserking();
};

class FingersOfFrost : public Buff
{

public:
    FingersOfFrost();
};

class GhostFingers : public Buff
{

public:
    GhostFingers();
};

class TemporalBeacon : public Buff
{

public:
    TemporalBeacon();
};

class TemporalBeaconParty : public Buff
{

public:
    TemporalBeaconParty();
};

class TangledCausality : public Buff
{

public:
    TangledCausality();
};

class MindQuickening : public Buff
{

public:
    MindQuickening();
};

}