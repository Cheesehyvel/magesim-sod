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

HotStreak::HotStreak()
{
    id = HOT_STREAK;
    name = "Hot Streak";
    duration = 10;
}

HeatingUp::HeatingUp()
{
    id = HEATING_UP;
    name = "Heating up";
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

MissileBarrage::MissileBarrage()
{
    id = MISSILE_BARRAGE;
    name = "Missile Barrage";
    duration = 15;
}

BrainFreeze::BrainFreeze()
{
    id = BRAIN_FREEZE;
    name = "Brain Freeze";
    duration = 15;
}

BalefireBolt::BalefireBolt()
{
    id = BALEFIRE_BOLT;
    name = "Balefire Bolt";
    duration = 30;
    max_stacks = 10;
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

EssenceOfSapphiron::EssenceOfSapphiron()
{
    id = ESSENCE_OF_SAPPHIRON;
    name = "Essence of Sapphiron";
    duration = 20;
    stats.sp = 130;
}

MindQuickening::MindQuickening()
{
    id = MQG;
    name = "Mind Quickening";
    duration = 20;
}

UnstablePower::UnstablePower()
{
    id = UNSTABLE_POWER;
    name = "Unstable Power";
    max_stacks = 12;
    stack_increment = -1;
    stack_refresh = false;
    duration = 20;
    stats.sp = 17;
}

EphemeralPower::EphemeralPower()
{
    id = EPHEMERAL_POWER;
    name = "Ephemeral Power";
    duration = 15;
    stats.sp = 175;
}

ArcanePotency::ArcanePotency()
{
    id = ARCANE_POTENCY;
    name = "Arcane Potency";
    duration = 20;
}

ObsidianInsight::ObsidianInsight()
{
    id = OBSIDIAN_INSIGHT;
    name = "Obsidian Insight";
    duration = 30;
    stats.sp = 50;
    stats.spen = 100;
}

BlueDragon::BlueDragon()
{
    id = BLUE_DRAGON;
    name = "Aura of the Blue Dragon";
    duration = 15;
}

NatPagle::NatPagle()
{
    id = NAT_PAGLE;
    name = "Pagle's Broken Reel";
    duration = 15;
    stats.hit = 10;
}

ChromaticInfusion::ChromaticInfusion()
{
    id = CHROMATIC_INFUSION;
    name = "Chromatic Infusion";
    duration = 15;
    stats.sp = 100;
}

BurstOfKnowledge::BurstOfKnowledge()
{
    id = BURST_OF_KNOWLEDGE;
    name = "Burst of Knowledge";
    duration = 10;
}

UnrestrainedPower::UnrestrainedPower()
{
    id = UNRESTRAINED_POWER;
    name = "Unrestrained Power";
    max_stacks = 24;
    stack_increment = -1;
    stack_refresh = false;
    duration = 20;
    stats.sp = 8;
}

ChaosFire::ChaosFire()
{
    id = CHAOS_FIRE;
    name = "Chaos Fire";
    duration = 60;
    stats.sp_fire = 100;
}

NetherwindFocus::NetherwindFocus()
{
    id = NETHERWIND_FOCUS;
    name = "Netherwind Focus";
    duration = 10;
}

EnigmasAnswer::EnigmasAnswer()
{
    id = ENIGMAS_ANSWER;
    name = "Enigma's Answer";
    duration = 20;
    max_stacks = 4;
    stats.hit = 5;
}

EnergizedHyperconductor::EnergizedHyperconductor()
{
    id = ENERGIZED_HYPERCONDUCTOR;
    name = "Energized Hyperconductor";
    duration = 10;
    stats.sp = 40;
}

ChargedInspiration::ChargedInspiration()
{
    id = CHARGED_INSPIRATION;
    name = "Charged Inspiration";
    duration = 12;
    stats.sp = 50;
}

CoinFlipHeads::CoinFlipHeads()
{
    id = COIN_FLIP_HEADS;
    name = "Coin Flip: Heads";
    duration = 30;
    stats.crit = 3;
}

CoinFlipTails::CoinFlipTails()
{
    id = COIN_FLIP_TAILS;
    name = "Coin Flip: Tails";
    duration = 30;
}

MildlyIrradiated::MildlyIrradiated()
{
    id = MILDLY_IRRADIATED;
    name = "Mildly Irradiated";
    duration = 15;
    stats.sp = 35;
}

EchoesOfMadness::EchoesOfMadness()
{
    id = ECHOES_OF_MADNESS;
    name = "Echoes of Madness";
    duration = 10;
}

EchoesOfDepraved::EchoesOfDepraved()
{
    id = ECHOES_OF_DEPRAVED;
    name = "Echoes of Depraved";
    duration = 10;
    stats.sp = 30;
}

EchoesOfInsanity::EchoesOfInsanity()
{
    id = ECHOES_OF_INSANITY;
    name = "Echoes of Insanity";
    duration = 10;
}

RoarOfTheDream::RoarOfTheDream()
{
    id = ROAR_OF_THE_DREAM;
    name = "Roar of the Dream";
    duration = 10;
    stats.sp = 66;
}

}