#include "unit.h"
#include "state.h"
#include "config.h"
#include "common.h"
#include "spell.h"
#include "action.h"

#include <memory>
#include <string>
#include <exception>

namespace unit
{
Unit::Unit(const Config& _config, const Stats& _stats)
    : config(_config), stats(_stats) {}

void Unit::reset()
{
    mana = maxMana();
    t_gcd = 0;
    t_gcd_capped = 0;
    t_oom = 0;
    is_channeling = false;
    last_spell_cast_time = 0;

    buffs.clear();
    snapshot_buffs.clear();
    cooldowns.clear();

    buff_stats.intellect = 0;
    buff_stats.spirit = 0;
    buff_stats.mp5 = 0;
    buff_stats.crit = 0;
    buff_stats.hit = 0;
    buff_stats.sp = 0;
    buff_stats.sp_arcane = 0;
    buff_stats.sp_fire = 0;
    buff_stats.sp_frost = 0;
    buff_stats.spen = 0;
    buff_stats.mana = 0;
}

bool Unit::hasCooldown(cooldown::ID id) const
{
    return cooldowns.find(id) != cooldowns.end();
}

double Unit::cooldownDuration(cooldown::ID id, const State& state) const
{
    auto const itr = cooldowns.find(id);
    if (itr == cooldowns.end())
        return 0;

    return itr->second->t_expires - state.t;
}

void Unit::addCooldown(std::shared_ptr<cooldown::Cooldown> cooldown)
{
    cooldowns[cooldown->id] = cooldown;
}

void Unit::removeCooldown(cooldown::ID id)
{
    cooldowns.erase(id);
}

int Unit::buffStacks(buff::ID id, bool snapshot) const
{
    auto const itr = buffs.find(id);
    if (itr != buffs.end())
        return itr->second->stacks;

    if (snapshot)
    {
        auto const snapshot_itr = snapshot_buffs.find(id);
        if (snapshot_itr != snapshot_buffs.end())
            return snapshot_itr->second->stacks;
    }

    return 0;
}

double Unit::buffDuration(buff::ID id, const State& state) const
{
    auto const itr = buffs.find(id);
    if (itr == buffs.end())
        return 0;

    return itr->second->t_expires - state.t;
}

bool Unit::hasBuff(buff::ID id, bool snapshot) const
{
    if (buffs.find(id) != buffs.end())
        return true;

    if (snapshot && hasSnapshot(id))
        return true;

    return false;
}

int Unit::addBuff(std::shared_ptr<buff::Buff> buff)
{
    auto const stacks = buffStacks(buff->id);
    bool has_buff = hasBuff(buff->id);

    int add = buff->stack_increment;

    if (add < 0 && !has_buff)
        add = buff->max_stacks;
    else if (stacks + add > buff->max_stacks)
        add = buff->max_stacks - stacks;
    else if (stacks + add < 0)
        add = -stacks;

    if (add != 0)
        addBuffStats(buff->stats, add);

    if (!has_buff) {
        buff->stacks = add;
        buffs[buff->id] = buff;
    }
    else if (add != 0) {
        buffs[buff->id]->t_expires = buff->t_expires;
        buffs[buff->id]->addStacks(add);
    }

    return buffs[buff->id]->stacks;
}

void Unit::removeBuff(buff::ID id, bool snapshot)
{
    auto const i = buffs.find(id);
    if (i != buffs.end())
    {
        auto const& buff = i->second;

        if (snapshot)
            snapshot_buffs[id] = buff;
        else
            removeBuffStats(buff->stats, buff->stacks);

        buffs.erase(id);
    }
}

bool Unit::canReactTo(buff::ID id, double t) const
{
    if (t < t_gcd)
        t = t_gcd;

    auto const i = buffs.find(id);

    return i != buffs.end() && t - i->second->t_gained >= config.reaction_time / 1000.0;
}

bool Unit::canReactTo(cooldown::ID id, double t) const
{
    if (t < t_gcd)
        t = t_gcd;

    auto const i = cooldowns.find(id);

    return i != cooldowns.end() && t - i->second->t_gained >= config.reaction_time / 1000.0;
}

bool Unit::hasSnapshot(buff::ID id) const
{
    return snapshot_buffs.find(id) != snapshot_buffs.end();
}

void Unit::removeSnapshot(buff::ID id)
{
    auto const i = snapshot_buffs.find(id);
    if (i != snapshot_buffs.end())
    {
        removeBuffStats(i->second->stats, i->second->stacks);
        snapshot_buffs.erase(i);
    }
}

void Unit::removeSnapshots()
{
    for (auto it = snapshot_buffs.begin(); it != snapshot_buffs.end(); ++it)
        removeBuffStats(it->second->stats, it->second->stacks);
    snapshot_buffs.clear();
}

void Unit::addBuffStats(const Stats& _stats, int stacks)
{
    buff_stats.intellect += _stats.intellect * stacks;
    buff_stats.spirit += _stats.spirit * stacks;
    buff_stats.mp5 += _stats.mp5 * stacks;
    buff_stats.crit += _stats.crit * stacks;
    buff_stats.hit += _stats.hit * stacks;
    buff_stats.sp += _stats.sp * stacks;
    buff_stats.sp_arcane += _stats.sp_arcane * stacks;
    buff_stats.sp_fire += _stats.sp_fire * stacks;
    buff_stats.sp_frost += _stats.sp_frost * stacks;
    buff_stats.spen += _stats.spen * stacks;
    buff_stats.mana += _stats.mana * stacks;
}

void Unit::removeBuffStats(const Stats& _stats, int stacks)
{
    buff_stats.intellect -= _stats.intellect * stacks;
    buff_stats.spirit -= _stats.spirit * stacks;
    buff_stats.mp5 -= _stats.mp5 * stacks;
    buff_stats.crit -= _stats.crit * stacks;
    buff_stats.hit -= _stats.hit * stacks;
    buff_stats.sp -= _stats.sp * stacks;
    buff_stats.sp_arcane -= _stats.sp_arcane * stacks;
    buff_stats.sp_fire -= _stats.sp_fire * stacks;
    buff_stats.sp_frost -= _stats.sp_frost * stacks;
    buff_stats.spen -= _stats.spen * stacks;
    buff_stats.mana -= _stats.mana * stacks;
}

void Unit::applyMana(const State& state, double _mana)
{
    mana+= _mana;

    if (mana > maxMana())
        mana = maxMana();
    else if (mana < 0)
        mana = 0;
}

double Unit::spiritManaPerSecond() const
{
    double spi = getSpirit();
    return 0.25 * std::min(spi, 50.0) + 0.125 * (std::max(spi, 50.0) - 50.0);
}

double Unit::staticManaPerSecond() const
{
    return getMp5() / 5.0;
}

double Unit::manaPerSecond(const State&) const
{
    return staticManaPerSecond();
}

double Unit::maxMana() const
{
    // Subtract 280 because the first 20 intellect only gives 1 mana instead of 15
    return base_mana + stats.intellect * 15 - 280 + stats.mana;
}

double Unit::manaPercent() const
{
    return mana / maxMana() * 100.0;
}

double Unit::manaPerTick(const State& state) const
{
    return manaPerSecond(state) * MANA_TICK_T;
}

double Unit::gcd(double t) const
{
    return t;
}

double Unit::baseCastTime(std::shared_ptr<spell::Spell> spell) const
{
    return spell->cast_time;
}

double Unit::castTime(std::shared_ptr<spell::Spell> spell) const
{
    double t = baseCastTime(spell);
    if (!spell->channeling)
        t*= castHaste();
    return t;
}

double Unit::castHaste() const
{
    return 1.0;
}

double Unit::travelTime(std::shared_ptr<spell::Spell> spell)
{
    if (!spell->speed)
        return 0;

    return std::max(config.distance / spell->speed, 0.0);
}

double Unit::hitChance(std::shared_ptr<spell::Spell>, double dlevel) const
{
    return stats.hit + buff_stats.hit;
}

double Unit::critChance(std::shared_ptr<spell::Spell>) const
{
    return stats.crit + buff_stats.crit;
}

bool Unit::canCast(std::shared_ptr<spell::Spell> spell) const
{
    return mana >= manaCost(spell);
}

bool Unit::canBuff(const buff::Buff &buff) const
{
    return mana >= manaCost(buff);
}

bool Unit::canMiss(std::shared_ptr<spell::Spell> spell) const
{
    return !spell->dot && !spell->max_heal && !spell->fixed_value;
}

bool Unit::canCrit(std::shared_ptr<spell::Spell> spell) const
{
    return !spell->dot && !spell->fixed_value;
}

bool Unit::canResist(std::shared_ptr<spell::Spell> spell) const
{
    return !spell->dot;
}

double Unit::baseCritMultiplier(std::shared_ptr<spell::Spell> spell) const
{
    return 1;
}

double Unit::critMultiplierMod(std::shared_ptr<spell::Spell> spell) const
{
    return 1;
}

double Unit::spellCoeffMod(std::shared_ptr<spell::Spell> spell) const
{
    return 0;
}

double Unit::buffHealMultiplier(std::shared_ptr<spell::Spell>, const State&) const
{
    return 1;
}

double Unit::buffDmgMultiplier(std::shared_ptr<spell::Spell>, const State&) const
{
    return 1;
}

double Unit::baseManaCost(std::shared_ptr<spell::Spell> spell) const
{
    if (spell->cost_base_mana)
        return base_mana * spell->cost / 100.0;

    return spell->cost;
}

double Unit::baseManaCost(const buff::Buff& buff) const
{
    return buff.cost;
}

double Unit::manaCost(std::shared_ptr<spell::Spell> spell) const
{
    double cost = floor(baseManaCost(spell) * manaCostMultiplier(spell));

    cost+= manaCostMod(spell, cost);

    return cost;
}

double Unit::manaCost(const buff::Buff& buff) const
{
    double cost = baseManaCost(buff);

    return round(cost);
}

double Unit::manaCostMod(std::shared_ptr<spell::Spell> spell, double mana_cost) const
{
    return 0;
}

double Unit::manaCostMultiplier(std::shared_ptr<spell::Spell> spell) const
{
    return 1;
}

double Unit::getIntellect() const
{
    return stats.intellect + buff_stats.intellect;
}

double Unit::getSpirit() const
{
    return stats.spirit + buff_stats.spirit;
}

double Unit::getSpellPower(School school, std::shared_ptr<target::Target> target) const
{
    double sp = stats.sp + buff_stats.sp;

    if (school == SCHOOL_ARCANE)
        sp+= stats.sp_arcane + buff_stats.sp_arcane;
    else if (school == SCHOOL_FIRE)
        sp+= stats.sp_fire + buff_stats.sp_fire;
    else if (school == SCHOOL_FROST)
        sp+= stats.sp_frost + buff_stats.sp_frost;
    else if (school == SCHOOL_SPELLFIRE)
        sp+= std::max(stats.sp_arcane + buff_stats.sp_arcane, stats.sp_fire + buff_stats.sp_fire);
    else if (school == SCHOOL_FROSTFIRE)
        sp+= std::max(stats.sp_frost + buff_stats.sp_frost, stats.sp_fire + buff_stats.sp_fire);
    else if (school == SCHOOL_SPELLFROST)
        sp+= std::max(stats.sp_arcane + buff_stats.sp_arcane, stats.sp_frost + buff_stats.sp_frost);

    if (get_raid_buffs) {
        if (config.demonic_pact && config.demonic_pact_bonus > 0)
            sp += config.demonic_pact_bonus;
    }

    if (target != NULL) {
        sp += target->getSpellPower(school);
    }

    return sp;
}

double Unit::getSpellPenetration(School school) const
{
    return stats.spen + buff_stats.spen;
}

double Unit::getMp5() const
{
    return stats.mp5 + buff_stats.mp5;
}

double Unit::cooldownMod(const cooldown::Cooldown &cooldown) const
{
    return 0;
}

void Unit::interrupt()
{
    is_channeling = false;
}

std::vector<action::Action> Unit::onBuffGain(const State &state, std::shared_ptr<buff::Buff> buff)
{
    return {};
}

std::vector<action::Action> Unit::onBuffExpire(const State &state, std::shared_ptr<buff::Buff> buff)
{
    return {};
}

std::vector<action::Action> Unit::onCastStartProc(const State &state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    return {};
}

std::vector<action::Action> Unit::onCastSuccessProc(const State &state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    return {};
}

std::vector<action::Action> Unit::onSpellImpactProc(const State &state, const spell::SpellInstance &instance, std::shared_ptr<target::Target> target)
{
    return {};
}

std::vector<action::Action> Unit::onSpellTickProc(const State &state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, int tick)
{
    return {};
}

std::vector<action::Action> Unit::onProcSuccess(const State &state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    return {};
}

std::vector<action::Action> Unit::usePotion()
{
    return {};
}

std::vector<action::Action> Unit::useTrinket(Trinket trinket, std::shared_ptr<cooldown::Cooldown> cooldown)
{
    return {};
}

action::Action Unit::buffAction(std::shared_ptr<buff::Buff> buff, bool primary_action) const
{
    action::Action action{ action::TYPE_BUFF };
    action.buff = buff;
    action.primary_action = primary_action;
    return action;
}

action::Action Unit::buffExpireAction(std::shared_ptr<buff::Buff> buff, bool primary_action) const
{
    action::Action action{ action::TYPE_BUFF_EXPIRE };
    action.buff = buff;
    action.primary_action = primary_action;
    return action;
}

action::Action Unit::cooldownAction(std::shared_ptr<cooldown::Cooldown> cooldown) const
{
    action::Action action{ action::TYPE_COOLDOWN };
    action.cooldown = cooldown;
    return action;
}

action::Action Unit::spellAction(std::shared_ptr<spell::Spell> spell) const
{
    action::Action action{ action::TYPE_SPELL };
    action.spell = spell;
    return action;
}

action::Action Unit::spellAction(std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target) const
{
    action::Action action{ action::TYPE_SPELL };
    action.spell = spell;
    action.target = target;
    return action;
}

action::Action Unit::manaAction(double mana, const std::string &str) const
{
    action::Action action{ action::TYPE_MANA };
    action.value = mana;
    action.str = str;
    return action;
}

action::Action Unit::waitAction(double t, const std::string &str) const
{
    action::Action action{ action::TYPE_WAIT };
    action.value = t;
    action.str = str;
    return action;
}

action::Action Unit::Unit::gcdAction(double t)
{
    if (t < t_gcd) {
        action::Action action{ action::TYPE_WAIT };
        action.value = t_gcd - t;
        action.str = "GCD";

        if (last_spell_cast_time > 0)
            t_gcd_capped += action.value;

        return action;
    }

    return { action::TYPE_NONE };
}

action::Action Unit::nextAction(const State &state)
{
    return { action::TYPE_NONE };
}

}