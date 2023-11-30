#pragma once

#include "common.h"
#include "timing.h"
#include "interruption.h"

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
    int target_resistance = 0;
    int target_level = 28;
    double target_hp = 100;
    int distance = 20;
    int reaction_time = 300;

    bool demonic_pact = false;
    double demonic_pact_bonus = 0;
    bool ashenvale_cry = false;
    bool dmf_dmg = false;

    bool pre_cast = false;

    Trinket trinket1 = TRINKET_NONE;
    Trinket trinket2 = TRINKET_NONE;

    // Rotation
    Rotation rotation = ROTATION_ST_FIRE;
    bool maintain_imp_scorch = false;
    int scorching_mages = 0;
    bool rot_fire_blast_weave = false;
    bool rot_ice_lance = false;

    std::vector<Timing> timings;
    std::vector<Interruption> interruptions;
};
