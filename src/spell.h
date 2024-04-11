#pragma once

#include "common.h"

#include <string>

namespace spell
{
    enum ID : int
    {
        ARCANE_BLAST = 400574,
        ARCANE_SURGE = 425124,
        ARCANE_EXPLOSION = 8437,
        ARCANE_MISSILES = 4145,
        BLAST_WAVE = 11113,
        BLIZZARD = 10,
        COLD_SNAP = 11958,
        CONE_OF_COLD = 120,
        EVOCATION = 12051,
        FLAMESTRIKE = 2121,
        FLAMESTRIKE_DOT = 212100, // Fake id
        FLAMESTRIKE_DR = 2120,
        FLAMESTRIKE_DR_DOT = 212000, // Fake id
        FIREBALL = 8400,
        FIREBALL_DOT = 840000, // fake id
        FROSTFIRE_BOLT = 401502,
        FROSTFIRE_BOLT_DOT = 4015020, // fake id
        SPELLFROST_BOLT = 412532,
        BALEFIRE_BOLT = 428878,
        DEEP_FREEZE = 428739,
        SCORCH = 2948,
        FIRE_BLAST = 2138,
        FROSTBOLT = 7322,
        ICE_LANCE = 400640,
        IGNITE = 12848,
        LIVING_BOMB = 400613,
        LIVING_BOMB_EXPLOSION = 400614,
        LIVING_FLAME = 401558,
        MANA_GEM = 42987,
        MASS_REGENERATION = 412510,
        PYROBLAST = 11366,
        PYROBLAST_DOT = 1136600, // Fake id
        REGENERATION = 401417,
        REWIND_TIME = 401462,
        TEMPORAL_BEACON = 400735,

        // Trinkets
        CHAOS_FIRE = 24389,
        MANA_INFUSION = 28760,

        // Enchants
        DISMANTLE = 439164,

        // Items
        MILDLY_IRRADIATED_POTION = 435973,
        ELECTROMAGNETIC_GIGAFLUX_REACTIVATOR = 11826,
        CHARGED_INSPIRATION = 437327,
        COIN_FLIP = 437368,
        ROBE_ARCHMAGE = 18385,
        CELESTIAL_ORB = 9253,
        ATALAI_SHADOW_BOLT = 446258,
        ENGULFING_SHADOWS = 27860,
    };

    enum Result : int
    {
        NONE,
        HIT,
        CRIT,
        MISS,
        PENDING
    };

    struct Spell
    {
        const ID id;
        const std::string name;
        double cost = 0;
        double min_dmg = 0;
        double max_dmg = 0;
        double min_heal = 0;
        double max_heal = 0;
        double cast_time = 0;
        double delay = 0;
        double coeff = 1;
        double gcd = 1.5;
        double speed = 0;
        bool cost_base_mana = false;
        bool channeling = false;
        bool dot = false;
        bool proc = false;
        bool binary = false;
        bool aoe = false;
        bool aoe_capped = true;
        bool fixed_value = false;
        bool active_use = true;
        bool off_gcd = false;
        bool is_trigger = false;
        bool can_proc = true;
        bool tick_refresh = true;
        bool dynamic = false;
        int ticks = 0;
        int t_interval = 1;
        const School school = SCHOOL_NONE;

        double actual_cost = 0;
        double actual_cast_time = 0;
        int tick = 0;
        bool done = false;

        double avgDmg() const
        {
            return (min_dmg + max_dmg)/2.0;
        }

        double avgHeal() const
        {
            return (min_heal + max_heal)/2.0;
        }

        bool isSchool(School cmp) const
        {
            return ::isSchool(school, cmp);
        }

        bool isSchool(School cmp1, School cmp2) 
        {
            return ::isSchool(school, cmp1, cmp2);
        }

        // TODO: what other fields can be const-init?
        Spell(ID _id, const std::string& _name, School _school)
            : id(_id), name(_name), school(_school) {}
    };

    struct SpellInstance
    {
        std::shared_ptr<Spell> spell;
        Result result = Result::NONE;
        double dmg = 0;
        double initial_dmg = 0;
        double heal = 0;
        double resist = 0;
        int tick = 0;
    };

    struct ArcaneBlast : Spell
    {
        ArcaneBlast(int lvl) : Spell(ARCANE_BLAST, "Arcane Blast", SCHOOL_ARCANE)
        {
            cost_base_mana = true;
            cost = 7;
            min_dmg = 453.0 * defaultRuneScale(lvl);
            max_dmg = 527.0 * defaultRuneScale(lvl);
            cast_time = 2.5;
            coeff = 2.5/3.5;
        }
    };

    struct ArcaneSurge : Spell
    {
        ArcaneSurge(int lvl) : Spell(ARCANE_SURGE, "Arcane Surge", SCHOOL_ARCANE)
        {
            min_dmg = 226.0 * defaultRuneScale(lvl);
            max_dmg = 264.0 * defaultRuneScale(lvl);
            coeff = 1.5/3.5;
        }
    };

    struct ArcaneExplosion : Spell
    {
        ArcaneExplosion(int lvl) : Spell(ARCANE_EXPLOSION, "Arcane Explosion", SCHOOL_ARCANE)
        {
            cost = 120;
            min_dmg = 60;
            max_dmg = 66;
            cast_time = 0;
            coeff = 0.143;
            aoe = true;

            if (lvl >= 54) {
                cost = 390;
                min_dmg = 249;
                max_dmg = 270;
            }
            else if (lvl >= 46) {
                cost = 315;
                min_dmg = 191;
                max_dmg = 208;
            }
            else if (lvl >= 38) {
                cost = 250;
                min_dmg = 143;
                max_dmg = 156;
            }
        }
    };

    struct ArcaneMissiles : Spell
    {
        ArcaneMissiles(int lvl) : Spell(ARCANE_MISSILES, "Arcane Missiles", SCHOOL_ARCANE)
        {
            cost = 235;
            min_dmg = max_dmg = 58;
            cast_time = 5;
            coeff = 0.24;
            channeling = true;
            ticks = 5;
            speed = 20;

            // AQ20 book
            // if (lvl >= 60) {
            //     cost = 655;
            //     min_dmg = 230;
            //     max_dmg = 230;
            // }
            if (lvl >= 56) {
                cost = 595;
                min_dmg = max_dmg = 196;
            }
            else if (lvl >= 48) {
                cost = 500;
                min_dmg = max_dmg = 155;
            }
            else if (lvl >= 40) {
                cost = 410;
                min_dmg = max_dmg = 118;
            }
        }
    };

    struct Blizzard : Spell
    {
        Blizzard(int lvl) : Spell(BLIZZARD, "Blizzard", SCHOOL_FROST)
        {
            cost = 320;
            min_dmg = max_dmg = 26;
            cast_time = 8;
            coeff = 0.042;
            channeling = true;
            ticks = 8;
            aoe = true;

            if (lvl >= 60) {
                cost = 1400;
                min_dmg = max_dmg = 149;
            }
            else if (lvl >= 44) {
                cost = 935;
                min_dmg = max_dmg = 90;
            }
            else if (lvl >= 36) {
                cost = 720;
                min_dmg = max_dmg = 65;
            }
        }
    };

    struct Frostbolt : Spell
    {
        Frostbolt(int lvl) : Spell(FROSTBOLT, "Frostbolt", SCHOOL_FROST)
        {
            cost = 65;
            min_dmg = 78;
            max_dmg = 87;
            cast_time = 3;
            coeff = 0.814; // 3.0 / 3.5 * 0.95
            speed = 28;
            binary = true;

            // AQ20 book
            // if (lvl >= 60) {
            //     cost = 290;
            //     min_dmg = 515;
            //     max_dmg = 555;
            // }
            if (lvl >= 56) {
                cost = 260;
                min_dmg = 440;
                max_dmg = 475;
            }
            else if (lvl >= 50) {
                cost = 225;
                min_dmg = 363;
                max_dmg = 394;
            }
            else if (lvl >= 38) {
                cost = 160;
                min_dmg = 235;
                max_dmg = 255;
            }
            else {
                cast_time = 2.6;
                coeff = 0.706; // 2.6 / 3.5 * 0.95
            }
        }
    };

    struct Fireball : Spell
    {
        Fireball(int lvl) : Spell(FIREBALL, "Fireball", SCHOOL_FIRE)
        {
            cost = 140;
            min_dmg = 146;
            max_dmg = 195;
            cast_time = 3.5;
            coeff = 1;
            speed = 24;

            // AQ20 book
            // if (lvl >= 60) {
            //     cost = 410;
            //     min_dmg = 596;
            //     max_dmg = 760;
            // }
            if (lvl >= 60) {
                cost = 395;
                min_dmg = 561;
                max_dmg = 715;
            }
            else if (lvl >= 48) {
                cost = 305;
                min_dmg = 404;
                max_dmg = 518;
            }
            else if (lvl >= 36) {
                cost = 220;
                min_dmg = 264;
                max_dmg = 345;
            }
        }
    };

    struct FireballDot : Spell
    {
        FireballDot(int lvl) : Spell(FIREBALL_DOT, "Fireball", SCHOOL_FIRE)
        {
            dot = true;
            active_use = false;
            can_proc = false;
            coeff = 0;
            t_interval = 2;
            ticks = 4;
            min_dmg = max_dmg = 5;

            // AQ20 book
            // if (lvl >= 60) {
            //     min_dmg = max_dmg = 19;
            // }
            if (lvl >= 60) {
                min_dmg = max_dmg = 18;
            }
            else if (lvl >= 48) {
                min_dmg = max_dmg = 13;
            }
            else if (lvl >= 36) {
                min_dmg = max_dmg = 8;
            }
        }
    };

    struct FrostfireBolt : Spell
    {
        FrostfireBolt(int lvl) : Spell(FROSTFIRE_BOLT, "Frostfire Bolt", SCHOOL_FROSTFIRE)
        {
            cost = 14;
            cost_base_mana = true;
            min_dmg = 387 * defaultRuneScale(lvl);
            max_dmg = 450 * defaultRuneScale(lvl);
            cast_time = 3;
            coeff = 1;
            speed = 24;
        }
    };

    struct FrostfireBoltDot : Spell
    {
        FrostfireBoltDot(int lvl) : Spell(FROSTFIRE_BOLT_DOT, "Frostfire Bolt", SCHOOL_FROSTFIRE)
        {
            dot = true;
            active_use = false;
            can_proc = false;
            t_interval = 3;
            ticks = 3;
            coeff = 0;
            min_dmg = max_dmg = 8 * defaultRuneScale(lvl);
        }
    };

    struct SpellfrostBolt : Spell
    {
        SpellfrostBolt(int lvl) : Spell(SPELLFROST_BOLT, "Spellfrost Bolt", SCHOOL_SPELLFROST)
        {
            cost = 12;
            cost_base_mana = true;
            min_dmg = 304 * defaultRuneScale(lvl);
            max_dmg = 355 * defaultRuneScale(lvl);
            cast_time = 2.5;
            coeff = 0.814; // 3.0 / 3.5 * 0.95
            speed = 24;
        }
    };

    struct BalefireBolt : Spell
    {
        BalefireBolt(int lvl) : Spell(BALEFIRE_BOLT, "Balefire Bolt", SCHOOL_SPELLFIRE)
        {
            cost = 20;
            cost_base_mana = true;
            min_dmg = 280 * defaultRuneScale(lvl);
            max_dmg = 420 * defaultRuneScale(lvl);
            cast_time = 2.5;
            coeff = 3.0/3.5;
            speed = 24;
        }
    };

    struct DeepFreeze : Spell
    {
        DeepFreeze(int lvl) : Spell(DEEP_FREEZE, "Deep Freeze", SCHOOL_FROST)
        {
            cost = 9;
            cost_base_mana = true;
            min_dmg = 462 * defaultRuneScale(lvl);
            max_dmg = 538 * defaultRuneScale(lvl);
            coeff = 2.5;
        }
    };

    struct Scorch : Spell
    {
        Scorch(int lvl) : Spell(SCORCH, "Scorch", SCHOOL_FIRE)
        {
            cost = 50;
            min_dmg = 56;
            max_dmg = 69;
            cast_time = 1.5;
            coeff = 1.5/3.5;

            if (lvl >= 58) {
                cost = 150;
                min_dmg = 237;
                max_dmg = 280;
            }
            else if (lvl >= 46) {
                cost = 115;
                min_dmg = 168;
                max_dmg = 199;
            }
            else if (lvl >= 40) {
                cost = 100;
                min_dmg = 139;
                max_dmg = 165;
            }
        }
    };

    struct FireBlast : Spell
    {
        FireBlast(int lvl) : Spell(FIRE_BLAST, "Fire Blast", SCHOOL_FIRE)
        {
            cost = 115;
            min_dmg = 110;
            max_dmg = 134;
            cast_time = 0;
            coeff = 1.5/3.5;

            if (lvl >= 54) {
                cost = 340;
                min_dmg = 446;
                max_dmg = 524;
            }
            else if (lvl >= 46) {
                cost = 280;
                min_dmg = 345;
                max_dmg = 407;
            }
            else if (lvl >= 38) {
                cost = 220;
                min_dmg = 253;
                max_dmg = 301;
            }
        }
    };

    struct IceLance : Spell
    {
        IceLance(int lvl) : Spell(ICE_LANCE, "Ice Lance", SCHOOL_FROST)
        {
            cost_base_mana = true;
            cost = 8;
            min_dmg = 55.0 * defaultRuneScale(lvl);
            max_dmg = 65.0 * defaultRuneScale(lvl);
            cast_time = 0;
            coeff = 1.5/3.5/3.0;
            speed = 38;
        }
    };

    struct Pyroblast : Spell
    {
        Pyroblast(int lvl) : Spell(PYROBLAST, "Pyroblast", SCHOOL_FIRE)
        {
            cost = 125;
            min_dmg = 148;
            max_dmg = 195;
            cast_time = 6;
            coeff = 1;
            speed = 24;

            if (lvl >= 60) {
                cost = 440;
                min_dmg = 716;
                max_dmg = 890;
            }
            else if (lvl >= 48) {
                cost = 335;
                min_dmg = 525;
                max_dmg = 654;
            }
            else if (lvl >= 36) {
                cost = 240;
                min_dmg = 347;
                max_dmg = 437;
            }
        }
    };

    struct PyroblastDot : Spell
    {
        PyroblastDot(int lvl) : Spell(PYROBLAST_DOT, "Pyroblast", SCHOOL_FIRE)
        {
            dot = true;
            active_use = false;
            t_interval = 3;
            ticks = 4;
            min_dmg = max_dmg = 14;
            coeff = 0.15;

            if (lvl >= 60) {
                min_dmg = max_dmg = 67;
            }
            else if (lvl >= 48) {
                min_dmg = max_dmg = 47;
            }
            else if (lvl >= 36) {
                min_dmg = max_dmg = 31;
            }
        }
    };

    struct Flamestrike : Spell
    {
        Flamestrike(int lvl) : Spell(FLAMESTRIKE, "Flamestrike", SCHOOL_FIRE)
        {
            aoe = true;
            cost = 330;
            min_dmg = 100;
            max_dmg = 126;
            cast_time = 3;
            coeff = 0.157;

            if (lvl >= 56) {
                cost = 990;
                min_dmg = 381;
                max_dmg = 466;
            }
            else if (lvl >= 48) {
                cost = 815;
                min_dmg = 298;
                max_dmg = 367;
            }
            else if (lvl >= 40) {
                cost = 650;
                min_dmg = 226;
                max_dmg = 279;
            }
        }
    };

    struct FlamestrikeDot : Spell
    {
        FlamestrikeDot(int lvl) : Spell(FLAMESTRIKE_DOT, "Flamestrike", SCHOOL_FIRE)
        {
            aoe = true;
            aoe_capped = false;
            dot = true;
            active_use = false;
            can_proc = false;
            tick_refresh = false;
            dynamic = true;
            t_interval = 2;
            ticks = 4;
            min_dmg = max_dmg = 22;
            coeff = 0.02;

            if (lvl >= 56) {
                min_dmg = max_dmg = 85;
            }
            else if (lvl >= 48) {
                min_dmg = max_dmg = 66;
            }
            else if (lvl >= 40) {
                min_dmg = max_dmg = 49;
            }
        }
    };

    struct FlamestrikeDR : Spell
    {
        FlamestrikeDR(int lvl) : Spell(FLAMESTRIKE_DR, "Flamestrike (Downranked)", SCHOOL_FIRE)
        {
            aoe = true;
            cost = 195;
            min_dmg = 55;
            max_dmg = 71;
            cast_time = 3;
            coeff = 0.157;

             if (lvl >= 56) {
                cost = 815;
                min_dmg = 298;
                max_dmg = 367;
            }
            else if (lvl >= 48) {
                cost = 650;
                min_dmg = 226;
                max_dmg = 279;
            }
            else if (lvl >= 40) {
                cost = 490;
                min_dmg = 159;
                max_dmg = 197;
            }
            else {
                coeff = 0.134;
            }
        }
    };

    struct FlamestrikeDRDot : Spell
    {
        FlamestrikeDRDot(int lvl) : Spell(FLAMESTRIKE_DR_DOT, "Flamestrike (Downranked)", SCHOOL_FIRE)
        {
            aoe = true;
            aoe_capped = false;
            dot = true;
            active_use = false;
            can_proc = false;
            tick_refresh = false;
            dynamic = true;
            t_interval = 2;
            ticks = 4;
            min_dmg = max_dmg = 12;
            coeff = 0.017;

            if (lvl >= 56) {
                min_dmg = max_dmg = 66;
            }
            else if (lvl >= 48) {
                min_dmg = max_dmg = 49;
            }
            else if (lvl >= 40) {
                min_dmg = max_dmg = 35;
            }
            else {
                coeff = 0.02;
            }
        }
    };

    struct LivingBomb : Spell
    {
        LivingBomb(int lvl) : Spell(LIVING_BOMB, "Living Bomb", SCHOOL_FIRE)
        {
            cost_base_mana = true;
            cost = 22;
            dot = true;
            t_interval = 3;
            ticks = 4;
            min_dmg = max_dmg = 85.0 * defaultRuneScale(lvl);
            coeff = 0.2;
        }
    };

    struct LivingBombExplosion : Spell
    {
        LivingBombExplosion(int lvl) : Spell(LIVING_BOMB_EXPLOSION, "Living Bomb", SCHOOL_FIRE)
        {
            aoe = true;
            active_use = false;
            min_dmg = 171.0 * defaultRuneScale(lvl);
            max_dmg = 171.0 * defaultRuneScale(lvl);
            coeff = 0.4;
        }
    };

    struct LivingFlame : Spell
    {
        LivingFlame(int lvl, double distance) : Spell(LIVING_FLAME, "Living Flame", SCHOOL_SPELLFIRE)
        {
            double diameter = 3;
            double flames_per_second = 2;
            double flame_hit = std::floor(distance / diameter);

            if (flame_hit == 0)
                speed = 0;
            else
                speed = distance / flame_hit * flames_per_second;
            
            ticks = 10 - std::ceil(flame_hit / flames_per_second);

            aoe = true;
            cost_base_mana = true;
            cost = 11;
            dot = true;
            t_interval = 1;
            min_dmg = max_dmg = 100.0 * defaultRuneScale(lvl);
            coeff = 0.143;
        }
    };

    struct BlastWave : Spell
    {
        BlastWave(int lvl) : Spell(BLAST_WAVE, "Blast Wave", SCHOOL_FIRE)
        {
            aoe = true;
            cost = 215;
            min_dmg = 160;
            max_dmg = 192;
            cast_time = 0;
            coeff = 0.129;

             if (lvl >= 60) {
                cost = 545;
                min_dmg = 462;
                max_dmg = 544;
            }
            else if (lvl >= 44) {
                cost = 355;
                min_dmg = 285;
                max_dmg = 338;
            }
            else if (lvl >= 36) {
                cost = 270;
                min_dmg = 208;
                max_dmg = 249;
            }
        }
    };

    struct ConeOfCold : Spell
    {
        ConeOfCold(int lvl) : Spell(CONE_OF_COLD, "Cone of Cold", SCHOOL_FROST)
        {
            aoe = true;
            cost = 210;
            min_dmg = 102;
            max_dmg = 112;
            cast_time = 0;
            coeff = 0.129;

             if (lvl >= 58) {
                cost = 555;
                min_dmg = 338;
                max_dmg = 368;
            }
            else if (lvl >= 50) {
                cost = 465;
                min_dmg = 270;
                max_dmg = 297;
            }
            else if (lvl >= 34) {
                cost = 290;
                min_dmg = 151;
                max_dmg = 165;
            }
        }
    };

    struct ColdSnap : Spell
    {
        ColdSnap(int lvl) : Spell(COLD_SNAP, "Cold Snap", SCHOOL_FROST)
        {
            is_trigger = true;
            gcd = 0;
        }
    };

    struct Ignite : Spell
    {
        Ignite(int lvl, double _dmg) : Spell(IGNITE, "Ignite", SCHOOL_FIRE)
        {
            dot = true;
            active_use = false;
            t_interval = 2;
            ticks = 2;
            coeff = 0;
            dynamic = true;
            min_dmg = max_dmg = _dmg;
        }
    };

    struct ManaGem : Spell
    {
        ManaGem(int lvl) : Spell(MANA_GEM, "Mana Gem", SCHOOL_ARCANE)
        {
            is_trigger = true;
            gcd = 0;
        }
    };

    struct Evocation : Spell
    {
        Evocation(int lvl) : Spell(EVOCATION, "Evocation", SCHOOL_ARCANE)
        {
            is_trigger = true;
            channeling = true;
            ticks = 1;
            cast_time = 8;
        }
    };

    struct Regeneration : Spell
    {
        Regeneration(int lvl) : Spell(REGENERATION, "Regeneration", SCHOOL_ARCANE)
        {
            cost_base_mana = true;
            cost = 43;
            channeling = true;
            ticks = 3;
            cast_time = 3;
            coeff = 0.457;
            min_heal = 82.0 * defaultHealingRuneScale(lvl);
            max_heal = 82.0 * defaultHealingRuneScale(lvl);
        }
    };

    struct MassRegeneration : Spell
    {
        MassRegeneration(int lvl) : Spell(MASS_REGENERATION, "Mass Regeneration", SCHOOL_ARCANE)
        {
            cost_base_mana = true;
            cost = 69;
            channeling = true;
            ticks = 3;
            cast_time = 3;
            coeff = 0.153;
            min_heal = 42.0 * 5.0 * defaultHealingRuneScale(lvl);
            max_heal = 42.0 * 5.0 * defaultHealingRuneScale(lvl);
        }
    };

    struct TemporalBeacon : Spell
    {
        TemporalBeacon(int lvl, double heal) : Spell(TEMPORAL_BEACON, "Temporal Beacon", SCHOOL_ARCANE)
        {
            active_use = false;
            cast_time = 0;
            min_heal = heal;
            max_heal = heal;
            coeff = 0;
        }
    };

    struct ChaosFire : Spell
    {
        ChaosFire(int lvl) : Spell(CHAOS_FIRE, "Chaos Fire", SCHOOL_PHYSICAL)
        {
            is_trigger = true;
            gcd = 0;
        }
    };

    struct ManaInfusion : Spell
    {
        ManaInfusion(int lvl) : Spell(MANA_INFUSION, "Mana Infusion", SCHOOL_PHYSICAL)
        {
            is_trigger = true;
            gcd = 0;
        }
    };

    struct Dismantle : Spell
    {
        Dismantle(int lvl) : Spell(DISMANTLE, "Dismantle", SCHOOL_NATURE)
        {
            active_use = false;
            min_dmg = 60;
            max_dmg = 90;
            gcd = 0;
            coeff = 0;
            can_proc = false;
            proc = true;
        }
    };

    struct MildlyIrradiatedPotion : Spell
    {
        MildlyIrradiatedPotion(int lvl) : Spell(MILDLY_IRRADIATED_POTION, "Mildly Irradiated Rejuvenation Potion", SCHOOL_PHYSICAL)
        {
            is_trigger = true;
            gcd = 0;
        }
    };


    struct ElectromagneticGigafluxReactivator : Spell
    {
        ElectromagneticGigafluxReactivator(int lvl) : Spell(ELECTROMAGNETIC_GIGAFLUX_REACTIVATOR, "Electromagnetic Gigaflux Reactivator", SCHOOL_NATURE)
        {
            aoe = true;
            min_dmg = 152;
            max_dmg = 172;
            gcd = 0;
        }
    };

    struct ChargedInspiration : public Spell
    {
        ChargedInspiration(int lvl) : Spell(CHARGED_INSPIRATION, "Charged Inspiration", SCHOOL_PHYSICAL)
        {
            is_trigger = true;
            gcd = 0;
        }
    };

    struct CoinFlip : public Spell
    {
        CoinFlip(int lvl) : Spell(COIN_FLIP, "Coin Flip", SCHOOL_PHYSICAL)
        {
            is_trigger = true;
            gcd = 0;
        }
    };

    struct RobeArchmage : public Spell
    {
        RobeArchmage(int lvl) : Spell(ROBE_ARCHMAGE, "Robe of the Archmage", SCHOOL_PHYSICAL)
        {
            is_trigger = true;
            gcd = 0;
        }
    };

    struct CelestialOrb : public Spell
    {
        CelestialOrb(int lvl) : Spell(CELESTIAL_ORB, "Celestial Orb", SCHOOL_PHYSICAL)
        {
            is_trigger = true;
            gcd = 0;
        }
    };

    struct AtalaiShadowBolt : public Spell
    {
        AtalaiShadowBolt(int lvl) : Spell(ATALAI_SHADOW_BOLT, "Shadow Bolt", SCHOOL_SHADOW)
        {
            active_use = false;
            min_dmg = 204;
            max_dmg = 236;
            gcd = 0;
            coeff = 0.56;
            can_proc = false;
            proc = true;
            speed = 20;
        }
    };

    struct EngulfingShadows : public Spell
    {
        EngulfingShadows(int lvl) : Spell(ENGULFING_SHADOWS, "Engulfing Shadows", SCHOOL_SHADOW)
        {
            active_use = false;
            min_dmg = 100;
            max_dmg = 100;
            gcd = 0;
            coeff = 0;
            can_proc = false;
            proc = true;
        }
    };

}