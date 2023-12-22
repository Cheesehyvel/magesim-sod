#pragma once

#include <random>

#define MANA_TICK_T 2.0

enum Rotation : int
{
    ROTATION_ST_ARCANE = 10,
    ROTATION_ST_FIRE = 20,
    ROTATION_ST_FIRE_SC = 21,
    ROTATION_ST_FROST = 30,
    ROTATION_AOE_AE = 100,
    ROTATION_AOE_AE_FS = 101,
    ROTATION_AOE_BLIZZ = 110,
    ROTATION_AOE_BLIZZ_FS = 111,
    ROTATION_AOE_FIRE = 120,
    ROTATION_AOE_FS = 130,
};

enum Race : int
{
    RACE_GNOME,
    RACE_HUMAN,
    RACE_TROLL,
    RACE_UNDEAD
};

enum Faction : int
{
    FACTION_HORDE,
    FACTION_ALLIANCE,
};

enum School : int
{
    SCHOOL_NONE,
    SCHOOL_ARCANE,
    SCHOOL_FROST,
    SCHOOL_FIRE,
    SCHOOL_NATURE,
    SCHOOL_SHADOW,
    SCHOOL_SPELLFIRE,
    SCHOOL_HOLY,
};

enum Trinket : int
{
    TRINKET_NONE = 0,
    TRINKET_RESTRAINED_ESSENCE = 23046,
    TRINKET_WARMTH_OF_FORGIVENESS = 23027,
    TRINKET_MQG = 19339,
    TRINKET_ZHC = 19950,
    TRINKET_HAZZARAH = 19959,
    TRINKET_BLUE_DRAGON = 19288,
    TRINKET_ALCHEMIST_STONE = 13503,
    TRINKET_EYE_OF_MOAM = 21473,
    TRINKET_NAT_PAGLE = 19947,
    TRINKET_DRACONIC_EMBLEM = 22268,
    TRINKET_BURST_OF_KNOWLEDGE = 11832,
    TRINKET_FIRE_RUBY = 20036,
    TRINKET_RECOMBO = 4381,
};

enum Potion : int
{
    POTION_NONE = 0,
    POTION_LESSER_MANA = 3385,
    POTION_MANA = 3327,
    POTION_GREATER_MANA = 6149,
    POTION_SUPERIOR_MANA = 13443,
    POTION_MAJOR_MANA = 13444,
};

enum EventType : int
{
    EVENT_NONE,
    EVENT_CAST_START,
    EVENT_CAST_FINISH,
    EVENT_SPELL_IMPACT,
    EVENT_SPELL_TICK,
    EVENT_MANA_REGEN,
    EVENT_MANA_GAIN,
    EVENT_BUFF_GAIN,
    EVENT_BUFF_EXPIRE,
    EVENT_BUFF_GAIN_ALL,
    EVENT_DEBUFF_GAIN,
    EVENT_DEBUFF_EXPIRE,
    EVENT_DOT,
    EVENT_CD_GAIN,
    EVENT_CD_EXPIRE,
    EVENT_UNIT,
    EVENT_UNIT_DESPAWN,
    EVENT_WAIT,
    EVENT_INTERRUPTION,
    EVENT_INTERRUPTION_END,
};

struct SpellStats
{
    std::string source;
    std::string name;
    double casts = 0;
    double misses = 0;
    double hits = 0;
    double crits = 0;
    double dmg = 0;
    double min_dmg = 0;
    double max_dmg = 0;
};

struct SimulationResult
{
    int dmg;
    int heal;
    double dps;
    double hps;
    double t;
    double t_gcd_capped;
    double t_oom;
    std::string log;
    std::string spells;
};

struct SimulationsResult
{
    double min_dps = 0;
    double max_dps = 0;
    double avg_dps = 0;
    double min_hps = 0;
    double max_hps = 0;
    double avg_hps = 0;
    double t_gcd_capped = 0;
    int n_oom = 0;
    int iterations;
    std::string histogram;
    std::string all_results;
    std::string spells;
};

bool isSchool(School var, School cmp);
bool isSchool(School var, School cmp1, School cmp2);

// Global rng generator
extern thread_local std::mt19937 g_rng;

template <typename Numeric>
Numeric random(Numeric from, Numeric to)
{
    using dist_type = typename std::conditional<
        std::is_integral<Numeric>::value, std::uniform_int_distribution<Numeric>, std::uniform_real_distribution<Numeric>>::type;

    thread_local static dist_type dist;

    return dist(g_rng, typename dist_type::param_type{from, to});
}

void setRNGSeed(int seed);
