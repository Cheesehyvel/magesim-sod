#pragma once

#include "common.h"
#include "timing.h"
#include "interruption.h"
#include "apl.h"

struct Config
{
    int duration = 180;
    double duration_variance = 15;
    int rng_seed = 0;
    bool avg_spell_dmg = false;
    bool additional_data = false;
    bool only_main_dmg = false;
    int targets = 1;
    int dot_targets = 1;
    double target_resistance = 0;
    int target_level = 28;
    double target_hp = 100;
    int player_level = 25;
    int distance = 20;
    int reaction_time = 300;

     // Buffs
    bool mage_armor = false;
    bool mana_spring = false;
    bool imp_mana_spring = false;
    bool demonic_pact = false;
    double demonic_pact_bonus = 0;

    // Debuffs
    bool curse_of_elements = false;
    bool curse_of_shadow = false;
    bool judgement_of_wisdom = false;
    bool mekkatorques_arcano_shredder = false;

    // World buffs
    bool ashenvale_cry = false;
    bool dmf_dmg = false;

    // Enchants / procs / sets
    bool set_udc_3p = false;
    bool set_t3_2p = false;
    bool set_t2_8p = false;
    bool set_aq40_5p = false;
    bool set_zg_5p = false;
    bool set_hyperconductive_wizard_3p = false;
    bool enchant_dismantle = false;
    bool item_gneuro_linked_monocle = false;
    bool item_hyperconductive_goldwrap = false;
    bool item_electromagnetic_hyperflux_reactivator = false;
    bool item_robe_archmage = false;
    bool item_celestial_orb = false;
    bool item_fractured_mind_pauldrons = false;
    bool item_shoulderpads_deranged = false;
    bool item_mantle_insanity = false;

    bool pre_cast = false;

    // Consumes
    Potion potion = POTION_NONE;
    bool atalai_mojo_forbidden_magic = false;

    Trinket trinket1 = TRINKET_NONE;
    Trinket trinket2 = TRINKET_NONE;

    // Rotation
    Rotation rotation = ROTATION_ST_FIRE;
    bool maintain_imp_scorch = false;
    int scorching_mages = 0;
    int winters_chill_mages = 0;
    bool rot_fire_blast_weave = false;
    bool rot_combustion_fb = false;
    bool rot_ice_lance = false;
    int rot_ab_stacks = 3;
    double rot_ab_spam_above = 100;
    double rot_ab_stacks_dec_below = 0;
    double rot_mb_mana = 0;

    APL::APL apl;

    std::vector<Timing> timings;
    std::vector<Interruption> interruptions;
};
