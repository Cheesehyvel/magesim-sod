#include "config.h"
#include "player.h"
#include "simulation.h"
#include "common.h"
#include "talents.h"

#include <emscripten.h>
#include <emscripten/bind.h>

#include <memory>

void addTiming(std::shared_ptr<Config> config, std::string name, double t, int wait_for_buff = 0, int wait_t = 0)
{
    Timing timing;
    timing.name = name;
    timing.t = t;
    timing.wait_for_buff = wait_for_buff;
    timing.wait_t = wait_t;
    config->timings.push_back(timing);
}

void addInterruption(std::shared_ptr<Config> config, bool silence, bool affects_all, double t, double duration)
{
    Interruption interruption;
    interruption.silence = silence;
    interruption.affects_all = affects_all;
    interruption.t = t;
    interruption.duration = duration;
    config->interruptions.push_back(interruption);
}

std::shared_ptr<Config> allocConfig()
{
    return std::make_shared<Config>();
}

Stats emptyStats()
{
    return {};
}

Talents emptyTalents()
{
    return {};
}

Runes emptyRunes()
{
    return {};
}

std::shared_ptr<unit::Player> allocPlayer(std::shared_ptr<Config> config, Stats stats, Talents talents, Runes runes)
{
    return std::make_shared<unit::Player>(*config, stats, talents, runes);
}

SimulationResult runSimulation(std::shared_ptr<Config> config, std::shared_ptr<unit::Player> player)
{
    auto sim = std::make_shared<Simulation>(*config, player);
    sim->logging = true;

    return sim->run(true);
}

SimulationsResult runSimulations(std::shared_ptr<Config> config, std::shared_ptr<unit::Player> player, int iterations)
{
    return std::make_shared<Simulation>(*config, player)->runMultiple(iterations);
}

EMSCRIPTEN_BINDINGS(my_module)
{

    emscripten::register_vector<double>("VectorDouble");

    emscripten::enum_<Rotation>("Rotation")
        .value("ROTATION_ST_ARCANE", ROTATION_ST_ARCANE)
        .value("ROTATION_ST_FIRE", ROTATION_ST_FIRE)
        .value("ROTATION_ST_FIRE_SC", ROTATION_ST_FIRE_SC)
        .value("ROTATION_ST_FROST", ROTATION_ST_FROST)
        .value("ROTATION_AOE_AE", ROTATION_AOE_AE)
        .value("ROTATION_AOE_AE_FS", ROTATION_AOE_AE_FS)
        .value("ROTATION_AOE_BLIZZ", ROTATION_AOE_BLIZZ)
        .value("ROTATION_AOE_BLIZZ_FS", ROTATION_AOE_BLIZZ_FS)
        .value("ROTATION_AOE_FIRE", ROTATION_AOE_FIRE)
        .value("ROTATION_AOE_FS", ROTATION_AOE_FS);

    emscripten::enum_<Race>("Race")
        .value("RACE_GNOME", RACE_GNOME)
        .value("RACE_HUMAN", RACE_HUMAN)
        .value("RACE_TROLL", RACE_TROLL)
        .value("RACE_UNDEAD", RACE_UNDEAD);

    emscripten::enum_<Trinket>("Trinket")
        .value("TRINKET_NONE", TRINKET_NONE)
        .value("TRINKET_RESTRAINED_ESSENCE", TRINKET_RESTRAINED_ESSENCE)
        .value("TRINKET_WARMTH_OF_FORGIVENESS", TRINKET_WARMTH_OF_FORGIVENESS)
        .value("TRINKET_MQG", TRINKET_MQG)
        .value("TRINKET_ZHC", TRINKET_ZHC)
        .value("TRINKET_HAZZARAH", TRINKET_HAZZARAH)
        .value("TRINKET_BLUE_DRAGON", TRINKET_BLUE_DRAGON)
        .value("TRINKET_ALCHEMIST_STONE", TRINKET_ALCHEMIST_STONE)
        .value("TRINKET_EYE_OF_MOAM", TRINKET_EYE_OF_MOAM)
        .value("TRINKET_NAT_PAGLE", TRINKET_NAT_PAGLE)
        .value("TRINKET_DRACONIC_EMBLEM", TRINKET_DRACONIC_EMBLEM)
        .value("TRINKET_BURST_OF_KNOWLEDGE", TRINKET_BURST_OF_KNOWLEDGE)
        .value("TRINKET_FIRE_RUBY", TRINKET_FIRE_RUBY)
        .value("TRINKET_RECOMBO", TRINKET_RECOMBO)
        ;

    emscripten::enum_<Potion>("Potion")
        .value("POTION_NONE", POTION_NONE)
        .value("POTION_LESSER_MANA", POTION_LESSER_MANA)
        .value("POTION_MANA", POTION_MANA)
        .value("POTION_GREATER_MANA", POTION_GREATER_MANA)
        .value("POTION_SUPERIOR_MANA", POTION_SUPERIOR_MANA)
        .value("POTION_MAJOR_MANA", POTION_MAJOR_MANA)
        .value("POTION_MILDY_IRRATIATED", POTION_MILDY_IRRATIATED)
        ;

    emscripten::enum_<LogType>("LogType")
        .value("LOG_NONE", LOG_NONE)
        .value("LOG_CAST_START", LOG_CAST_START)
        .value("LOG_CAST_SUCCESS", LOG_CAST_SUCCESS)
        .value("LOG_SPELL_IMPACT", LOG_SPELL_IMPACT)
        .value("LOG_MANA", LOG_MANA)
        .value("LOG_BUFF", LOG_BUFF)
        .value("LOG_DOT", LOG_DOT)
        .value("LOG_DEBUG", LOG_DEBUG)
        .value("LOG_WAIT", LOG_WAIT)
        .value("LOG_UNIT", LOG_UNIT);

    emscripten::class_<Config>("Config")
        .smart_ptr<std::shared_ptr<Config>>("Config")
        .property("duration", &Config::duration)
        .property("duration_variance", &Config::duration_variance)
        .property("rng_seed", &Config::rng_seed)
        .property("avg_spell_dmg", &Config::avg_spell_dmg)
        .property("additional_data", &Config::additional_data)
        .property("only_main_dmg", &Config::only_main_dmg)
        .property("targets", &Config::targets)
        .property("dot_targets", &Config::dot_targets)
        .property("target_resistance", &Config::target_resistance)
        .property("target_level", &Config::target_level)
        .property("target_hp", &Config::target_hp)
        .property("player_level", &Config::player_level)
        .property("distance", &Config::distance)
        .property("reaction_time", &Config::reaction_time)
        
        .property("mage_armor", &Config::mage_armor)
        .property("mana_spring", &Config::mana_spring)
        .property("imp_mana_spring", &Config::imp_mana_spring)
        .property("demonic_pact", &Config::demonic_pact)
        .property("demonic_pact_bonus", &Config::demonic_pact_bonus)

        .property("curse_of_elements", &Config::curse_of_elements)
        .property("curse_of_shadow", &Config::curse_of_shadow)
        .property("judgement_of_wisdom", &Config::judgement_of_wisdom)

        .property("ashenvale_cry", &Config::ashenvale_cry)
        .property("dmf_dmg", &Config::dmf_dmg)
        
        .property("set_udc_3p", &Config::set_udc_3p)
        .property("set_t3_2p", &Config::set_t3_2p)
        .property("set_t2_8p", &Config::set_t2_8p)
        .property("set_aq40_5p", &Config::set_aq40_5p)
        .property("set_zg_5p", &Config::set_zg_5p)
        .property("set_hyperconductive_wizard_3p", &Config::set_hyperconductive_wizard_3p)
        .property("item_gneuro_linked_monocle", &Config::item_gneuro_linked_monocle)
        .property("item_hyperconductive_goldwrap", &Config::item_hyperconductive_goldwrap)
        .property("item_robe_archmage", &Config::item_robe_archmage)
        .property("item_celestial_orb", &Config::item_celestial_orb)

        .property("pre_cast", &Config::pre_cast)

        .property("potion", &Config::potion)

        .property("trinket1", &Config::trinket1)
        .property("trinket2", &Config::trinket2)

        .property("rotation", &Config::rotation)
        .property("maintain_imp_scorch", &Config::maintain_imp_scorch)
        .property("scorching_mages", &Config::scorching_mages)
        .property("rot_fire_blast_weave", &Config::rot_fire_blast_weave)
        .property("rot_ice_lance", &Config::rot_ice_lance)
        .property("rot_ab_stacks", &Config::rot_ab_stacks)
        .property("rot_ab_spam_above", &Config::rot_ab_spam_above)
        .property("rot_ab_stacks_dec_below", &Config::rot_ab_stacks_dec_below)
        .property("rot_mb_mana", &Config::rot_mb_mana)
        ;

    emscripten::function("allocConfig", &allocConfig);

    emscripten::function("addTiming", &addTiming);

    emscripten::function("addInterruption", &addInterruption);

    emscripten::value_object<Stats>("Stats")
        .field("intellect", &Stats::intellect)
        .field("spirit", &Stats::spirit)
        .field("mp5", &Stats::mp5)
        .field("crit", &Stats::crit)
        .field("hit", &Stats::hit)
        .field("sp", &Stats::sp)
        .field("sp_arcane", &Stats::sp_arcane)
        .field("sp_fire", &Stats::sp_fire)
        .field("sp_frost", &Stats::sp_frost)
        .field("spen", &Stats::spen)
        .field("mana", &Stats::mana)
        ;

    emscripten::value_object<Talents>("Talents")
        .field("arcane_subtlety", &Talents::arcane_subtlety)
        .field("arcane_focus", &Talents::arcane_focus)
        .field("clearcast", &Talents::clearcast)
        .field("imp_arcane_explosion", &Talents::imp_arcane_explosion)
        .field("arcane_meditation", &Talents::arcane_meditation)
        .field("presence_of_mind", &Talents::presence_of_mind)
        .field("arcane_mind", &Talents::arcane_mind)
        .field("arcane_instability", &Talents::arcane_instability)
        .field("arcane_power", &Talents::arcane_power)
        .field("imp_fireball", &Talents::imp_fireball)
        .field("ignite", &Talents::ignite)
        .field("imp_fire_blast", &Talents::imp_fire_blast)
        .field("incinerate", &Talents::incinerate)
        .field("imp_flamestrike", &Talents::imp_flamestrike)
        .field("pyroblast", &Talents::pyroblast)
        .field("imp_scorch", &Talents::imp_scorch)
        .field("master_of_elements", &Talents::master_of_elements)
        .field("critical_mass", &Talents::critical_mass)
        .field("blast_wave", &Talents::blast_wave)
        .field("fire_power", &Talents::fire_power)
        .field("combustion", &Talents::combustion)
        .field("imp_frostbolt", &Talents::imp_frostbolt)
        .field("elemental_precision", &Talents::elemental_precision)
        .field("ice_shards", &Talents::ice_shards)
        .field("piercing_ice", &Talents::piercing_ice)
        .field("cold_snap", &Talents::cold_snap)
        .field("frost_channeling", &Talents::frost_channeling)
        .field("shatter", &Talents::shatter)
        .field("imp_cone_of_cold", &Talents::imp_cone_of_cold)
        .field("winters_chill", &Talents::winters_chill)
        ;

    emscripten::value_object<Runes>("Runes")
        .field("burnout", &Runes::burnout)
        .field("enlightenment", &Runes::enlightenment)
        .field("fingers_of_frost", &Runes::fingers_of_frost)
        .field("regeneration", &Runes::regeneration)
        .field("arcane_blast", &Runes::arcane_blast)
        .field("ice_lance", &Runes::ice_lance)
        .field("living_bomb", &Runes::living_bomb)
        .field("rewind_time", &Runes::rewind_time)
        .field("arcane_surge", &Runes::arcane_surge)
        .field("icy_veins", &Runes::icy_veins)
        .field("living_flame", &Runes::living_flame)
        .field("mass_regeneration", &Runes::mass_regeneration)
        .field("hot_streak", &Runes::hot_streak)
        .field("missile_barrage", &Runes::missile_barrage)
        .field("frostfire_bolt", &Runes::frostfire_bolt)
        .field("spellfrost_bolt", &Runes::spellfrost_bolt)
        .field("brain_freeze", &Runes::brain_freeze)
        .field("spell_power", &Runes::spell_power)
        .field("chronostatic_preservation", &Runes::chronostatic_preservation)
        ;

    emscripten::class_<unit::Player>("Player")
        .smart_ptr<std::shared_ptr<unit::Player>>("Player")
        .property("race", &unit::Player::race);

    emscripten::function("allocPlayer", &allocPlayer);

    emscripten::value_object<SimulationResult>("SimulationResult")
        .field("dmg", &SimulationResult::dmg)
        .field("dps", &SimulationResult::dps)
        .field("heal", &SimulationResult::heal)
        .field("hps", &SimulationResult::hps)
        .field("t", &SimulationResult::t)
        .field("t_gcd_capped", &SimulationResult::t_gcd_capped)
        .field("t_oom", &SimulationResult::t_oom)
        .field("log", &SimulationResult::log)
        .field("spells", &SimulationResult::spells);

    emscripten::value_object<SimulationsResult>("SimulationsResult")
        .field("min_dps", &SimulationsResult::min_dps)
        .field("avg_dps", &SimulationsResult::avg_dps)
        .field("max_dps", &SimulationsResult::max_dps)
        .field("min_hps", &SimulationsResult::min_hps)
        .field("avg_hps", &SimulationsResult::avg_hps)
        .field("max_hps", &SimulationsResult::max_hps)
        .field("t_gcd_capped", &SimulationsResult::t_gcd_capped)
        .field("n_oom", &SimulationsResult::n_oom)
        .field("iterations", &SimulationsResult::iterations)
        .field("histogram", &SimulationsResult::histogram)
        .field("all_results", &SimulationsResult::all_results)
        .field("spells", &SimulationsResult::spells);

    emscripten::function("emptyStats", &emptyStats);
    emscripten::function("emptyTalents", &emptyTalents);
    emscripten::function("emptyRunes", &emptyRunes);
    emscripten::function("runSimulation", &runSimulation);
    emscripten::function("runSimulations", &runSimulations);
}
