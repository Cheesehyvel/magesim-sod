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
    base_mana = 493; // lvl 25
    name = "Player";
    id = 1;
    used_timings.resize(_config.timings.size());
}

void Player::reset()
{
    Unit::reset();
    combustion = 0;
    fingers_of_frost = 0;
    t_flamestrike = -20;
    t_flamestrike_dr = -20;
    t_scorch = -60;
    t_mana_spent = 0;
    t_living_bomb = -20;
    ab_streak = 0;
    has_pre_cast = false;
    crit_suppression = true;

    // Enable these as we go up in level
    mana_ruby = false;
    mana_citrine = false;
    mana_jade = false;
    mana_agate = false;

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

    if (hasBuff(buff::PRESENCE_OF_MIND) && !spell->channeling)
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

    return crit;
}

double Player::critMultiplierMod(std::shared_ptr<spell::Spell> spell) const
{
    double multi = Unit::critMultiplierMod(spell);

    if (spell->proc)
        return multi;

    if (spell->isSchool(SCHOOL_FROST) && talents.ice_shards)
        multi += talents.ice_shards * 0.2;

    return multi;
}


double Player::spellPenetration(std::shared_ptr<spell::Spell> spell) const
{
    double pen = Unit::spellPenetration(spell);

    if (spell->isSchool(SCHOOL_ARCANE) && talents.arcane_subtlety)
        pen+= 5.0 * talents.arcane_subtlety;

    return pen;
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

    if (spell->school == SCHOOL_ARCANE && spell->id != spell::ARCANE_BLAST && hasBuff(buff::ARCANE_BLAST, true)) {
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

double Player::manaCostMultiplier(std::shared_ptr<spell::Spell> spell) const
{
    double multi = Unit::manaCostMultiplier(spell);

    if (hasBuff(buff::CLEARCAST) && spell->id != spell::ARCANE_SURGE)
        return 0;

    if (talents.frost_channeling)
        multi*= 0.05 * talents.frost_channeling;

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

bool Player::shouldConsumeClearcast(std::shared_ptr<spell::Spell> spell) const
{
    if (spell->is_trigger || spell->tick)
        return false;

    return true;
}

bool Player::isFrozen() const
{
    return hasBuff(buff::FINGERS_OF_FROST) || hasBuff(buff::GHOST_FINGERS);
}

bool Player::hasChillEffect(std::shared_ptr<spell::Spell> spell) const
{
    if (spell->id == spell::CONE_OF_COLD || spell->id == spell::FROSTBOLT)
        return true;

    return false;
}

std::vector<action::Action> Player::onBuffGain(const State& state, std::shared_ptr<buff::Buff> buff)
{
    auto actions = Unit::onBuffGain(state, buff);

    if (buff->id == buff::ARCANE_BLAST) {
        ab_streak++;
    }

    return actions;
}

std::vector<action::Action> Player::onBuffExpire(const State& state, std::shared_ptr<buff::Buff> buff)
{
    std::vector<action::Action> actions = Unit::onBuffExpire(state, buff);

    if (buff->id == buff::ARCANE_BLAST)
        ab_streak = 0;

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

    if (hasBuff(buff::CLEARCAST) && shouldConsumeClearcast(spell))
        actions.push_back(buffExpireAction<buff::Clearcast>());

    if (spell->id == spell::ARCANE_BLAST)
        actions.push_back(buffAction<buff::ArcaneBlast>());
    else if (hasBuff(buff::ARCANE_BLAST) && spell->school == SCHOOL_ARCANE && spell->min_dmg > 0)
        actions.push_back(buffExpireAction<buff::ArcaneBlast>());

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

        // Some proc trinkets here
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

    if (instance.result != spell::MISS) {
        if (talents.imp_scorch && instance.spell->id == spell::SCORCH && (talents.imp_scorch == 3 || random<int>(0, 2) < talents.imp_scorch)) {
            t_scorch = state.t;
            actions.push_back(debuffAction<debuff::ImprovedScorch>(target));
        }

        if (instance.spell->id == spell::FIREBALL)
            actions.push_back(spellAction<spell::FireballDot>(target));
        if (instance.spell->id == spell::PYROBLAST)
            actions.push_back(spellAction<spell::PyroblastDot>(target));

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

            // 50% chance, no icd ? unconfirmed
            // if (config.judgement_of_wisdom) {
            //     double chance = 25;
            //     if (instance.spell->actual_cast_time > 0)
            //         chance*= instance.spell->actual_cast_time;
            //     else
            //         chance*= 0.75;
            //     if (instance.spell->channeling)
            //         chance/= (double) instance.spell->ticks;
            //     if (random<double>(0, 100) < chance)
            //         actions.push_back(manaAction(33.0, "Judgement of Wisdom"));
            // }

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
        // TODO: MAJOR REWORK FOR VANILLA IGNITE
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
                actions.push_back(manaAction(instance.spell->actual_cost * -0.01 * runes.burnout, "Burnout"));
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
    if (hasCooldown(cooldown::MANA_GEM) || !hasManaGem())
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

bool Player::shouldUseManaPotion(const State& state)
{
    if (hasCooldown(cooldown::POTION))
        return false;

    // Check for planned potions timings
    auto timing = getNextTiming("potion");
    if (timing)
        return false;

    if (hasBuff(buff::INNERVATE))
        return false;

    double max = 360;

    if (hasTrinket(TRINKET_ALCHEMIST_STONE))
        max *= 1.33;

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

    double duration = 120;
    double mana = round(random<double>(280, 360));

    if (hasTrinket(TRINKET_ALCHEMIST_STONE))
        mana *= 1.33;

    actions.push_back(manaAction(mana, "Mana Potion"));
    actions.push_back(cooldownAction<cooldown::Potion>(duration));

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
    return true;
}

bool Player::isUseTrinket(Trinket trinket) const
{
    if (trinket == TRINKET_MQG)
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

    if (trinket == TRINKET_MQG) {
        buff = std::make_shared<buff::MindQuickening>();
        cooldown->duration = 300;
    }
    if (trinket == TRINKET_RECOMBO) {
        actions.push_back(manaAction((double) random<int>(150, 250), "Minor Recombobulator"));
        cooldown->duration = 300;
    }

    if (buff != nullptr)
        actions.push_back(buffAction(buff));

    if (actions.size() > 0) {
        actions.push_back(cooldownAction(cooldown));
        if (trinketSharesCD(trinket))
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
        auto action = buffAction<buff::ArcanePower>();
        action.cooldown = std::make_shared<cooldown::ArcanePower>();
        return action;
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
    else if (!hasCooldown(cooldown::POTION) && useTimingIfPossible("potion", state, true)) {
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

    return { action::TYPE_NONE };
}

bool Player::canBlast(const State& state) const
{
    // Lets assume we cant blast for 30+ seconds to save computing time
    if (state.timeRemain() > 30)
        return false;

    auto ab = std::make_shared<spell::ArcaneBlast>();
    auto const cast_time = 2.5 * castHaste();
    auto const base_cost = baseManaCost(ab);

    double cur_mana = mana;
    int stacks = buffStacks(buff::ARCANE_BLAST);

    for (auto t = state.t; t <= state.duration; t += cast_time) {
        if (stacks < 4)
            stacks++;
        cur_mana -= base_cost * 1.75 * stacks;
        if (cur_mana < 0)
            return false;
    }

    return true;
}

bool Player::shouldPreCast() const
{
    return config.pre_cast && config.rotation < ROTATION_AOE_AE;
}

std::shared_ptr<spell::Spell> Player::preCastSpell()
{
    if (config.rotation == ROTATION_ST_FIRE)
        return std::make_shared<spell::Fireball>();
    if (config.rotation == ROTATION_ST_ARCANE && runes.arcane_blast)
        return std::make_shared<spell::ArcaneBlast>();
    if (config.rotation == ROTATION_ST_FROST)
        return std::make_shared<spell::Frostbolt>();
    if (config.rotation == ROTATION_ST_FIRE_SC)
        return std::make_shared<spell::Scorch>();

    return std::make_shared<spell::Fireball>();
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

    if (config.maintain_imp_scorch && talents.imp_scorch) {
        if (target->debuffStacks(debuff::IMPROVED_SCORCH) < 5 || state.t - t_scorch >= 15.0 + talents.imp_scorch * 4.0)
            return spellAction<spell::Scorch>(target);
    }

    // Fire rotations
    if (config.rotation == ROTATION_ST_FIRE || config.rotation == ROTATION_ST_FIRE_SC) {

        bool multi_target = config.targets > 1;

        if (runes.living_flame && !hasCooldown(cooldown::LIVING_FLAME)) {
            return spellAction<spell::LivingFlame>(config.distance);
        }

        // Check for Living Bomb targets
        if (runes.living_bomb && state.timeRemain() >= 12.0) {
            for (auto const& tar : state.targets) {
                if (tar->t_living_bomb + 12.0 < state.t && tar->id <= config.dot_targets)
                    return spellAction<spell::LivingBomb>(tar);
            }
        }

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

        std::shared_ptr<spell::Spell> main_spell;
        if (config.rotation == ROTATION_ST_FIRE)
            main_spell = std::make_shared<spell::Fireball>();
        else
            main_spell = std::make_shared<spell::Scorch>();

        if (!hasBuff(buff::PRESENCE_OF_MIND)) {
            // Last second finishers
            auto scorch = std::make_shared<spell::Scorch>();
            if (state.duration - state.t < castTime(scorch) && !hasCooldown(cooldown::FIRE_BLAST))
                return spellAction<spell::FireBlast>(target);
            if (state.duration - state.t < castTime(main_spell) + travelTime(main_spell))
                return spellAction<spell::Scorch>(target);

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

        return spellAction(main_spell, target);
    }

    // Arcane rotations
    else if (config.rotation == ROTATION_ST_ARCANE) {
        auto ab = std::make_shared<spell::ArcaneBlast>();
        int ab_stacks = config.rot_ab_stacks;

        if (config.rot_ab_stacks_dec_below >= manaPercent())
            ab_stacks--;

        if (!state.isMoving()) {
            if (runes.regeneration && !hasBuff(buff::TEMPORAL_BEACON)) {
                return spellAction<spell::Regeneration>();
            }
            else if (runes.mass_regeneration && !hasBuff(buff::TEMPORAL_BEACON_PARTY)) {
                return spellAction<spell::MassRegeneration>();
            }
        }

        if (runes.living_flame && !hasCooldown(cooldown::LIVING_FLAME)) {
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

        if (config.rot_fire_blast_weave && !hasCooldown(cooldown::FIRE_BLAST)) {
            return spellAction<spell::FireBlast>(target);
        }

        if (state.isMoving() && !hasBuff(buff::PRESENCE_OF_MIND)) {
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
        else if (config.rot_ab_spam_above <= manaPercent())
            return spellAction(ab, target);
        else if (ab_streak >= ab_stacks)
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

        if (config.rot_fire_blast_weave && !hasCooldown(cooldown::FIRE_BLAST)) {
            return spellAction<spell::FireBlast>(target);
        }

        if (state.isMoving() && !hasBuff(buff::PRESENCE_OF_MIND)) {
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

        return spellAction<spell::Frostbolt>(target);
    }

    // Arcane Explosion
    else if (config.rotation == ROTATION_AOE_AE) {
        return spellAction<spell::ArcaneExplosion>();
    }

    // Arcane Explosion + Flamestrike
    else if (config.rotation == ROTATION_AOE_AE_FS) {
        auto fs = std::make_shared<spell::Flamestrike>();
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
        auto fs = std::make_shared<spell::Flamestrike>();
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
        auto fs = std::make_shared<spell::Flamestrike>();
        double ct = castTime(fs);
        if (t_flamestrike + 8.0 - ct > state.t && t_flamestrike_dr + 8.0 - ct <= state.t && state.t + 8.0 < state.duration)
            return spellAction<spell::FlamestrikeDR>();
        return spellAction(fs);
    }

    // Fire
    else if (config.rotation == ROTATION_AOE_FIRE) {
        auto fs = std::make_shared<spell::Flamestrike>();

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