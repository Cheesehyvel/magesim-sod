#include "player.h"
#include "config.h"
#include "unit.h"
#include "talents.h"
#include "state.h"

#include <algorithm>
#include <string>
#include <exception>

namespace unit
{

Player::Player(const Config& _config, const Stats& _stats, const Talents& _talents, const Runes& _runes)
    : Unit(_config, _stats), talents(_talents), runes(_runes)
{
    if (config.player_level == 25)
        base_mana = 481;
    else if (config.player_level == 40)
        base_mana = 853;
    else if (config.player_level == 50)
        base_mana = 1048;
    else if (config.player_level == 60)
        base_mana = 1213;
    
    name = "Player";
    id = 1;
    crit_suppression = true;
    used_timings.resize(_config.timings.size());
}

void Player::reset()
{
    Unit::reset();
    combustion = 0;
    fingers_of_frost = 0;
    heating_up = false;
    hotstreak_crits = 0;
    hotstreak_hits = 0;
    t_flamestrike = -20;
    t_flamestrike_dr = -20;
    t_scorch = -60;
    t_mana_spent = 0;
    t_living_bomb = -20;
    ab_streak = 0;
    has_pre_cast = false;

    mana_ruby = config.player_level >= 58;
    mana_citrine = config.player_level >= 48;
    mana_jade = config.player_level >= 38;
    mana_agate = config.player_level >= 28;

    std::fill(used_timings.begin(), used_timings.end(), false);
}

Faction Player::faction() const
{
    if (race == RACE_GNOME || race == RACE_HUMAN)
        return FACTION_ALLIANCE;
    return FACTION_HORDE;
}

bool Player::hasTrinket(Trinket trinket) const
{
    return config.trinket1 == trinket || config.trinket2 == trinket;
}

bool Player::hasTrinkets(Trinket trinket1, Trinket trinket2) const
{
    return hasTrinket(trinket1) || hasTrinket(trinket2);
}

void Player::applyMana(const State& state, double _mana)
{
    Unit::applyMana(state, _mana);

    if (_mana < 0) 
        t_mana_spent = state.t;
}

double Player::manaPerSecond(const State& state) const
{
    double mps = staticManaPerSecond();
    double spi = spiritManaPerSecond();

    double while_casting = 0;


    if (config.mage_armor && config.player_level >= 34)
        while_casting+= 0.3;
    if (talents.arcane_meditation)
        while_casting+= 0.05 * talents.arcane_meditation;
    if (runes.enlightenment && manaPercent() <= 30.0)
        while_casting+= 0.1;

    if (t_mana_spent + 5 <= state.t)
        while_casting = 1;

    if (hasBuff(buff::INNERVATE)) {
        spi*= 5.0;
        while_casting = 1;
    }
    if (hasBuff(buff::EVOCATION)) {
        spi*= 16.0;
        while_casting = 1;
    }
    if (hasBuff(buff::ARCANE_SURGE)) {
        spi*= 4.0;
        while_casting = 1;
    }
    if (hasBuff(buff::BLUE_DRAGON)) {
        while_casting = 1;
    }

    mps += std::min(1.0, while_casting) * spi;

    return mps;
}

double Player::maxMana() const
{
    double mana = Unit::maxMana();

    if (talents.arcane_mind)
        mana *= 1.0 + (talents.arcane_mind * 0.02);

    return mana;
}

double Player::baseCastTime(std::shared_ptr<spell::Spell> spell) const
{
    double t = Unit::baseCastTime(spell);

    if (spell->id == spell::FROSTBOLT && talents.imp_frostbolt)
        t -= talents.imp_frostbolt * 0.1;
    if (spell->id == spell::FIREBALL && talents.imp_fireball)
        t -= talents.imp_fireball * 0.1;
    if ((spell->id == spell::FLAMESTRIKE || spell->id == spell::FLAMESTRIKE_DR) && config.set_zg_5p)
        t -= 0.5;

    if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE))
        t -= 2.5;

    if (spell->id == spell::PYROBLAST && hasBuff(buff::HOT_STREAK))
        t = 0;
    else if ((spell->id == spell::FIREBALL || spell->id == spell::FROSTFIRE_BOLT) && hasBuff(buff::BRAIN_FREEZE))
        t = 0;
    else if (hasBuff(buff::PRESENCE_OF_MIND) && !spell->channeling)
        t = 0;
    else if (hasBuff(buff::NETHERWIND_FOCUS) && !spell->channeling)
        t = 0;

    return t;
}

double Player::castHaste() const
{
    double haste = 1.0 / Unit::castHaste();

    if (hasBuff(buff::MQG))
        haste *= 1.33;
    if (hasBuff(buff::ICY_VEINS))
        haste *= 1.2;
    if (hasBuff(buff::BERSERKING))
        haste *= 1.1;

    return 1.0 / haste;
}

double Player::hitChance(std::shared_ptr<spell::Spell> spell, double dlevel) const
{
    double hit = Unit::hitChance(spell, dlevel);

    if (spell->isSchool(SCHOOL_ARCANE) && talents.arcane_focus)
        hit += talents.arcane_focus * 2.0;
    if (spell->isSchool(SCHOOL_FIRE, SCHOOL_FROST) && talents.elemental_precision)
        hit += talents.elemental_precision * 2.0;

    return hit;
}

double Player::critChance(std::shared_ptr<spell::Spell> spell) const
{
    double crit = Unit::critChance(spell);

    if (spell->proc)
        return crit;

    if (talents.incinerate && (spell->id == spell::FIRE_BLAST || spell->id == spell::SCORCH))
        crit += talents.incinerate * 2.0;

    if (talents.imp_flamestrike && (spell->id == spell::FLAMESTRIKE || spell->id == spell::FLAMESTRIKE_DR))
        crit += talents.imp_flamestrike * 5.0;

    if (talents.imp_arcane_explosion && spell->id == spell::ARCANE_EXPLOSION)
        crit += talents.imp_arcane_explosion * 2.0;

    if (talents.critical_mass && spell->isSchool(SCHOOL_FIRE))
        crit += talents.critical_mass * 2.0;

    if (hasBuff(buff::COMBUSTION) && spell->isSchool(SCHOOL_FIRE))
        crit += buffStacks(buff::COMBUSTION) * 10.0;

    if (talents.shatter && isFrozen())
        crit += talents.shatter * 10.0;

    if (hasBuff(buff::ARCANE_POTENCY) && spell->isSchool(SCHOOL_ARCANE))
        crit += 5;

    return crit;
}

double Player::critMultiplierMod(std::shared_ptr<spell::Spell> spell) const
{
    double multi = Unit::critMultiplierMod(spell);

    if (spell->proc)
        return multi;

    if (spell->isSchool(SCHOOL_FROST) && talents.ice_shards)
        multi += talents.ice_shards * 0.2;

    if (hasBuff(buff::ARCANE_POTENCY) && spell->isSchool(SCHOOL_ARCANE))
        multi += 0.5;

    if (runes.spell_power)
        multi += 0.5;

    return multi;
}

double Player::buffHealMultiplier(std::shared_ptr<spell::Spell> spell, const State& state) const
{
    double multi = Unit::buffHealMultiplier(spell, state);
    
    if (config.ashenvale_cry)
        multi*= 1.05;

    return multi;
}

double Player::buffDmgMultiplier(std::shared_ptr<spell::Spell> spell, const State& state) const
{
    double multi = Unit::buffDmgMultiplier(spell, state);
    double additive = 1;

    if (config.dmf_dmg)
        multi*= 1.1;
    if (config.ashenvale_cry)
        multi*= 1.05;
    if (config.set_udc_3p)
        multi*= 1.02;

    if (spell->proc)
        return multi;

    if ((spell->school == SCHOOL_FIRE || spell->school == SCHOOL_FROST) && hasBuff(buff::TANGLED_CAUSALITY))
        multi*= 0.5;

    if (runes.enlightenment && manaPercent() >= 70.0)
        multi *= 1.1;

    if (talents.piercing_ice && spell->isSchool(SCHOOL_FROST))
        multi *= 1 + talents.piercing_ice * 0.02;
    if (spell->id == spell::CONE_OF_COLD && talents.imp_cone_of_cold)
        multi *= 1.05 + talents.imp_cone_of_cold * 0.1;

    if (spell->isSchool(SCHOOL_ARCANE) && spell->id != spell::ARCANE_BLAST && hasBuff(buff::ARCANE_BLAST, true)) {
        double ab = 0.15;
        multi *= 1 + ab * buffStacks(buff::ARCANE_BLAST, true);
    }

    if (spell->id == spell::ARCANE_SURGE)
        multi *= 1.0 + (manaPercent()/100.0 * 3.0);

    if (spell->id == spell::ICE_LANCE && isFrozen())
        multi *= 3;

    // Additive category
    additive = 1;

    if (talents.fire_power && spell->isSchool(SCHOOL_FIRE))
        additive += talents.fire_power * 0.02;

    if (hasBuff(buff::ARCANE_POWER))
        additive += 0.3;
    else if (hasBuff(buff::POWER_INFUSION))
        additive += 0.2;

    multi *= additive;

    return multi;
}

double Player::baseManaCost(std::shared_ptr<spell::Spell> spell) const
{
    double cost = Unit::baseManaCost(spell);

    if (hasBuff(buff::BURST_OF_KNOWLEDGE))
        cost = std::max(cost - 100, 0.0);

    return cost;
}

double Player::manaCostMultiplier(std::shared_ptr<spell::Spell> spell) const
{
    double multi = Unit::manaCostMultiplier(spell);

    if (hasBuff(buff::CLEARCAST) && spell->id != spell::ARCANE_SURGE)
        return 0;

    if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE))
        return 0;

    if (talents.frost_channeling && spell->isSchool(SCHOOL_FROST))
        multi*= 0.05 * talents.frost_channeling;

    if (hasBuff(buff::CHARGED_INSPIRATION))
        multi*= 0.5;

    return multi;
}

double Player::manaCostMod(std::shared_ptr<spell::Spell> spell, double mana_cost) const
{
    if (spell->id == spell::ARCANE_SURGE) {
        return mana - mana_cost;
    }

    double mod = Unit::manaCostMod(spell, mana_cost);

    if (hasBuff(buff::ARCANE_POWER))
        mod+= 0.3 * mana_cost;

    if (spell->id == spell::ARCANE_BLAST) {
        double multi = 1.75 * buffStacks(buff::ARCANE_BLAST);
        mod+= ceil(mana_cost * multi - 0.5);
    }

    return mod;
}

double Player::getSpellPenetration(School school) const
{
    double pen = Unit::getSpellPenetration(school);

    if (isSchool(school, SCHOOL_ARCANE) && talents.arcane_subtlety)
        pen+= 5.0 * talents.arcane_subtlety;

    return pen;
}

double Player::cooldownMod(const cooldown::Cooldown &cooldown) const
{
    double mod = Unit::cooldownMod(cooldown);

    if (config.set_t3_2p && cooldown.id == cooldown::EVOCATION)
        mod-= 60;

    return mod;
}

bool Player::shouldConsumeClearcast(std::shared_ptr<spell::Spell> spell) const
{
    if (spell->is_trigger || spell->tick)
        return false;

    if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE))
        return false;

    return true;
}

bool Player::isFrozen() const
{
    return hasBuff(buff::FINGERS_OF_FROST) || hasBuff(buff::GHOST_FINGERS);
}

bool Player::hasChillEffect(std::shared_ptr<spell::Spell> spell) const
{
    if (spell->id == spell::CONE_OF_COLD || spell->id == spell::FROSTBOLT || spell->id == spell::FROSTFIRE_BOLT)
        return true;

    return false;
}

std::vector<action::Action> Player::onBuffGain(const State& state, std::shared_ptr<buff::Buff> buff)
{
    auto actions = Unit::onBuffGain(state, buff);

    if (buff->id == buff::ARCANE_BLAST)
        ab_streak++;

    return actions;
}

std::vector<action::Action> Player::onBuffExpire(const State& state, std::shared_ptr<buff::Buff> buff)
{
    std::vector<action::Action> actions = Unit::onBuffExpire(state, buff);

    if (buff->id == buff::ARCANE_BLAST)
        ab_streak = 0;
    if (buff->id == buff::BRAIN_FREEZE)
        actions.push_back(cooldownAction<cooldown::BrainFreeze>());

    return actions;
}

std::vector<action::Action> Player::onCastStartProc(const State& state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    auto actions = Unit::onCastStartProc(state, spell, target);

    return actions;
}

std::vector<action::Action> Player::onCastSuccessProc(const State& state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    auto actions = Unit::onCastSuccessProc(state, spell, target);

    if (spell->id == spell::MANA_GEM)
        return useManaGem();

    bool is_harmful = spell->max_dmg > 0;

    // Cooldowns
    if (spell->id == spell::FIRE_BLAST)
        actions.push_back(cooldownAction<cooldown::FireBlast>(talents.imp_fire_blast));
    if (spell->id == spell::BLAST_WAVE)
        actions.push_back(cooldownAction<cooldown::BlastWave>());
    if (spell->id == spell::CONE_OF_COLD)
        actions.push_back(cooldownAction<cooldown::ConeOfCold>());
    if (spell->id == spell::LIVING_FLAME)
        actions.push_back(cooldownAction<cooldown::LivingFlame>());
    if (spell->id == spell::EVOCATION) {
        actions.push_back(cooldownAction<cooldown::Evocation>());
        actions.push_back(buffAction<buff::Evocation>());
    }
    if (spell->id == spell::COLD_SNAP) {
        actions.push_back(cooldownAction<cooldown::ColdSnap>());
        actions.push_back(cooldownExpireAction<cooldown::IcyVeins>());
        actions.push_back(cooldownExpireAction<cooldown::ConeOfCold>());
    }
    if (spell->id == spell::ARCANE_SURGE) {
        actions.push_back(buffAction<buff::ArcaneSurge>());
        actions.push_back(cooldownAction<cooldown::ArcaneSurge>());
    }

    if (config.set_t2_8p && (spell->id == spell::ARCANE_MISSILES || spell->id == spell::FIREBALL || spell->id == spell::FROSTBOLT))
        actions.push_back(buffAction<buff::NetherwindFocus>());

    if (spell->id == spell::LIVING_BOMB)
        target->t_living_bomb = state.t;
    if (spell->id == spell::FLAMESTRIKE) {
        t_flamestrike = state.t;
        actions.push_back(spellAction<spell::FlamestrikeDot>());
    }
    if (spell->id == spell::FLAMESTRIKE_DR) {
        t_flamestrike_dr = state.t;
        actions.push_back(spellAction<spell::FlamestrikeDRDot>());
    }

    if (hasBuff(buff::GHOST_FINGERS))
        actions.push_back(buffExpireAction<buff::GhostFingers>());

    if (hasBuff(buff::PRESENCE_OF_MIND) && spell->cast_time && !spell->channeling)
        actions.push_back(buffExpireAction<buff::PresenceOfMind>());
    else if (hasBuff(buff::NETHERWIND_FOCUS) && spell->cast_time && !spell->channeling)
        actions.push_back(buffExpireAction<buff::NetherwindFocus>());

    if (hasBuff(buff::CLEARCAST) && shouldConsumeClearcast(spell))
        actions.push_back(buffExpireAction<buff::Clearcast>());

    if (spell->id == spell::ARCANE_BLAST)
        actions.push_back(buffAction<buff::ArcaneBlast>());
    else if (hasBuff(buff::ARCANE_BLAST) && spell->isSchool(SCHOOL_ARCANE) && spell->min_dmg > 0)
        actions.push_back(buffExpireAction<buff::ArcaneBlast>());

    if (spell->id == spell::ARCANE_MISSILES && hasBuff(buff::MISSILE_BARRAGE))
        actions.push_back(buffExpireAction<buff::MissileBarrage>());
    if (spell->id == spell::PYROBLAST && hasBuff(buff::HOT_STREAK))
        actions.push_back(buffExpireAction<buff::HotStreak>());

    if (hasBuff(buff::BRAIN_FREEZE) && spell->actual_cast_time == 0 && (spell->id == spell::FROSTFIRE_BOLT || spell->id == spell::FIREBALL))
        actions.push_back(buffExpireAction<buff::BrainFreeze>());

    if (runes.missile_barrage) {
        if (spell->id == spell::ARCANE_BLAST || spell->id == spell::FIREBALL || spell->id == spell::FROSTBOLT) {
            int chance = 20;
            if (spell->id == spell::ARCANE_BLAST)
                chance *= 2;

            if (random<int>(0, 99) < chance)
                actions.push_back(buffAction<buff::MissileBarrage>());
        }
    }

    if (runes.fingers_of_frost && hasBuff(buff::FINGERS_OF_FROST) && is_harmful) {
        fingers_of_frost--;
        if (fingers_of_frost <= 0) {
            actions.push_back(buffExpireAction<buff::FingersOfFrost>());
            actions.push_back(buffAction<buff::GhostFingers>());
        }
    }

    if (spell->can_proc) {
        for (auto& i : onCastOrTick(state, spell, target))
            actions.push_back(std::move(i));

        if (hasTrinket(TRINKET_BLUE_DRAGON) && random<int>(0, 49) == 0)
            actions.push_back(buffAction<buff::BlueDragon>());

        if (config.set_hyperconductive_wizard_3p && random<int>(0, 9) == 0)
            actions.push_back(buffAction<buff::EnergizedHyperconductor>());
    }

    if (is_harmful && hasBuff(buff::UNSTABLE_POWER)) {
        actions.push_back(buffAction<buff::UnstablePower>());
    }

    if (is_harmful && hasBuff(buff::CHAOS_FIRE) && spell->isSchool(SCHOOL_FIRE)) {
        actions.push_back(buffExpireAction<buff::ChaosFire>());
    }

    return actions;
}

std::vector<action::Action> Player::onProcSuccess(const State& state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    auto actions = Unit::onProcSuccess(state, spell, target);

    return actions;
}

std::vector<action::Action> Player::onSpellImpactProc(const State& state, const spell::SpellInstance &instance, std::shared_ptr<target::Target> target)
{
    std::vector<action::Action> actions = Unit::onSpellImpactProc(state, instance, target);

    if (instance.spell->id == spell::LIVING_BOMB && instance.tick == instance.spell->ticks) {
        actions.push_back(spellAction<spell::LivingBombExplosion>());
    }

    if (instance.result == spell::MISS && config.set_aq40_5p)
        actions.push_back(buffAction<buff::EnigmasAnswer>());

    if (instance.result != spell::MISS) {
        if (talents.imp_scorch && instance.spell->id == spell::SCORCH && (talents.imp_scorch == 3 || random<int>(0, 2) < talents.imp_scorch)) {
            t_scorch = state.t;
            actions.push_back(debuffAction<debuff::ImprovedScorch>(target));
        }

        if (instance.spell->id == spell::FIREBALL)
            actions.push_back(spellAction<spell::FireballDot>(target));
        if (instance.spell->id == spell::PYROBLAST)
            actions.push_back(spellAction<spell::PyroblastDot>(target));
        if (instance.spell->id == spell::FROSTFIRE_BOLT)
            actions.push_back(spellAction<spell::FrostfireBoltDot>(target));

        if (instance.spell->dot) {
            // Dot trinkets, if any
        }
        else {
            if (talents.clearcast) {
                double chance = talents.clearcast * 2.0;
                // Less chance per tick for channelled spells
                if (instance.spell->ticks)
                    chance /= instance.spell->ticks;
                if (random<double>(0, 100) < chance)
                    actions.push_back(buffAction<buff::Clearcast>());
            }

            if (config.judgement_of_wisdom && config.player_level >= 38 && faction() == FACTION_ALLIANCE) {
                double chance = 50;
                // Less chance per tick for channelled spells
                if (instance.spell->channeling)
                    chance/= (double) instance.spell->ticks;
                if (random<double>(0, 100) < chance) {
                    double m = 33.0;
                    if (config.player_level >= 58)
                        m = 59.0;
                    else if (config.player_level >= 48)
                        m = 46.0;
                    actions.push_back(manaAction(m, "Judgement of Wisdom"));
                }
            }

            if (hasBuff(buff::COMBUSTION) && instance.spell->isSchool(SCHOOL_FIRE)) {
                if (instance.result == spell::CRIT)
                    combustion++;
                if (combustion == 3) {
                    actions.push_back(buffExpireAction<buff::Combustion>());
                    actions.push_back(cooldownAction<cooldown::Combustion>());
                }
                else {
                    actions.push_back(buffAction<buff::Combustion>());
                }
            }
        }
    }

    if (instance.result == spell::CRIT) {
        // Ignite
        if (talents.ignite && instance.spell->isSchool(SCHOOL_FIRE) && !instance.spell->proc) {
            // 40% over 2 ticks = 20%
            actions.push_back(spellAction<spell::Ignite>(target, round(instance.dmg * 0.2)));
        }

        if (!instance.spell->dot) {
            if (talents.master_of_elements) {
                double mana = baseManaCost(instance.spell) * 0.1 * talents.master_of_elements;
                if ((instance.spell->channeling || instance.spell->dot) && instance.spell->ticks)
                    mana = mana / instance.spell->ticks;
                actions.push_back(manaAction(mana, "Master of Elements"));
            }

            if (runes.burnout && instance.spell->actual_cost)
                actions.push_back(manaAction(instance.spell->actual_cost * -0.01, "Burnout"));
        }
    }



    if (runes.hot_streak && !instance.spell->dot) {
        if (instance.spell->id == spell::FIREBALL ||
            instance.spell->id == spell::SCORCH ||
            instance.spell->id == spell::LIVING_BOMB_EXPLOSION ||
            instance.spell->id == spell::FROSTFIRE_BOLT ||
            instance.spell->id == spell::FIRE_BLAST)
        {
            if (instance.spell->aoe) {
                hotstreak_hits++;
                if (instance.result == spell::CRIT)
                    hotstreak_crits++;

                if (hotstreak_hits == config.targets) {
                    if (hotstreak_crits == 1 && !heating_up) {
                        heating_up = true;
                    }
                    else if (hotstreak_crits == 1 && heating_up || hotstreak_crits > 1) {
                        actions.push_back(buffAction<buff::HotStreak>());
                        heating_up = false;
                    }
                    else {
                        heating_up = false;
                    }

                    hotstreak_hits = hotstreak_crits = 0;
                }
            }
            else {
                if (instance.result == spell::CRIT) {
                    if (heating_up) {
                        actions.push_back(buffAction<buff::HotStreak>());
                        heating_up = false;
                    }
                    else {
                        heating_up = true;
                    }
                }
                else {
                    heating_up = false;
                }
            }
        }
    }

    if (runes.fingers_of_frost) {
        if (hasChillEffect(instance.spell)) {
            if (random<int>(0, 99) < 15) {
                fingers_of_frost = 2;
                actions.push_back(buffAction<buff::FingersOfFrost>());
            }
        }
    }

    if (runes.brain_freeze && hasChillEffect(instance.spell) && !hasCooldown(cooldown::BRAIN_FREEZE) && random<int>(0, 99) < 15)
        actions.push_back(buffAction<buff::BrainFreeze>());

    if (instance.dmg && instance.spell->isSchool(SCHOOL_ARCANE)) {
        double heal = (instance.dmg + instance.resist)* 0.8;
        if (instance.spell->id == spell::ARCANE_EXPLOSION)
            heal*= 0.2;
        if (hasBuff(buff::TEMPORAL_BEACON)) {
            actions.push_back(spellAction<spell::TemporalBeacon>(heal));
        }
        if (hasBuff(buff::TEMPORAL_BEACON_PARTY)) {
            for (int i=0; i<5; i++)
                actions.push_back(spellAction<spell::TemporalBeacon>(heal));
        }
    }

    return actions;
}

std::vector<action::Action> Player::onSpellTickProc(const State& state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, int tick)
{
    std::vector<action::Action> actions = Unit::onSpellTickProc(state, spell, target, tick);

    if (spell->can_proc) {
        for (auto &i : onCastOrTick(state, spell, target, tick))
            actions.push_back(std::move(i));
    }

    if (spell->id == spell::REGENERATION && tick == spell->ticks)
        actions.push_back(buffAction<buff::TemporalBeacon>());
    if (spell->id == spell::MASS_REGENERATION && tick == spell->ticks)
        actions.push_back(buffAction<buff::TemporalBeaconParty>());

    return actions;
}

std::vector<action::Action> Player::onCastOrTick(const State& state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, int tick)
{
    std::vector<action::Action> actions;

    // Harmful spells
    if (spell->max_dmg > 0) {
        //
    }

    return actions;
}

bool Player::hasManaGem() const
{
    if (mana_ruby || mana_citrine || mana_jade || mana_agate)
        return true;
    return false;
}

double Player::manaGemMax() const
{
    double max = 0;

    if (mana_ruby)
        max = 1200;
    else if (mana_citrine)
        max = 925;
    else if (mana_jade)
        max = 650;
    else if (mana_agate)
        max = 425;
    else
        return 0;

    return max;
}

bool Player::shouldUseManaGem(const State& state)
{
    if (!hasManaPotion() || hasCooldown(cooldown::MANA_GEM) || !hasManaGem())
        return false;

    // Check for planned mana gem timings
    auto timing = getNextTiming("mana_gem");
    if (timing && timing->t < state.t + 120)
        return false;

    double max = manaGemMax();

    // If tide is running, add a tick as buffer
    if (hasBuff(buff::MANA_TIDE))
        max += 170;

    return Unit::maxMana() - mana >= max;
}

bool Player::hasManaPotion() const
{
    if (config.potion == POTION_MILDY_IRRATIATED)
        return false;

    return config.potion != POTION_NONE;
}

double Player::manaPotionMax() const
{
    double max = 0;

    if (config.potion == POTION_LESSER_MANA)
        max = 360;
    else if (config.potion == POTION_MANA)
        max = 585;
    else if (config.potion == POTION_GREATER_MANA)
        max = 900;
    else if (config.potion == POTION_SUPERIOR_MANA)
        max = 1500;
    else if (config.potion == POTION_MAJOR_MANA)
        max = 2250;

    if (hasTrinket(TRINKET_ALCHEMIST_STONE))
        max *= 1.33;

    return max;
}

bool Player::shouldUseManaPotion(const State& state)
{
    if (hasCooldown(cooldown::POTION) || hasBuff(buff::INNERVATE) || !hasManaPotion())
        return false;

    // Check for planned potions timings
    auto timing = getNextTiming("potion");
    if (timing)
        return false;

    double max = manaPotionMax();

    // If tide is running, add a tick as buffer
    if (hasBuff(buff::MANA_TIDE))
        max += 170;

    // If gem is configured to be used within 10 sec, count with the mana gain to avoid overcapping
    if (!hasCooldown(cooldown::MANA_GEM) && hasManaGem()) {
        bool gem_soon = false;
        auto timing = getNextTiming("mana_gem");
        if (timing != NULL && timing->t - state.t < 10)
            max += manaGemMax();
    }

    return maxMana() - mana >= max;
}

bool Player::shouldEvocate(const State& state)
{
    if (hasCooldown(cooldown::EVOCATION))
        return false;

    // Check for planned evocation timings
    auto timing = getNextTiming("evocation");
    if (timing) {
        if (isTimingReady(timing, state)) {
            useTiming(timing);
            return true;
        }
        if (timing->t < state.t + 480)
            return false;
    }

    if (hasBuff(buff::INNERVATE) || hasBuff(buff::MANA_TIDE) || hasBuff(buff::ARCANE_SURGE))
        return false;

    if (manaPercent() > 10.0)
        return false;

    if (state.duration - state.t < 5)
        return false;

    return true;
}

std::vector<action::Action> Player::useManaGem()
{
    std::vector<action::Action> actions;

    double mana = 0;

    if (mana_ruby) {
        mana_ruby = false;
        mana = random<int>(1000, 1200);
    }
    else if (mana_citrine) {
        mana_citrine = false;
        mana = random<int>(775, 925);
    }
    else if (mana_jade) {
        mana_jade = false;
        mana = random<int>(550, 650);
    }
    else if (mana_agate) {
        mana_agate = false;
        mana = random<int>(375, 425);
    }

    actions.push_back(manaAction(mana, "Mana Gem"));
    actions.push_back(cooldownAction<cooldown::ManaGem>());

    return actions;
}

std::vector<action::Action> Player::usePotion()
{
    std::vector<action::Action> actions = Unit::usePotion();

    if (hasManaPotion()) {
        double mana = 0;

        if (config.potion == POTION_LESSER_MANA)
            mana = random<double>(280, 360);
        else if (config.potion == POTION_MANA)
            mana = random<double>(455, 585);
        else if (config.potion == POTION_GREATER_MANA)
            mana = random<double>(700, 900);
        else if (config.potion == POTION_SUPERIOR_MANA)
            mana = random<double>(900, 1500);
        else if (config.potion == POTION_MAJOR_MANA)
            mana = random<double>(1350, 2250);

        if (hasTrinket(TRINKET_ALCHEMIST_STONE))
            mana *= 1.33;

        mana = round(mana);

        actions.push_back(manaAction(mana, "Mana Potion"));
        actions.push_back(cooldownAction<cooldown::Potion>(120));
    }
    else if (config.potion == POTION_MILDY_IRRATIATED) {
        double mana = random<double>(262, 438);

        if (hasTrinket(TRINKET_ALCHEMIST_STONE))
            mana *= 1.33;

        mana = round(mana);

        actions.push_back(manaAction(mana, "Mana Potion"));
        actions.push_back(buffAction<buff::MildlyIrradiated>());
        actions.push_back(cooldownAction<cooldown::Potion>(120));
    }

    return actions;
}

bool Player::isTrinketOnSharedCD(Trinket trinket) const
{
    if (!trinketSharesCD(trinket))
        return false;
    return hasCooldown(cooldown::TRINKET_SHARED);
}

bool Player::trinketSharesCD(Trinket trinket) const
{
    if (trinket == TRINKET_BURST_OF_KNOWLEDGE)
        return false;
    return true;
}

bool Player::isUseTrinket(Trinket trinket) const
{
    if (trinket == TRINKET_RESTRAINED_ESSENCE)
        return true;
    if (trinket == TRINKET_WARMTH_OF_FORGIVENESS)
        return true;
    if (trinket == TRINKET_MQG)
        return true;
    if (trinket == TRINKET_ZHC)
        return true;
    if (trinket == TRINKET_HAZZARAH)
        return true;
    if (trinket == TRINKET_EYE_OF_MOAM)
        return true;
    if (trinket == TRINKET_NAT_PAGLE)
        return true;
    if (trinket == TRINKET_DRACONIC_EMBLEM)
        return true;
    if (trinket == TRINKET_BURST_OF_KNOWLEDGE)
        return true;
    if (trinket == TRINKET_FIRE_RUBY)
        return true;

    // Lets not use this trinket automatically
    // if (trinket == TRINKET_RECOMBO)
    //     return true;

    return false;
}

const Timing* Player::getNextTiming(const std::string& name) const
{
    for (int i = 0; i < config.timings.size(); ++i) {
        if (!used_timings[i] && config.timings[i].name == name)
            return &config.timings[i];
    }

    return nullptr;
}

void Player::useTiming(const Timing* timing)
{
    for (auto i = 0; i < config.timings.size(); ++i)
        if (&config.timings[i] == timing)
            used_timings[i] = true;
}

bool Player::isTimingReady(const Timing* timing, const State& state) const
{
    if (timing->t > state.t)
        return false;

    for (auto i = 0; i < config.timings.size(); ++i)
        if (&config.timings[i] == timing && used_timings[i])
            return false;

    if (timing->wait_for_buff != 0 && timing->wait_t + timing->t > state.t) {
        auto buff_id = static_cast<buff::ID>(timing->wait_for_buff);
        if (!canReactTo(buff_id, state.t))
            return false;
    }

    return true;
}

bool Player::useTimingIfPossible(const std::string& name, const State& state, bool expl)
{
    auto timing = getNextTiming(name);

    if (timing == NULL && !expl)
        return true;

    if (timing != NULL && isTimingReady(timing, state)) {
        useTiming(timing);
        return true;
    }

    return false;
}

bool Player::isTimingReadySoon(const std::string& name, const State& state, double t)
{
    auto timing = getNextTiming(name);

    if (timing != NULL && timing->t - state.t < t)
        return true;

    return false;
}

std::vector<action::Action> Player::useTrinket(Trinket trinket, std::shared_ptr<cooldown::Cooldown> cooldown)
{
    std::vector<action::Action> actions = Unit::useTrinket(trinket, cooldown);
    std::shared_ptr<buff::Buff> buff;

    cooldown->duration = 120;

    if (trinket == TRINKET_RESTRAINED_ESSENCE) {
        buff = std::make_shared<buff::EssenceOfSapphiron>();
    }
    else if (trinket == TRINKET_WARMTH_OF_FORGIVENESS) {
        actions.push_back(manaAction(500, "Warmth of Forgiveness"));
        cooldown->duration = 180;
    }
    else if (trinket == TRINKET_MQG) {
        buff = std::make_shared<buff::MindQuickening>();
        cooldown->duration = 300;
    }
    else if (trinket == TRINKET_ZHC) {
        buff = std::make_shared<buff::UnstablePower>();
    }
    else if (trinket == TRINKET_HAZZARAH) {
        buff = std::make_shared<buff::ArcanePotency>();
        cooldown->duration = 180;
    }
    else if (trinket == TRINKET_EYE_OF_MOAM) {
        buff = std::make_shared<buff::ObsidianInsight>();
        cooldown->duration = 180;
    }
    else if (trinket == TRINKET_NAT_PAGLE) {
        buff = std::make_shared<buff::NatPagle>();
        cooldown->duration = 75;
    }
    else if (trinket == TRINKET_DRACONIC_EMBLEM) {
        buff = std::make_shared<buff::ChromaticInfusion>();
        cooldown->duration = 75;
    }
    else if (trinket == TRINKET_BURST_OF_KNOWLEDGE) {
        buff = std::make_shared<buff::BurstOfKnowledge>();
        cooldown->duration = 900;
    }
    else if (trinket == TRINKET_FIRE_RUBY) {
        actions.push_back(manaAction((double) random<int>(1, 500), "Fire Ruby"));
        buff = std::make_shared<buff::ChaosFire>();
        cooldown->duration = 180;
    }
    else if (trinket == TRINKET_RECOMBO) {
        actions.push_back(manaAction((double) random<int>(150, 250), "Minor Recombobulator"));
        cooldown->duration = 300;
    }

    if (buff != nullptr)
        actions.push_back(buffAction(buff));

    if (actions.size() > 0) {
        actions.push_back(cooldownAction(cooldown));
        if (trinketSharesCD(trinket) && buff != nullptr)
            actions.push_back(cooldownAction<cooldown::TrinketShared>(buff->duration));
    }

    return actions;
}

action::Action Player::useCooldown(const State& state)
{
    if (state.isMoving()) {
        if (talents.presence_of_mind && !hasCooldown(cooldown::PRESENCE_OF_MIND) && useTimingIfPossible("presence_of_mind", state)) {
            return buffCooldownAction<buff::PresenceOfMind, cooldown::PresenceOfMind>(true);
        }
        return { action::TYPE_NONE };
    }

    if (talents.arcane_power && !hasCooldown(cooldown::ARCANE_POWER) && useTimingIfPossible("arcane_power", state)) {
        return buffCooldownAction<buff::ArcanePower, cooldown::ArcanePower>(true);
    }
    else if (talents.combustion && !hasCooldown(cooldown::COMBUSTION) && !hasBuff(buff::COMBUSTION) && useTimingIfPossible("combustion", state)) {
        combustion = 0;
        return buffAction<buff::Combustion>(true);
    }
    else if (talents.presence_of_mind && !hasCooldown(cooldown::PRESENCE_OF_MIND) && useTimingIfPossible("presence_of_mind", state)) {
        return buffCooldownAction<buff::PresenceOfMind, cooldown::PresenceOfMind>(true);
    }
    else if (runes.icy_veins && !hasCooldown(cooldown::ICY_VEINS) && useTimingIfPossible("icy_veins", state)) {
        return buffCooldownAction<buff::IcyVeins, cooldown::IcyVeins>(true);
    }
    else if (talents.cold_snap && !hasCooldown(cooldown::COLD_SNAP) && useTimingIfPossible("cold_snap", state, true)) {
        return spellCooldownAction<spell::ColdSnap, cooldown::ColdSnap>();
    }
    else if (race == RACE_TROLL && !hasCooldown(cooldown::BERSERKING) && useTimingIfPossible("berserking", state)) {
        return buffCooldownAction<buff::Berserking, cooldown::Berserking>(true);
    }
    else if (config.item_gneuro_linked_monocle && !hasCooldown(cooldown::CHARGED_INSPIRATION) && useTimingIfPossible("gneuro_linked_monocle", state)) {
        return buffCooldownAction<buff::ChargedInspiration, cooldown::ChargedInspiration>(true);
    }
    else if (config.item_hyperconductive_goldwrap && !hasCooldown(cooldown::COIN_FLIP) && useTimingIfPossible("hyperconductive_goldwrap", state)) {
        if (random<int>(0, 1) == 0)
            return buffCooldownAction<buff::CoinFlipHeads, cooldown::CoinFlip>(true);
        else
            return buffCooldownAction<buff::CoinFlipTails, cooldown::CoinFlip>(true);
    }
    else if (config.potion != POTION_NONE && !hasCooldown(cooldown::POTION) && !hasManaPotion() && useTimingIfPossible("potion", state)) {
        action::Action action { action::TYPE_POTION };
        action.primary_action = true;
        return action;
    }
    else if (!hasCooldown(cooldown::POTION) && hasManaPotion() && useTimingIfPossible("potion", state, true)) {
        action::Action action { action::TYPE_POTION };
        action.primary_action = true;
        return action;
    }
    else if (!hasCooldown(cooldown::MANA_GEM) && useTimingIfPossible("mana_gem", state, true)) {
        return spellAction<spell::ManaGem>();
    }
    else if (isUseTrinket(config.trinket1) && !hasCooldown(cooldown::TRINKET1) && !isTrinketOnSharedCD(config.trinket1) && useTimingIfPossible("trinket1", state)) {
        action::Action action{ action::TYPE_TRINKET };
        action.cooldown = std::make_shared<cooldown::Cooldown>(cooldown::TRINKET1);
        action.trinket = config.trinket1;
        action.primary_action = true;
        return action;
    }
    else if (isUseTrinket(config.trinket2) && !hasCooldown(cooldown::TRINKET2) && !isTrinketOnSharedCD(config.trinket2) && useTimingIfPossible("trinket2", state)) {
        action::Action action{ action::TYPE_TRINKET };
        action.cooldown = std::make_shared<cooldown::Cooldown>(cooldown::TRINKET2);
        action.trinket = config.trinket2;
        action.primary_action = true;
        return action;
    }
    else if (config.trinket1 == TRINKET_RECOMBO && !hasCooldown(cooldown::TRINKET1) && !isTrinketOnSharedCD(config.trinket1) && 
            (useTimingIfPossible("trinket1", state, true) || !isTimingReadySoon("trinket1", state, 300.0) && maxMana() - mana >= 250.0))
    {
        action::Action action{ action::TYPE_TRINKET };
        action.cooldown = std::make_shared<cooldown::Cooldown>(cooldown::TRINKET1);
        action.trinket = config.trinket1;
        action.primary_action = true;
        return action;
    }
    else if (config.trinket2 == TRINKET_RECOMBO && !hasCooldown(cooldown::TRINKET2) && !isTrinketOnSharedCD(config.trinket2) && 
            (useTimingIfPossible("trinket2", state, true) || !isTimingReadySoon("trinket2", state, 300.0) && maxMana() - mana >= 250.0))
    {
        action::Action action{ action::TYPE_TRINKET };
        action.cooldown = std::make_shared<cooldown::Cooldown>(cooldown::TRINKET2);
        action.trinket = config.trinket2;
        action.primary_action = true;
        return action;
    }
    else if (!hasCooldown(cooldown::EVOCATION) && useTimingIfPossible("evocation", state, true)) {
        return spellAction<spell::Evocation>();
    }
    else if (config.item_robe_archmage && maxMana() - mana >= 625.0 && !hasCooldown(cooldown::ROBE_ARCHMAGE)) {
        action::Action action{ action::TYPE_MANA };
        action.value = (double) random<int>(375, 625);
        action.str = "Robe of the Archmage";
        action.cooldown = std::make_shared<cooldown::RobeArchmage>();
        action.primary_action = true;
        return action;
    }
    else if (config.item_celestial_orb && maxMana() - mana >= 1200.0 && !hasCooldown(cooldown::CELESTIAL_ORB)) {
        action::Action action{ action::TYPE_MANA };
        action.value = (double) random<int>(400, 1200);
        action.str = "Celestial Orb";
        action.cooldown = std::make_shared<cooldown::CelestialOrb>();
        action.primary_action = true;
        return action;
    }

    return { action::TYPE_NONE };
}

bool Player::shouldPreCast() const
{
    return config.pre_cast && config.rotation < ROTATION_AOE_AE;
}

std::shared_ptr<spell::Spell> Player::preCastSpell()
{
    if (config.rotation == ROTATION_ST_FIRE) {
        if (runes.frostfire_bolt)
            return std::make_shared<spell::FrostfireBolt>(config.player_level);
        else if (talents.pyroblast)
            return std::make_shared<spell::Pyroblast>(config.player_level);
        else
            return std::make_shared<spell::Fireball>(config.player_level);
    }
    if (config.rotation == ROTATION_ST_FIRE_SC) {
        if (talents.pyroblast)
            return std::make_shared<spell::Pyroblast>(config.player_level);
        else
            return std::make_shared<spell::Scorch>(config.player_level);
    }
    if (config.rotation == ROTATION_ST_ARCANE && runes.arcane_blast)
        return std::make_shared<spell::ArcaneBlast>(config.player_level);
    if (config.rotation == ROTATION_ST_FROST)
        return std::make_shared<spell::Frostbolt>(config.player_level);

    return std::make_shared<spell::Fireball>(config.player_level);
}

double Player::preCombatDuration(const State& state)
{
    double t = 0;

    if (shouldPreCast()) {
        auto spell = preCastSpell();

        t += castTime(spell);
        t -= 0.1; // Assume we don't have perfect timing
    }

    return t;
}

action::Action Player::preCombat(const State& state)
{
    if (shouldPreCast()) {
        if (!has_pre_cast) {
            has_pre_cast = true;
            return spellAction(preCastSpell(), state.targets[0]);
        }
    }

    return { action::TYPE_NONE };
}

action::Action Player::nextAction(const State& state)
{
    if (!state.inCombat())
        return preCombat(state);

    auto gcd = gcdAction(state.t);

    if (gcd.type != action::TYPE_NONE)
        return gcd;

    auto cd = useCooldown(state);

    if (cd.type != action::TYPE_NONE)
        return cd;

    // Mana consumes
    if (shouldUseManaGem(state)) {
        return spellAction<spell::ManaGem>();
    }
    else if (shouldUseManaPotion(state)) {
        action::Action action{ action::TYPE_POTION };
        action.primary_action = true;
        return action;
    }
    else if (shouldEvocate(state)) {
        return spellAction<spell::Evocation>();
    }

    // Default target
    auto target = state.targets[0];

    // Fire rotations
    if (config.rotation == ROTATION_ST_FIRE || config.rotation == ROTATION_ST_FIRE_SC) {

        bool hot_streak = canReactTo(buff::HOT_STREAK, state.t) && talents.pyroblast;
        bool multi_target = config.targets > 1;
        auto pyroblast = std::make_shared<spell::Pyroblast>(config.player_level);
        bool pyro_will_land = travelTime(pyroblast) <= state.duration - state.t;

        if (config.maintain_imp_scorch && talents.imp_scorch && !state.isMoving()) {
            if (target->debuffStacks(debuff::IMPROVED_SCORCH) < 5 || state.t - t_scorch >= 10.0 + talents.imp_scorch * 4.0)
                return spellAction<spell::Scorch>(target);
        }

        // Pyroblast - first check
        if (hot_streak && pyro_will_land && (multi_target || heating_up)) {
            if (multi_target && !config.only_main_dmg) {
                for (auto const& tar : state.targets) {
                    if (tar->t_pyroblast + 12.0 < state.t)
                        return spellAction<spell::Pyroblast>(tar);
                }
            }
            return spellAction(pyroblast, target);
        }

        if (runes.living_flame && !hasCooldown(cooldown::LIVING_FLAME)) {

            if (runes.arcane_blast && state.timeRemain() > 20.0) {
                auto ab = std::make_shared<spell::ArcaneBlast>(config.player_level);
                int ab_stacks = 3;
                if (manaPercent() > 20)
                    ab_stacks = 4;
                if (ab_streak < ab_stacks && canCast(ab))
                    return spellAction(ab, target);
                else
                    return spellAction<spell::LivingFlame>(config.distance);
            }

            return spellAction<spell::LivingFlame>(config.distance);
        }

        if (runes.missile_barrage && canReactTo(buff::MISSILE_BARRAGE, state.t) && !state.isMoving())
            return spellAction<spell::ArcaneMissiles>(target);

        // Check for Living Bomb targets
        if (runes.living_bomb && state.timeRemain() >= 12.0) {
            for (auto const& tar : state.targets) {
                if (tar->t_living_bomb + 12.0 < state.t && tar->id <= config.dot_targets)
                    return spellAction<spell::LivingBomb>(tar);
            }
        }

        std::shared_ptr<spell::Spell> main_spell;
        if (config.rotation == ROTATION_ST_FIRE_SC)
            main_spell = std::make_shared<spell::Scorch>(config.player_level);
        else if (runes.frostfire_bolt)
            main_spell = std::make_shared<spell::FrostfireBolt>(config.player_level);
        else
            main_spell = std::make_shared<spell::Fireball>(config.player_level);

        // Pyroblast - second check
        if (hot_streak && pyro_will_land)
            return spellAction(pyroblast, target);

        // Fire blast weave
        if (!multi_target || config.only_main_dmg) {
            double t_explosion = state.targets[0]->t_living_bomb + 12.0 - state.t;

            if (config.rot_fire_blast_weave && !hasCooldown(cooldown::FIRE_BLAST)) {
                if (runes.living_bomb) {
                    if (t_explosion <= 1.5)
                        return spellAction<spell::FireBlast>(target);
                }
                else {
                    return spellAction<spell::FireBlast>(target);
                }
            }

            if (runes.living_bomb && t_explosion < 0.05 && state.timeRemain() >= 12.0) {
                action::Action action{ action::TYPE_WAIT };
                action.value = std::max(t_explosion, 0.0) + 0.01;
                return action;
            }
        }

        if (!hasBuff(buff::PRESENCE_OF_MIND) && !hasBuff(buff::NETHERWIND_FOCUS) && !hasBuff(buff::BRAIN_FREEZE)) {
            // Last second finishers
            auto scorch = std::make_shared<spell::Scorch>(config.player_level);
            if (state.duration - state.t < castTime(scorch) && !hasCooldown(cooldown::FIRE_BLAST))
                return spellAction<spell::FireBlast>(target);
            if (state.duration - state.t < castTime(main_spell) + travelTime(main_spell))
                return spellAction(scorch, target);

            // Moving
            if (state.isMoving()) {
                if (config.targets > 2 && config.distance <= 10) {
                    return spellAction<spell::ArcaneExplosion>();
                }
                else if (!hasCooldown(cooldown::FIRE_BLAST)) {
                    return spellAction<spell::FireBlast>(target);
                }
                else {
                    action::Action action{ action::TYPE_WAIT };
                    action.value = state.interruptedFor();
                    if (action.value > 0.1)
                        action.value = 0.1;
                    return action;
                }
            }
        }

        if (config.rotation == ROTATION_ST_FIRE_SC && config.rot_combustion_fb && hasBuff(buff::COMBUSTION))
            return spellAction<spell::Fireball>(target);

        return spellAction(main_spell, target);
    }

    // Arcane rotations
    else if (config.rotation == ROTATION_ST_ARCANE) {
        auto ab = std::make_shared<spell::ArcaneBlast>(config.player_level);
        int ab_stacks = 0;

        if (runes.arcane_blast) {
            ab_stacks = config.rot_ab_stacks;
            if (config.rot_ab_stacks_dec_below >= manaPercent())
                ab_stacks--;
        }

        if (!state.isMoving()) {
            if (runes.regeneration && !hasBuff(buff::TEMPORAL_BEACON)) {
                return spellAction<spell::Regeneration>();
            }
            else if (runes.mass_regeneration && !hasBuff(buff::TEMPORAL_BEACON_PARTY)) {
                return spellAction<spell::MassRegeneration>();
            }
        }

        if (runes.living_flame && !hasCooldown(cooldown::LIVING_FLAME)) {
            if (runes.arcane_blast) {
                if (state.timeRemain() <= 21)
                    ab_stacks = 0;
                else if (manaPercent() > 20)
                    ab_stacks = 4;
            }

            if (ab_streak < ab_stacks)
                return spellAction(ab, target);
            else
                return spellAction<spell::LivingFlame>(config.distance);
        }

        if (runes.arcane_surge && 
            !hasCooldown(cooldown::ARCANE_SURGE) &&
            (state.timeRemain() < 2.5 ||
             manaPercent() < 20.0 && ab_streak >= ab_stacks ||
             manaPercent() < 30.0 && !runes.arcane_blast))
        {
            return spellAction<spell::ArcaneSurge>();
        }

        if (config.rot_fire_blast_weave && !hasCooldown(cooldown::FIRE_BLAST))
            return spellAction<spell::FireBlast>(target);

        if (state.isMoving() && !hasBuff(buff::PRESENCE_OF_MIND) && !hasBuff(buff::NETHERWIND_FOCUS)) {
            if (config.targets > 2 && config.distance <= 10) {
                return spellAction<spell::ArcaneExplosion>();
            }
            else if (!hasCooldown(cooldown::FIRE_BLAST)) {
                return spellAction<spell::FireBlast>(target);
            }
            else {
                action::Action action{ action::TYPE_WAIT };
                action.value = state.interruptedFor();
                if (action.value > 0.1)
                    action.value = 0.1;
                return action;
            }
        }
        
        if (state.duration - state.t < castTime(ab) && !hasCooldown(cooldown::FIRE_BLAST))
            return spellAction<spell::FireBlast>(target);
        // Spam AB above certain mana %
        else if (config.rot_ab_spam_above < manaPercent() && runes.arcane_blast)
            return spellAction(ab, target);
        else if (ab_streak >= ab_stacks)
            return spellAction<spell::ArcaneMissiles>(target);
        // AM if we have MB and below mana %
        else if (config.rot_mb_mana && manaPercent() < config.rot_mb_mana && canReactTo(buff::MISSILE_BARRAGE, state.t))
            return spellAction<spell::ArcaneMissiles>(target);
        else if (runes.arcane_blast && canCast(ab))
            return spellAction(ab, target);
        else
            return spellAction<spell::ArcaneMissiles>(target);
    }

    // Frost
    else if (config.rotation == ROTATION_ST_FROST) {

        if (hasBuff(buff::GHOST_FINGERS) && config.rot_ice_lance)
            return spellAction<spell::IceLance>(target);

        if (canReactTo(buff::BRAIN_FREEZE, state.t)) {
            if (runes.frostfire_bolt)
                return spellAction<spell::FrostfireBolt>(target);
            else
                return spellAction<spell::Fireball>(target);
        }

        if (config.rot_fire_blast_weave && !hasCooldown(cooldown::FIRE_BLAST))
            return spellAction<spell::FireBlast>(target);

        if (state.isMoving() && !hasBuff(buff::PRESENCE_OF_MIND) && !hasBuff(buff::NETHERWIND_FOCUS)) {
            if (config.targets > 2 && config.distance <= 10) {
                return spellAction<spell::ArcaneExplosion>();
            }
            else if (!hasCooldown(cooldown::FIRE_BLAST)) {
                return spellAction<spell::FireBlast>(target);
            }
            else {
                action::Action action{ action::TYPE_WAIT };
                action.value = state.interruptedFor();
                if (action.value > 0.1)
                    action.value = 0.1;
                return action;
            }
        }

        if (runes.frostfire_bolt)
            return spellAction<spell::FrostfireBolt>(target);

        return spellAction<spell::Frostbolt>(target);
    }

    // Arcane Explosion
    else if (config.rotation == ROTATION_AOE_AE) {
        return spellAction<spell::ArcaneExplosion>();
    }

    // Arcane Explosion + Flamestrike
    else if (config.rotation == ROTATION_AOE_AE_FS) {
        auto fs = std::make_shared<spell::Flamestrike>(config.player_level);
        if (state.isMoving())
            return spellAction<spell::ArcaneExplosion>();
        else if (t_flamestrike + 8.0 - castTime(fs) <= state.t)
            return spellAction(fs);
        else if (t_flamestrike_dr + 8.0 - castTime(fs) <= state.t && state.t + 8.0 < state.duration)
            return spellAction<spell::FlamestrikeDR>();
        else
            return spellAction<spell::ArcaneExplosion>();
    }

    // Blizzard
    else if (config.rotation == ROTATION_AOE_BLIZZ) {
        if (state.isMoving())
            return spellAction<spell::ArcaneExplosion>();
        return spellAction<spell::Blizzard>();
    }

    // Blizzard + Flamestrike
    else if (config.rotation == ROTATION_AOE_BLIZZ_FS) {
        auto fs = std::make_shared<spell::Flamestrike>(config.player_level);
        if (state.isMoving())
            return spellAction<spell::ArcaneExplosion>();
        else if (t_flamestrike + 8.0 - castTime(fs) <= state.t)
            return spellAction(fs);
        else if (t_flamestrike_dr + 8.0 - castTime(fs) <= state.t && state.t + 8.0 < state.duration)
            return spellAction<spell::FlamestrikeDR>();
        else
            return spellAction<spell::Blizzard>();
    }

    // Flamestrike
    else if (config.rotation == ROTATION_AOE_FS) {
        auto fs = std::make_shared<spell::Flamestrike>(config.player_level);
        double ct = castTime(fs);
        if (t_flamestrike + 8.0 - ct > state.t && t_flamestrike_dr + 8.0 - ct <= state.t && state.t + 8.0 < state.duration)
            return spellAction<spell::FlamestrikeDR>();
        return spellAction(fs);
    }

    // Fire
    else if (config.rotation == ROTATION_AOE_FIRE) {
        auto fs = std::make_shared<spell::Flamestrike>(config.player_level);

        // Above aoe cap
        if (config.targets > 12 && !state.isMoving()) {
            if (t_flamestrike + 8.0 - castTime(fs) <= state.t && state.t + 8.0 < state.duration)
                return spellAction(fs);
            else if (t_flamestrike_dr + 8.0 - castTime(fs) <= state.t && state.t + 8.0 < state.duration)
                return spellAction<spell::FlamestrikeDR>();
        }

        // Check for Living Bomb targets
        if (runes.living_bomb && state.t + 12.0 < state.duration) {
            for (auto const& tar : state.targets) {
                if (tar->t_living_bomb + 12.0 < state.t && tar->id <= config.dot_targets)
                    return spellAction<spell::LivingBomb>(tar);
            }
        }

        if (state.isMoving())
            return spellAction<spell::ArcaneExplosion>();
        else if (t_flamestrike + 8.0 - castTime(fs) <= state.t && state.t + 8.0 < state.duration)
            return spellAction(fs);
        else if (t_flamestrike_dr + 8.0 - castTime(fs) <= state.t && state.t + 8.0 < state.duration)
            return spellAction<spell::FlamestrikeDR>();
        else
            return spellAction(fs);
    }

    // Undefined rotation
    else {
        return spellAction<spell::Fireball>(target);
    }

    throw std::runtime_error("Player::nextAction failed to decide");
}

}