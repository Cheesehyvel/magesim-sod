#include "buff.h"
#include "common.h"

#include <string>
#include <iomanip>

namespace buff
{

int Buff::addStack()
{
    if (stacks < max_stacks)
        stacks++;
    else
        return 0;

    return stacks;
}

int Buff::addStacks(int add)
{
    stacks+= add;
    if (stacks > max_stacks)
        stacks = max_stacks;
    else if (stacks < 0)
        stacks = 0;
    return stacks;
}

int Buff::removeStack()
{
    if (stacks > 0)
        stacks--;
    else
        return 0;

    return stacks;
}

ArcaneBlast::ArcaneBlast()
{
    id = ARCANE_BLAST;
    name = "Arcane Blast";
    duration = 6;
    max_stacks = 4;
    snapshot = true;
}

ArcaneSurge::ArcaneSurge()
{
    id = ARCANE_SURGE;
    name = "Arcane Surge";
    duration = 8;
}

ArcanePower::ArcanePower()
{
    id = ARCANE_POWER;
    name = "Arcane Power";
    duration = 15;
}

PresenceOfMind::PresenceOfMind()
{
    id = PRESENCE_OF_MIND;
    name = "Presence of Mind";
    duration = 100;
    snapshot = true;
}

IcyVeins::IcyVeins()
{
    id = ICY_VEINS;
    name = "Icy Veins";
    duration = 20;
}

Combustion::Combustion()
{
    id = COMBUSTION;
    name = "Combustion";
    duration = 2000;
    max_stacks = 20;
}

Evocation::Evocation()
{
    id = EVOCATION;
    name = "Evocation";
    duration = 8.0;
}

Clearcast::Clearcast()
{
    id = CLEARCAST;
    name = "Clearcast";
    duration = 15;
    snapshot = true;
}

PowerInfusion::PowerInfusion()
{
    id = POWER_INFUSION;
    name = "Power Infusion";
    duration = 15;
}

Innervate::Innervate()
{
    id = INNERVATE;
    name = "Innervate";
    duration = 20;
}

ManaTide::ManaTide()
{
    id = MANA_TIDE;
    name = "Mana Tide";
    duration = 12;
}

Berserking::Berserking()
{
    id = BERSERKING;
    name = "Berserking";
    duration = 10;
}

FingersOfFrost::FingersOfFrost()
{
    id = FINGERS_OF_FROST;
    name = "Fingers of Frost";
    duration = 15;
}

GhostFingers::GhostFingers()
{
    id = GHOST_FINGERS;
    name = "Fingers of Frost (Ghost proc)";
    duration = 0.01;
    hidden = true;
}

TemporalBeacon::TemporalBeacon()
{
    id = TEMPORAL_BEACON;
    name = "Temporal Beacon";
    duration = 30;
}

TemporalBeaconParty::TemporalBeaconParty()
{
    id = TEMPORAL_BEACON_PARTY;
    name = "Temporal Beacon";
    duration = 15;
}

TangledCausality::TangledCausality()
{
    id = TANGLED_CAUSALITY;
    name = "Tangled Causality";
    duration = 300;
}

MindQuickening::MindQuickening()
{
    id = MQG;
    name = "MQG";
    duration = 20;
}

}