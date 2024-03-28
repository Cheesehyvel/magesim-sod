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
        MANA_GEM = 42987,
        LIVING_FLAME = 401558,
        BRAIN_FREEZE = 400731,
        DEEP_FREEZE = 428739,

        // Trinkets
        ESSENCE_OF_SAPPHIRON = 28779,
        MANA_INFUSION = 28760,
        MQG = 23723,
        UNSTABLE_POWER = 24658,
        EPHEMERAL_POWER = 23271,
        ARCANE_POTENCY = 24544,
        OBSIDIAN_INSIGHT = 26166,
        NAT_PAGLE = 24610,
        CHROMATIC_INFUSION = 27675,
        BURST_OF_KNOWLEDGE = 15646,
        UNRESTRAINED_POWER = 446303,
        CHAOS_FIRE = 24389,

        // Enchants
        DISMANTLE = 439164,

        // Items
        MILDLY_IRRADIATED_POTION = 435973,
        CHARGED_INSPIRATION = 437327,
        COIN_FLIP = 437368,
        ELECTROMAGNETIC_GIGAFLUX_REACTIVATOR = 11826,
        ROBE_ARCHMAGE = 18385,
        CELESTIAL_ORB = 9253,
        ATALAI_SHADOW_BOLT = 446258,
        ECHOES_OF_MADNESS = 446528,
        ECHOES_OF_DEPRAVED = 446572,
        ECHOES_OF_INSANITY = 446541,
    };


    // Owned by Action and Unit.  Needs to be a pointer because child classes can be bigger.
    struct Cooldown
    {
        ID id;
        double duration;
        double t_gained = 0;
        double t_expires = 0;

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
        Potion(double _duration = 120)
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

    class LivingFlame : public Cooldown
    {

    public:
        LivingFlame()
        {
            id = LIVING_FLAME;
            duration = 30;
        }

    };

    class BrainFreeze : public Cooldown
    {

    public:
        BrainFreeze()
        {
            id = BRAIN_FREEZE;
            duration = 2;
        }

    };

    class DeepFreeze : public Cooldown
    {

    public:
        DeepFreeze()
        {
            id = DEEP_FREEZE;
            duration = 30;
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

    class EssenceOfSapphiron : public Cooldown
    {

    public:
        EssenceOfSapphiron()
        {
            id = ESSENCE_OF_SAPPHIRON;
            duration = 120;
        }

    };

    class ManaInfusion : public Cooldown
    {

    public:
        ManaInfusion()
        {
            id = MANA_INFUSION;
            duration = 180;
        }

    };

    class MindQuickening : public Cooldown
    {

    public:
        MindQuickening()
        {
            id = MQG;
            duration = 300;
        }

    };

    class UnstablePower : public Cooldown
    {

    public:
        UnstablePower()
        {
            id = UNSTABLE_POWER;
            duration = 120;
        }

    };

    class EphemeralPower : public Cooldown
    {

    public:
        EphemeralPower()
        {
            id = EPHEMERAL_POWER;
            duration = 90;
        }

    };

    class ArcanePotency : public Cooldown
    {

    public:
        ArcanePotency()
        {
            id = ARCANE_POTENCY;
            duration = 180;
        }

    };

    class ObsidianInsight : public Cooldown
    {

    public:
        ObsidianInsight()
        {
            id = OBSIDIAN_INSIGHT;
            duration = 180;
        }

    };

    class NatPagle : public Cooldown
    {

    public:
        NatPagle()
        {
            id = NAT_PAGLE;
            duration = 75;
        }

    };

    class ChromaticInfusion : public Cooldown
    {

    public:
        ChromaticInfusion()
        {
            id = CHROMATIC_INFUSION;
            duration = 75;
        }

    };

    class BurstOfKnowledge : public Cooldown
    {

    public:
        BurstOfKnowledge()
        {
            id = BURST_OF_KNOWLEDGE;
            duration = 900;
        }

    };

    class UnrestrainedPower : public Cooldown
    {

    public:
        UnrestrainedPower()
        {
            id = UNRESTRAINED_POWER;
            duration = 120;
        }

    };

    class ChaosFire : public Cooldown
    {

    public:
        ChaosFire()
        {
            id = CHAOS_FIRE;
            duration = 180;
        }

    };

    class Dismantle : public Cooldown
    {

    public:
        Dismantle()
        {
            id = DISMANTLE;
            duration = 1;
        }

    };

    class MildlyIrradiatedPotion : public Cooldown
    {

    public:
        MildlyIrradiatedPotion()
        {
            id = MILDLY_IRRADIATED_POTION;
            duration = 120;
        }

    };

    class ChargedInspiration : public Cooldown
    {

    public:
        ChargedInspiration()
        {
            id = CHARGED_INSPIRATION;
            duration = 600;
        }

    };

    class CoinFlip : public Cooldown
    {

    public:
        CoinFlip()
        {
            id = COIN_FLIP;
            duration = 900;
        }

    };

    class ElectromagneticGigafluxReactivator : public Cooldown
    {

    public:
        ElectromagneticGigafluxReactivator()
        {
            id = ELECTROMAGNETIC_GIGAFLUX_REACTIVATOR;
            duration = 1800;
        }

    };

    class RobeArchmage : public Cooldown
    {

    public:
        RobeArchmage()
        {
            id = ROBE_ARCHMAGE;
            duration = 300;
        }

    };

    class CelestialOrb : public Cooldown
    {

    public:
        CelestialOrb()
        {
            id = CELESTIAL_ORB;
            duration = 1800;
        }

    };

    class AtalaiShadowBolt : public Cooldown
    {

    public:
        AtalaiShadowBolt()
        {
            id = ATALAI_SHADOW_BOLT;
            duration = 40;
        }

    };

    class EchoesOfMadness : public Cooldown
    {

    public:
        EchoesOfMadness()
        {
            id = ECHOES_OF_MADNESS;
            duration = 40;
        }

    };

    class EchoesOfDepraved : public Cooldown
    {

    public:
        EchoesOfDepraved()
        {
            id = ECHOES_OF_DEPRAVED;
            duration = 40;
        }

    };

    class EchoesOfInsanity : public Cooldown
    {

    public:
        EchoesOfInsanity()
        {
            id = ECHOES_OF_INSANITY;
            duration = 40;
        }

    };

}
