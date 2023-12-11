#pragma once

#include <string>

const double RUNE_SCALE = 0.41667; // lvl 25

namespace spell
{
    enum ID : int
    {
        ARCANE_BLAST = 400574,
        ARCANE_SURGE = 425124,
        ARCANE_EXPLOSION = 8437,
        ARCANE_MISSILES = 4145,
        BLAST_WAVE = 11113, // lvl 30
        BLIZZARD = 10,
        COLD_SNAP = 11958,
        CONE_OF_COLD = 120, // lvl 26
        EVOCATION = 12051,
        FLAMESTRIKE = 2121,
        FLAMESTRIKE_DOT = 212100, // Fake id
        FLAMESTRIKE_DR = 2120,
        FLAMESTRIKE_DR_DOT = 212000, // Fake id
        FIREBALL = 8400,
        FIREBALL_DOT = 840000, // fake id
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
        SCORCH = 2948,
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
        ArcaneBlast() : Spell(ARCANE_BLAST, "Arcane Blast", SCHOOL_ARCANE)
        {
            cost_base_mana = true;
            cost = 7;
            min_dmg = 453.0 * RUNE_SCALE;
            max_dmg = 527.0 * RUNE_SCALE;
            cast_time = 2.5;
            coeff = 2.5/3.5;
        }
    };

    struct ArcaneSurge : Spell
    {
        ArcaneSurge() : Spell(ARCANE_SURGE, "Arcane Surge", SCHOOL_ARCANE)
        {
            min_dmg = 226.0 * RUNE_SCALE;
            max_dmg = 264.0 * RUNE_SCALE;
            coeff = 1.5/3.5;
        }
    };

    struct ArcaneExplosion : Spell
    {
        ArcaneExplosion() : Spell(ARCANE_EXPLOSION, "Arcane Explosion", SCHOOL_ARCANE)
        {
            cost = 120;
            min_dmg = 60;
            max_dmg = 66;
            cast_time = 0;
            coeff = 0.143;
            aoe = true;
        }
    };

    struct ArcaneMissiles : Spell
    {
        ArcaneMissiles() : Spell(ARCANE_MISSILES, "Arcane Missiles", SCHOOL_ARCANE)
        {
            cost = 235;
            min_dmg = 58;
            max_dmg = 58;
            cast_time = 5;
            coeff = 0.24;
            channeling = true;
            ticks = 5;
            speed = 20;
        }
    };

    struct Blizzard : Spell
    {
        Blizzard() : Spell(BLIZZARD, "Blizzard", SCHOOL_FROST)
        {
            cost = 320;
            min_dmg = 26;
            max_dmg = 26;
            cast_time = 8;
            coeff = 0.042;
            channeling = true;
            ticks = 8;
            aoe = true;
        }
    };

    struct Frostbolt : Spell
    {
        Frostbolt() : Spell(FROSTBOLT, "Frostbolt", SCHOOL_FROST)
        {
            cost = 65;
            min_dmg = 78;
            max_dmg = 87;
            cast_time = 2.6;
            coeff = 0.706; // 2.6 / 3.5 * 0.95
            speed = 28;
            binary = true;
        }
    };

    struct Fireball : Spell
    {
        Fireball() : Spell(FIREBALL, "Fireball", SCHOOL_FIRE)
        {
            cost = 140;
            min_dmg = 146;
            max_dmg = 195;
            cast_time = 3.5;
            coeff = 1;
            speed = 24;
        }
    };

    struct FireballDot : Spell
    {
        FireballDot() : Spell(FIREBALL_DOT, "Fireball", SCHOOL_FIRE)
        {
            dot = true;
            active_use = false;
            can_proc = false;
            coeff = 0;
            t_interval = 2;
            ticks = 4;
            min_dmg = 5;
            max_dmg = 5;
        }
    };

    struct Scorch : Spell
    {
        Scorch() : Spell(SCORCH, "Scorch", SCHOOL_FIRE)
        {
            cost = 50;
            min_dmg = 56;
            max_dmg = 69;
            cast_time = 1.5;
            coeff = 1.5/3.5;
        }
    };

    struct FireBlast : Spell
    {
        FireBlast() : Spell(FIRE_BLAST, "Fire Blast", SCHOOL_FIRE)
        {
            cost = 115;
            min_dmg = 110;
            max_dmg = 134;
            cast_time = 0;
            coeff = 1.5/3.5;
        }
    };

    struct IceLance : Spell
    {
        IceLance() : Spell(ICE_LANCE, "Ice Lance", SCHOOL_FROST)
        {
            cost_base_mana = true;
            cost = 8;
            min_dmg = 55.0 * RUNE_SCALE;
            max_dmg = 65.0 * RUNE_SCALE;
            cast_time = 0;
            coeff = 1.5/3.5/3.0;
            speed = 38;
        }
    };

    struct Pyroblast : Spell
    {
        Pyroblast() : Spell(PYROBLAST, "Pyroblast", SCHOOL_FIRE)
        {
            cost = 125;
            min_dmg = 148;
            max_dmg = 195;
            cast_time = 6;
            coeff = 1;
            speed = 24;
        }
    };

    struct PyroblastDot : Spell
    {
        PyroblastDot() : Spell(PYROBLAST_DOT, "Pyroblast", SCHOOL_FIRE)
        {
            dot = true;
            active_use = false;
            t_interval = 3;
            ticks = 4;
            min_dmg = 14;
            max_dmg = 14;
            coeff = 0.15;
        }
    };

    struct Flamestrike : Spell
    {
        Flamestrike() : Spell(FLAMESTRIKE, "Flamestrike", SCHOOL_FIRE)
        {
            aoe = true;
            cost = 330;
            min_dmg = 100;
            max_dmg = 126;
            cast_time = 3;
            coeff = 0.157;
        }
    };

    struct FlamestrikeDot : Spell
    {
        FlamestrikeDot() : Spell(FLAMESTRIKE_DOT, "Flamestrike", SCHOOL_FIRE)
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
            min_dmg = 22;
            max_dmg = 22;
            coeff = 0.02;
        }
    };

    struct FlamestrikeDR : Spell
    {
        FlamestrikeDR() : Spell(FLAMESTRIKE_DR, "Flamestrike (Rank 8)", SCHOOL_FIRE)
        {
            aoe = true;
            cost = 195;
            min_dmg = 55;
            max_dmg = 71;
            cast_time = 3;
            coeff = 0.134;
        }
    };

    struct FlamestrikeDRDot : Spell
    {
        FlamestrikeDRDot() : Spell(FLAMESTRIKE_DR_DOT, "Flamestrike (Rank 8)", SCHOOL_FIRE)
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
            min_dmg = 12;
            max_dmg = 12;
            coeff = 0.017;
        }
    };

    struct LivingBomb : Spell
    {
        LivingBomb() : Spell(LIVING_BOMB, "Living Bomb", SCHOOL_FIRE)
        {
            cost_base_mana = true;
            cost = 22;
            dot = true;
            t_interval = 3;
            ticks = 4;
            min_dmg = 85.0 * RUNE_SCALE;
            max_dmg = 85.0 * RUNE_SCALE;
            coeff = 0.2;
        }
    };

    struct LivingBombExplosion : Spell
    {
        LivingBombExplosion() : Spell(LIVING_BOMB_EXPLOSION, "Living Bomb", SCHOOL_FIRE)
        {
            aoe = true;
            active_use = false;
            min_dmg = 171.0 * RUNE_SCALE;
            max_dmg = 171.0 * RUNE_SCALE;
            coeff = 0.4;
        }
    };

    struct LivingFlame : Spell
    {
        LivingFlame(double distance) : Spell(LIVING_FLAME, "Living Flame", SCHOOL_SPELLFIRE)
        {
            double diameter = 4;
            double flames_per_second = 2;
            double flame_hit = std::floor(distance / diameter);

            if (flame_hit == 0)
                speed = 0;
            else
                speed = distance / flame_hit * flames_per_second;
            
            ticks = 20 - std::ceil(flame_hit / flames_per_second);

            aoe = true;
            cost_base_mana = true;
            cost = 11;
            dot = true;
            t_interval = 1;
            min_dmg = 100.0 * RUNE_SCALE;
            max_dmg = 100.0 * RUNE_SCALE;
            coeff = 0.143;
        }
    };

    struct BlastWave : Spell
    {
        BlastWave() : Spell(BLAST_WAVE, "Blast Wave", SCHOOL_FIRE)
        {
            aoe = true;
            cost = 215;
            min_dmg = 160;
            max_dmg = 192;
            cast_time = 0;
            coeff = 0.129;
        }
    };

    struct ConeOfCold : Spell
    {
        ConeOfCold() : Spell(CONE_OF_COLD, "Cone of Cold", SCHOOL_FROST)
        {
            aoe = true;
            cost = 210;
            min_dmg = 102;
            max_dmg = 112;
            cast_time = 0;
            coeff = 0.129;
        }
    };

    struct ColdSnap : Spell
    {
        ColdSnap() : Spell(COLD_SNAP, "Cold Snap", SCHOOL_FROST)
        {
            cost = 0;
            is_trigger = true;
            gcd = 0;
        }
    };

    struct Ignite : Spell
    {
        Ignite(double _dmg) : Spell(IGNITE, "Ignite", SCHOOL_FIRE)
        {
            dot = true;
            active_use = false;
            t_interval = 2;
            ticks = 2;
            coeff = 0;
            min_dmg = max_dmg = _dmg;
            fixed_value = true;
        }
    };

    struct ManaGem : Spell
    {
        ManaGem() : Spell(MANA_GEM, "Mana Gem", SCHOOL_ARCANE)
        {
            is_trigger = true;
            gcd = 0;
        }
    };

    struct Evocation : Spell
    {
        Evocation() : Spell(EVOCATION, "Evocation", SCHOOL_ARCANE)
        {
            is_trigger = true;
            channeling = true;
            ticks = 1;
            cast_time = 8;
        }
    };

    struct Regeneration : Spell
    {
        Regeneration() : Spell(REGENERATION, "Regeneration", SCHOOL_ARCANE)
        {
            cost_base_mana = true;
            cost = 43;
            channeling = true;
            ticks = 3;
            cast_time = 3;
            min_heal = 82.0 * RUNE_SCALE;
            max_heal = 82.0 * RUNE_SCALE;
        }
    };

    struct MassRegeneration : Spell
    {
        MassRegeneration() : Spell(MASS_REGENERATION, "Mass Regeneration", SCHOOL_ARCANE)
        {
            cost_base_mana = true;
            cost = 69;
            channeling = true;
            ticks = 3;
            cast_time = 3;
            min_heal = 42.0 * 5.0 * RUNE_SCALE;
            max_heal = 42.0 * 5.0 * RUNE_SCALE;
        }
    };

    struct TemporalBeacon : Spell
    {
        TemporalBeacon(double heal) : Spell(TEMPORAL_BEACON, "Temporal Beacon", SCHOOL_ARCANE)
        {
            active_use = false;
            cast_time = 0;
            min_heal = heal;
            max_heal = heal;
            coeff = 0;
        }
    };

}