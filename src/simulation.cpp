#include "common.h"
#include "spell.h"
#include "buff.h"
#include "debuff.h"
#include "cooldown.h"
#include "unit.h"
#include "target.h"
#include "log.h"
#include "player.h"
#include "state.h"
#include "config.h"
#include "simulation.h"

#include <string>
#include <memory>
#include <vector>
#include <iomanip>
#include <map>
#include <unordered_map>
#include <sstream>
#include <algorithm>

Simulation::Simulation(const Config& _config, std::shared_ptr<unit::Player> _player)
    : config(_config), player(_player), state(_config)
{
    for (int i=1; i<=config.targets; i++)
        state.addTarget(i);
}

void Simulation::reset()
{
    clearLog();
    state.reset();
    player->reset();
}

SimulationsResult Simulation::runMultiple(int iterations)
{
    SimulationResult r;
    SimulationsResult result;

    logging = iterations == 1;
    logging_spells = true;

    constexpr double bin_size = 5;

    std::unordered_map<spell::ID, SpellStats> spells;
    std::unordered_map<int, int> histogram;
    std::string results{ "" };

    if (config.additional_data)
        results += "DPS,Duration\n";

    for (int i = 0; i < iterations; i++) {
        if (config.rng_seed)
            setRNGSeed(config.rng_seed + i);

        r = run();

        if (i == 0 || r.dps < result.min_dps)
            result.min_dps = r.dps;
        if (i == 0 || r.dps > result.max_dps)
            result.max_dps = r.dps;
        result.avg_dps += (r.dps - result.avg_dps) / (i + 1);

        if (i == 0 || r.hps < result.min_hps)
            result.min_hps = r.hps;
        if (i == 0 || r.hps > result.max_hps)
            result.max_hps = r.hps;
        result.avg_hps += (r.hps - result.avg_hps) / (i + 1);

        result.t_gcd_capped += (r.t_gcd_capped - result.t_gcd_capped) / (i + 1);

        if (r.t_oom)
            result.n_oom++;

        auto const bin = static_cast<int>(floor(r.dps / bin_size) * bin_size);
        if (histogram.find(bin) != histogram.end())
            histogram[bin]++;
        else
            histogram[bin] = 1;

        if (logging_spells) {
            for (auto itr = state.spells.begin(); itr != state.spells.end(); itr++) {
                if (spells.find(itr->first) == spells.end()) {
                    spells[itr->first].name = itr->second.name;
                    spells[itr->first].source = itr->second.source;
                    spells[itr->first].min_dmg = itr->second.min_dmg;
                    spells[itr->first].max_dmg = itr->second.max_dmg;
                }
                spells[itr->first].casts += (itr->second.casts - spells[itr->first].casts) / (i+1.0);
                spells[itr->first].misses += (itr->second.misses - spells[itr->first].misses) / (i+1.0);
                spells[itr->first].hits += (itr->second.hits - spells[itr->first].hits) / (i+1.0);
                spells[itr->first].crits += (itr->second.crits - spells[itr->first].crits) / (i+1.0);
                spells[itr->first].dmg += (itr->second.dmg - spells[itr->first].dmg) / (i+1.0);
                if (itr->second.max_dmg > spells[itr->first].max_dmg)
                    spells[itr->first].max_dmg = itr->second.max_dmg;
                if (itr->second.min_dmg < spells[itr->first].min_dmg)
                    spells[itr->first].min_dmg = itr->second.min_dmg;
            }
        }

        if (config.additional_data)
            results += std::to_string(r.dps) + "," + std::to_string(r.t) + "," + std::to_string(r.hps) + "\n";
    }

    result.iterations = iterations;

    if (config.additional_data)
        result.all_results = std::move(results);

    if (logging_spells)
        result.spells = spellStats(spells);

    // Histogram json string
    std::string ss{ "{" };
    for (auto itr = histogram.begin(); itr != histogram.end(); itr++) {
        if (itr != histogram.begin())
            ss += ",";
        ss += "\"" + std::to_string(itr->first) + "\":" + std::to_string(itr->second);
    }
    ss += "}";
    result.histogram = std::move(ss);

    return result;
}

SimulationResult Simulation::run(bool single)
{
    if (single && config.rng_seed)
        setRNGSeed(config.rng_seed);

    reset();

    runPrecombat();

    onManaRegen(player);

    if (config.mana_spring && config.player_level > 25 && player->faction() == FACTION_HORDE) {
        double m = 0;
        if (config.player_level >= 56)
            m = 10;
        else if (config.player_level >= 46)
            m = 8;
        else if (config.player_level >= 36)
            m = 6;
        else if (config.player_level >= 26)
            m = 4;

        if (config.imp_mana_spring)
            m = std::round(m * 1.25);
        
        for (double t = 0; t<state.duration; t+= 2)
            pushManaGain(player, t, m, "Mana Spring");
    }

    for (auto &timing : config.timings)
    {
        if (timing.name == "innervate")
            pushBuffGain(player, std::make_shared<buff::Innervate>(), timing.t);
        else if (timing.name == "power_infusion")
            pushBuffGain(player, std::make_shared<buff::PowerInfusion>(), timing.t);
        else if (timing.name == "mana_tide")
            pushBuffGain(player, std::make_shared<buff::ManaTide>(), timing.t);
    }

    if (config.scorching_mages > 0) {
        int n = 0;
        int p = player->talents.imp_scorch;
        if (p == 0) {
            if (config.player_level == 25)
                p = 1;
            else
                p = 3;
        }
        for (double t=1.5; t<state.duration;) {
            if (n < 5) {
                for (int i=0; i<config.scorching_mages; i++) {
                    if (p == 3 || random<int>(0, 2) < p) {
                        pushDebuffGain(state.targets[0], std::make_shared<debuff::ImprovedScorch>(), t);
                        n++;
                        if (n == 5)
                            break;
                    }
                }
            }
            else {
                pushDebuffGain(state.targets[0], std::make_shared<debuff::ImprovedScorch>(), t);
            }

            if (n < 5)
                t+= 1.5;
            else
                t+= 20.0;
        }
    }

    if (config.winters_chill_mages > 0) {
        int n = 0;
        int p = player->talents.winters_chill;
        if (p == 0)
            p = 5;
        // Assume ffb mages
        for (double t=3.0; t<state.duration;) {
            if (n < 5) {
                for (int i=0; i<config.winters_chill_mages; i++) {
                    if (p == 5 || random<int>(0, 4) < p) {
                        pushDebuffGain(state.targets[0], std::make_shared<debuff::WintersChill>(), t);
                        n++;
                        if (n == 5)
                            break;
                    }
                }
            }
            else {
                pushDebuffGain(state.targets[0], std::make_shared<debuff::WintersChill>(), t);
            }

            if (n < 5)
                t+= 3.0;
            else
                t+= 10.0;
        }
    }

    // Let's no deal with interruptions with APL
    if (config.rotation != ROTATION_APL) {
        for (int i=0; i<config.interruptions.size(); i++) {
            pushInterruption(i);
        }
    }

    workCurrent();

    if (!state.isSilenced() && !player->shouldPreCast())
        nextAction(player);

    work();

    SimulationResult result;
    result.t = state.t;
    result.dmg = config.only_main_dmg ? state.mainDmg() : state.totalDmg();
    result.dps = result.dmg / state.t;
    result.heal = state.healed;
    result.hps = result.heal / state.t;
    result.t_gcd_capped = player->t_gcd_capped;
    result.t_oom = player->t_oom;

    if (logging) {
        result.log = jsonLog();
        result.spells = spellStats(state.spells);
    }

    return result;
}

void Simulation::runPrecombat()
{
    state.t = player->t_gcd = 0.0 - player->preCombatDuration(state);

    if (state.t >= 0)
        return;

    while (nextAction(player)) {
        workCurrent();

        if (player->t_gcd != state.t)
            state.t = player->t_gcd;

        if (state.t >= 0)
            break;
    }

    state.t = player->t_gcd = 0;
}

void Simulation::workCurrent()
{
    while (true)
    {
        auto event = queue.front();
        if (event.t != state.t)
            return;
        queue.pop_front();
        tick(event);
    }
}

void Simulation::work()
{
    while (true)
    {
        auto event = queue.front();
        queue.pop_front();

        if (event.t > state.duration)
        {
            state.t = state.duration;
            break;
        }

        tick(event);
    }

    queue.clear();
}

void Simulation::tick(Event& event)
{
    state.t = event.t;

    switch (event.type)
    {
        case EVENT_NONE:
        case EVENT_DOT:
            break;
        case EVENT_CAST_START:
            cast(event.unit, event.spell, event.target);
            break;
        case EVENT_CAST_FINISH:
            onCastFinish(event.unit, event.spell, event.target);
            break;
        case EVENT_SPELL_IMPACT:
            onSpellImpact(event.unit, event.instance, event.target);
            break;
        case EVENT_SPELL_TICK:
            onSpellTick(event.unit, event.spell, event.target, event.tick);
            break;
        case EVENT_MANA_REGEN:
            onManaRegen(event.unit);
            break;
        case EVENT_MANA_GAIN:
            onManaGain(event.unit, event.mana, event.source);
            break;
        case EVENT_BUFF_GAIN:
            onBuffGain(event.unit, event.buff);
            break;
        case EVENT_BUFF_EXPIRE:
            onBuffExpire(event.unit, event.buff);
            break;
        case EVENT_BUFF_GAIN_ALL:
            onBuffGainAll(event.buff);
            break;
        case EVENT_DEBUFF_GAIN:
            onDebuffGain(event.target, event.debuff);
            break;
        case EVENT_DEBUFF_EXPIRE:
            onDebuffExpire(event.target, event.debuff);
            break;
        case EVENT_CD_GAIN:
            onCooldownGain(event.unit, event.cooldown);
            break;
        case EVENT_CD_EXPIRE:
            onCooldownExpire(event.unit, event.cooldown);
            break;
        case EVENT_UNIT:
            onUnitSpawn(event.unit);
            break;
        case EVENT_UNIT_DESPAWN:
            onUnitDespawn(event.unit);
            break;
        case EVENT_WAIT:
            onWait(event.unit, event.spell, event.target);
            break;
        case EVENT_INTERRUPTION:
            onInterruption(event.interruption_index);
            break;
        case EVENT_INTERRUPTION_END:
            onInterruptionEnd(event.interruption_index);
            break;
    }
}

void Simulation::push(Event& event)
{
    event.t += state.t;

    for (auto itr = queue.begin(); itr != queue.end(); itr++) {
        if (event.t < itr->t) {
            queue.insert(itr, event);
            return;
        }
    }

    queue.push_back(event);
}

void Simulation::pushCastStart(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, double t)
{
    Event event;
    event.type = EVENT_CAST_START;
    event.spell = spell;
    event.unit = unit;
    event.target = target;
    event.t = t;

    push(event);
}

void Simulation::pushCastFinish(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, double t)
{
    Event event;
    event.type = EVENT_CAST_FINISH;
    event.spell = spell;
    event.unit = unit;
    event.target = target;
    event.t = t;

    push(event);
}

void Simulation::pushSpellImpact(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, double t)
{
    Event event;
    event.type = EVENT_SPELL_IMPACT;
    event.instance = getSpellInstance(unit, spell, target);
    event.unit = unit;
    event.target = target;
    event.t = t;

    push(event);
}

void Simulation::pushSpellImpact(std::shared_ptr<unit::Unit> unit, spell::SpellInstance &instance, std::shared_ptr<target::Target> target, double t)
{
    Event event;
    event.type = EVENT_SPELL_IMPACT;
    event.instance = instance;
    event.unit = unit;
    event.target = target;
    event.t = t;

    push(event);
}

void Simulation::pushChannelingTick(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, double t, int tick)
{
    Event event;
    event.type = EVENT_SPELL_TICK;
    event.spell = spell;
    event.unit = unit;
    event.target = target;
    event.t = t;
    event.tick = tick;

    push(event);
}

void Simulation::pushDot(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, int tick, double t_offset)
{
    spell::SpellInstance instance;

    if (spell->dynamic) {
        instance.spell = spell;
        instance.result = spell::PENDING;
    }
    else {
        instance = getSpellInstance(unit, spell, target);
    }

    instance.tick = tick;

    Event event;
    event.type = EVENT_SPELL_IMPACT;
    event.instance = instance;
    event.unit = unit;
    event.target = target;
    event.t = tick * spell->t_interval + t_offset;

    push(event);
}

void Simulation::pushDotTick(std::shared_ptr<unit::Unit> unit, spell::SpellInstance &instance, std::shared_ptr<target::Target> target)
{
    Event event;
    event.type = EVENT_SPELL_IMPACT;
    event.instance = instance;
    event.unit = unit;
    event.target = target;
    event.t = instance.tick * instance.spell->t_interval;

    push(event);
}

void Simulation::pushManaRegen(std::shared_ptr<unit::Unit> unit)
{
    Event event;
    event.type = EVENT_MANA_REGEN;
    event.unit = unit;
    event.t = MANA_TICK_T;
    push(event);
}

void Simulation::pushManaGain(std::shared_ptr<unit::Unit> unit, double t, double mana, const std::string &source)
{
    Event event;
    event.type = EVENT_MANA_GAIN;
    event.t = t;
    event.unit = unit;
    event.mana = mana;
    event.source = source;

    push(event);
}

void Simulation::pushBuffGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff, double t)
{
    Event event;
    event.type = EVENT_BUFF_GAIN;
    event.t = t;
    event.unit = unit;
    event.buff = buff;

    push(event);
}

void Simulation::pushBuffGainAll(std::shared_ptr<buff::Buff> buff, double t)
{
    Event event;
    event.type = EVENT_BUFF_GAIN_ALL;
    event.t = t;
    event.buff = buff;

    push(event);
}

void Simulation::pushBuffExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff)
{
    Event event;
    event.type = EVENT_BUFF_EXPIRE;
    event.t = buff->duration;
    event.unit = unit;
    event.buff = buff;

    push(event);
}

void Simulation::pushDebuffGain(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff, double t)
{
    Event event;
    event.type = EVENT_DEBUFF_GAIN;
    event.t = t;
    event.target = target;
    event.debuff = debuff;

    push(event);
}

void Simulation::pushDebuffExpire(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff)
{
    Event event;
    event.type = EVENT_DEBUFF_EXPIRE;
    event.t = debuff->duration;
    event.target = target;
    event.debuff = debuff;

    push(event);
}

void Simulation::pushCooldownGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<cooldown::Cooldown> cooldown, double t)
{
    Event event;
    event.type = EVENT_CD_GAIN;
    event.t = t;
    event.unit = unit;
    event.cooldown = cooldown;

    push(event);
}

void Simulation::pushCooldownExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<cooldown::Cooldown> cooldown)
{
    Event event;
    event.type = EVENT_CD_EXPIRE;
    event.t = cooldown->duration;
    event.unit = unit;
    event.cooldown = cooldown;

    push(event);
}

void Simulation::pushUnitDespawn(std::shared_ptr<unit::Unit> unit, double t)
{
    Event event;
    event.type = EVENT_UNIT_DESPAWN;
    event.t = t;
    event.unit = unit;

    push(event);
}

void Simulation::pushWait(std::shared_ptr<unit::Unit> unit, double t, const std::string &str, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    Event event;
    event.type = EVENT_WAIT;
    event.unit = unit;
    event.t = t;
    event.spell = spell;
    event.target = target;

    push(event);

    if (!str.empty())
        addLog(unit, LOG_WAIT, str + ", " + unit->name + " waiting " + std::to_string(t) + " seconds...");
}

void Simulation::pushInterruption(int index)
{
    Event start;
    start.type = EVENT_INTERRUPTION;
    start.t = config.interruptions[index].t;
    start.interruption_index = index;
    push(start);

    Event end;
    end.type = EVENT_INTERRUPTION_END;
    end.t = config.interruptions[index].t + config.interruptions[index].duration;
    end.interruption_index = index;
    push(end);
}

void Simulation::onAction(std::shared_ptr<unit::Unit> unit, action::Action &action)
{
    if (action.type == action::TYPE_WAIT) {
        pushWait(unit, action.value, action.str, action.spell, action.target);
    }
    else if (action.type == action::TYPE_SPELL) {
        cast(unit, action.spell, action.target);
        if (action.cooldown)
            onCooldownGain(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_BUFF) {
        onBuffGain(unit, action.buff);
        if (action.cooldown)
            onCooldownGain(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_BUFF_EXPIRE) {
        onBuffExpire(unit, action.buff);
        if (action.cooldown)
            onCooldownGain(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_DEBUFF) {
        onDebuffGain(action.target, action.debuff);
        if (action.cooldown)
            onCooldownGain(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_COOLDOWN) {
        onCooldownGain(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_COOLDOWN_EXPIRE) {
        onCooldownExpire(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_POTION) {
        usePotion(unit);
    }
    else if (action.type == action::TYPE_TRINKET) {
        useTrinket(unit, action.trinket, action.cooldown);
    }
    else if (action.type == action::TYPE_MANA) {
        onManaGain(unit, action.value, action.str);
        if (action.cooldown)
            onCooldownGain(unit, action.cooldown);
    }
    else if (action.type == action::TYPE_UNIT) {
        onUnitSpawn(action.unit);
    }
    if (action.primary_action)
        nextAction(unit);
}

bool Simulation::nextAction(std::shared_ptr<unit::Unit> unit)
{
    auto action = unit->nextAction(state);

    if (action.type == action::TYPE_NONE)
        return false;

    onAction(unit, action);

    return true;
}

void Simulation::processActions(std::shared_ptr<unit::Unit> unit, std::vector<action::Action> &actions)
{
    for (auto & i : actions)
        onAction(unit, i);
}

void Simulation::cast(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    if (unit->canCast(spell)) {
        if (spell->active_use && !spell->off_gcd && unit->t_gcd > state.t)
            pushWait(unit, unit->t_gcd - state.t, "GCD", spell, target);
        else
            onCastStart(unit, spell, target);
    }
    else {
        if (!unit->t_oom)
            unit->t_oom = state.t;
        pushWait(unit, 0.5, "Out of mana");
    }
}

void Simulation::onCastStart(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    logCastStart(unit, spell, target);
    onCastStartProc(unit, spell, target);

    spell->actual_cast_time = unit->castTime(spell);

    if (spell->active_use) {
        unit->t_gcd = state.t + unit->gcd(spell->gcd);
        if (spell->gcd)
            unit->last_spell_cast_time = spell->actual_cast_time;
    }

    if (spell->channeling)
        onCastSuccess(unit, spell, target);
    else
        pushCastFinish(unit, spell, target, spell->actual_cast_time);
}

void Simulation::onCastFinish(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    if (spell->tick || unit->canCast(spell))
        onCastSuccess(unit, spell, target);
    else
        nextAction(unit);
}

void Simulation::onCastSuccess(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    spell->actual_cost = unit->manaCost(spell);
    unit->applyMana(state, -spell->actual_cost);

    if (spell->dot && spell->active_use && random<double>(0, 100) > hitChance(unit, spell, target)) {
        logCastMiss(unit, spell, target);
    }
    else {
        logCastSuccess(unit, spell, target);

        if (spell->channeling && !spell->tick)
            unit->is_channeling = true;

        if (spell->aoe) {
            for (auto const& tar : state.targets) {
                if (spell->channeling && !spell->tick) {
                    for (int i = 1; i <= spell->ticks; i++)
                        pushChannelingTick(unit, spell, tar, spell->actual_cast_time / spell->ticks * i, i);
                }
                else if (spell->dot) {
                    dotApply(unit, spell, tar);
                }
                else if (!spell->is_trigger) {
                    pushSpellImpact(unit, spell, tar, unit->travelTime(spell));
                }
            }
        }
        else {
            if (spell->channeling && !spell->tick) {
                for (int i = 1; i <= spell->ticks; i++)
                    pushChannelingTick(unit, spell, target, spell->actual_cast_time / spell->ticks * i, i);
            }
            else if (spell->dot) {
                dotApply(unit, spell, target);
            }
            else if (!spell->is_trigger) {
                pushSpellImpact(unit, spell, target, unit->travelTime(spell));
            }
        }
    }

    if (spell->active_use)
        onCastSuccessProc(unit, spell, target);
    if (spell->proc)
        onProcSuccess(unit, spell, target);

    if (spell->active_use) {
        if (state.inCombat()) {
            if (spell->channeling && spell->actual_cast_time > 0)
                pushWait(unit, spell->actual_cast_time);
            else
                pushWait(unit, 0);
        }
    }
}

void Simulation::onSpellImpact(std::shared_ptr<unit::Unit> unit, spell::SpellInstance &instance, std::shared_ptr<target::Target> target)
{
    if (instance.result == spell::PENDING)
        rollSpellInstance(unit, instance, target);

    if (instance.spell->dot)
        instance.dmg*= debuffDmgMultiplier(unit, instance.spell, target);

    if (instance.dmg)
        target->dmg += static_cast<unsigned long long>(instance.dmg);
    if (instance.heal)
        state.healed += static_cast<unsigned long long>(instance.heal);

    logSpellImpact(unit, instance, target);
    onSpellImpactProc(unit, instance, target);

    // Log spell use
    if (logging_spells) {
        initSpellStats(unit, instance.spell);
        state.spells[instance.spell->id].casts++;
        if (instance.result == spell::MISS)
            state.spells[instance.spell->id].misses++;
        else if (instance.result == spell::CRIT)
            state.spells[instance.spell->id].crits++;
        else
            state.spells[instance.spell->id].hits++;

        if (instance.dmg) {
            state.spells[instance.spell->id].dmg += instance.dmg;
            if (instance.dmg > state.spells[instance.spell->id].max_dmg)
                state.spells[instance.spell->id].max_dmg = instance.dmg;
            if (instance.dmg < state.spells[instance.spell->id].min_dmg || state.spells[instance.spell->id].min_dmg == 0)
                state.spells[instance.spell->id].min_dmg = instance.dmg;
        }
    }
}

void Simulation::onSpellTick(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, int tick)
{
    if (!spell->is_trigger) {
        auto instance = getSpellInstance(unit, spell, target);
        instance.tick = tick;
        pushSpellImpact(unit, instance, target, unit->travelTime(spell));
    }

    onSpellTickProc(unit, spell, target, tick);
}

void Simulation::dotApply(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    // Ignite special case
    if (spell->id == spell::IGNITE) {
        double leftover = 0;

        // Remove pending ignite ticks
        for (auto i = queue.begin(); i != queue.end();) {
            if (i->type == EVENT_SPELL_IMPACT && i->instance.spell->id == spell->id && i->unit == unit && i->target == target) {
                leftover+= i->instance.spell->min_dmg;
                i = queue.erase(i);
            }
            else {
                i++;
            }
        }

        // Add leftover dmg to new ignite
        spell->min_dmg = spell->max_dmg = spell->min_dmg + round(leftover / 2.0);

        for (int i = 1; i <= spell->ticks; i++)
            pushDot(unit, spell, target, i);
    }
    else {
        // Remove all dot ticks and create new ones
        if (spell->tick_refresh) {
            removeSpellImpacts(unit, spell, target);

            for (int i = 1; i <= spell->ticks; i++)
                pushDot(unit, spell, target, i, unit->travelTime(spell));
        }
        else {

            // Find pending dot ticks
            double queued_ticks = 0;
            double last_tick = 0;
            for (auto i = queue.begin(); i != queue.end(); i++) {
                if (i->type == EVENT_SPELL_IMPACT && i->instance.spell->id == spell->id && i->unit == unit && i->target == target) {
                    queued_ticks++;
                    last_tick = i->t;
                }
            }

            // Tick relative to current time
            if (queued_ticks > 0)
                last_tick -= state.t;

            // Add dot ticks at the end
            for (int i = 1 + queued_ticks; i <= spell->ticks; i++)
                pushDot(unit, spell, target, i, unit->travelTime(spell) + last_tick - queued_ticks * spell->t_interval);
        }
    }
}

void Simulation::onCastStartProc(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    auto actions = unit->onCastStartProc(state, spell, target);
    processActions(unit, actions);
}

void Simulation::onCastSuccessProc(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    auto actions = unit->onCastSuccessProc(state, spell, target);
    processActions(unit, actions);
}

void Simulation::onSpellImpactProc(std::shared_ptr<unit::Unit> unit, const spell::SpellInstance &instance, std::shared_ptr<target::Target> target)
{
    std::vector<action::Action> actions = unit->onSpellImpactProc(state, instance, target);
    processActions(unit, actions);
}

void Simulation::onSpellTickProc(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, int tick)
{
    std::vector<action::Action> actions = unit->onSpellTickProc(state, spell, target, tick);
    processActions(unit, actions);
}

void Simulation::onProcSuccess(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    std::vector<action::Action> actions = unit->onProcSuccess(state, spell, target);
    processActions(unit, actions);
}

void Simulation::onUnitSpawn(std::shared_ptr<unit::Unit> unit)
{
    if (unit->unique)
        despawnDuplicate(unit);

    unit->reset();
    logUnitSpawn(unit);
    state.addUnit(unit);

    if (unit->duration)
        pushUnitDespawn(unit, unit->duration);

    if (state.inCombat())
        pushWait(unit, 0.25);
    else
        pushWait(unit, -state.t);
}

void Simulation::onUnitDespawn(std::shared_ptr<unit::Unit> unit)
{
    logUnitDespawn(unit);
    removeUnitEvents(unit);
    state.removeUnit(unit);
}

void Simulation::despawnDuplicate(std::shared_ptr<unit::Unit> unit)
{
    for (auto const& i : state.units)
        if (i->id == unit->id)
        {
            onUnitDespawn(i);
            break;
        }
}

void Simulation::onManaRegen(std::shared_ptr<unit::Unit> unit, bool next)
{
    onManaGain(unit, unit->manaPerTick(state), "Mana Regen");
    if (next)
        pushManaRegen(unit);
}

void Simulation::onManaGain(std::shared_ptr<unit::Unit> unit, double mana, const std::string &source)
{
    unit->applyMana(state, mana);
    logManaGain(unit, mana, source);

    // Evocation
    if (unit->id == player->id && unit->hasBuff(buff::EVOCATION) && unit->manaPercent() > 90.0)
        interrupt(unit);
}

void Simulation::onWait(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    if (unit->is_channeling) {
        unit->removeSnapshots();
        unit->is_channeling = false;
    }

    if (spell != NULL)
        cast(unit, spell, target);
    else
        nextAction(unit);
}

void Simulation::interrupt(std::shared_ptr<unit::Unit> unit)
{
    unit->interrupt();

    if (unit->hasBuff(buff::EVOCATION))
        onBuffExpire(unit, std::make_shared<buff::Evocation>());

    for (auto i = queue.begin(); i != queue.end();) {
        if ((i->type == EVENT_CAST_FINISH || i->type == EVENT_SPELL_TICK || i->type == EVENT_WAIT) && i->unit->id == unit->id)
            i = queue.erase(i);
        else
            ++i;
    }

    if (state.t > 0)
        nextAction(unit);
}

void Simulation::onInterruption(int index)
{
    logInterruption(&config.interruptions[index]);

    state.activateInterruption(index);

    player->interrupt();

    if (player->hasBuff(buff::EVOCATION))
        onBuffExpire(player, std::make_shared<buff::Evocation>());

    if (config.interruptions[index].affects_all) {
        for (auto i = state.units.begin(); i != state.units.end(); ++i)
            (*i)->interrupt();
    }

    for (auto i = queue.begin(); i != queue.end();) {
        if ((i->type == EVENT_CAST_FINISH || i->type == EVENT_SPELL_TICK || i->type == EVENT_WAIT) &&
            (i->unit->id == player->id || config.interruptions[index].affects_all))
        {
            i = queue.erase(i);
        }
        else {
            ++i;
        }
    }

    if (!config.interruptions[index].silence && state.t > 0) {
        nextAction(player);

        if (config.interruptions[index].affects_all) {
            for (auto i = state.units.begin(); i != state.units.end(); ++i)
                nextAction(*i);
        }
    }
}

void Simulation::onInterruptionEnd(int index)
{
    state.deactivateInterruption(index);

    if (config.interruptions[index].silence && state.t > 0 && !state.isSilenced()) {
        nextAction(player);

        if (config.interruptions[index].affects_all && !state.isSilenced(false)) {
            for (auto i = state.units.begin(); i != state.units.end(); ++i)
                nextAction(*i);
        }
    }
}

void Simulation::onBuffGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff)
{
    if (buff->cost) {
        if (!unit->canBuff(*buff))
            return;
        unit->applyMana(state, -unit->manaCost(*buff));
    }

    int old_stacks = unit->buffStacks(buff->id);

    if (old_stacks < 1)
        buff->t_gained = state.t;

    int stacks = unit->addBuff(buff);

    if (old_stacks < 1 || buff->stack_refresh) {
        buff->t_expires = state.t + buff->duration;
        removeBuffExpiration(unit, buff);
        pushBuffExpire(unit, buff);
    }

    if (buff->id == buff::MANA_TIDE) {
        for (double t = 3; t <= 12; t += 3)
            pushManaGain(unit, t, 170, "Mana Tide");
    }

    if (stacks != old_stacks || buff->show_refresh)
        logBuffGain(unit, buff, stacks);

    auto actions = unit->onBuffGain(state, buff);
    processActions(unit, actions);
}

void Simulation::onBuffExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff)
{
    bool snapshot = false;
    if (buff->snapshot && unit->is_channeling && buffDuration(unit, buff->id) > 0)
        snapshot = true;

    removeBuffExpiration(unit, buff);
    logBuffExpire(unit, buff);
    unit->removeBuff(buff->id, snapshot);

    auto actions = unit->onBuffExpire(state, buff);
    processActions(unit, actions);
}

void Simulation::onBuffGainAll(std::shared_ptr<buff::Buff> buff)
{
    onBuffGain(player, buff);

    for (auto itr = state.units.begin(); itr != state.units.end(); itr++) {
        if ((*itr)->get_raid_buffs)
            onBuffGain(*itr, buff);
    }
}

void Simulation::onDebuffGain(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff)
{
    debuff->t_expires = state.t + debuff->duration;
    int stacks = target->addDebuff(debuff);
    removeDebuffExpiration(target, debuff);
    pushDebuffExpire(target, debuff);

    if (stacks)
        logDebuffGain(target, debuff, stacks);
}

void Simulation::onDebuffExpire(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff)
{
    removeDebuffExpiration(target, debuff);
    logDebuffExpire(target, debuff);
    target->removeDebuff(debuff->id);
}

void Simulation::onCooldownGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<cooldown::Cooldown> cooldown, bool mod, bool trigger_shared)
{
    if (mod)
        cooldown->duration += unit->cooldownMod(*cooldown);

    cooldown->t_gained = state.t;
    unit->addCooldown(cooldown);

    if (cooldownDuration(unit, cooldown->id) < cooldown->duration) {
        cooldown->t_expires = state.t + cooldown->duration;
        removeCooldownExpiration(unit, *cooldown);
        pushCooldownExpire(unit, cooldown);
    }

    if (trigger_shared)
        triggerSharedCooldowns(unit, cooldown);
}

void Simulation::onCooldownExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<cooldown::Cooldown> cooldown)
{
    unit->removeCooldown(cooldown->id);
}

void Simulation::triggerSharedCooldowns(std::shared_ptr<unit::Unit> unit, std::shared_ptr<cooldown::Cooldown> cooldown)
{
    std::map<cooldown::ID, double> trinkets = {
        {cooldown::ESSENCE_OF_SAPPHIRON, 20},
        {cooldown::MQG, 20},
        {cooldown::UNSTABLE_POWER, 20},
        {cooldown::EPHEMERAL_POWER, 15},
        {cooldown::ARCANE_POTENCY, 20},
        {cooldown::OBSIDIAN_INSIGHT, 30},
        {cooldown::CHROMATIC_INFUSION, 15},
        {cooldown::UNRESTRAINED_POWER, 20},
        {cooldown::CHAOS_FIRE, 15},
    };

    auto entry = trinkets.find(cooldown->id);
    if (entry != trinkets.end()) {
        for (auto itr = trinkets.begin(); itr != trinkets.end(); itr++) {
            if (itr->first != cooldown->id)
                onCooldownGain(unit, std::make_shared<cooldown::Cooldown>(itr->first, entry->second), false, false);
        }
    }
}

void Simulation::usePotion(std::shared_ptr<unit::Unit> unit)
{
    std::vector<action::Action> actions = unit->usePotion();
    processActions(unit, actions);
}

void Simulation::useTrinket(std::shared_ptr<unit::Unit> unit, Trinket trinket, std::shared_ptr<cooldown::Cooldown> cooldown)
{
    std::vector<action::Action> actions = unit->useTrinket(trinket, cooldown);
    processActions(unit, actions);
}

void Simulation::removeSpellImpact(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    for (auto i = queue.begin(); i != queue.end(); ++i)
        if (i->type == EVENT_SPELL_IMPACT && i->instance.spell->id == spell->id && i->unit == unit && i->target == target)
        {
            queue.erase(i);
            return;
        }
}

void Simulation::removeSpellImpacts(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    for (auto i = queue.begin(); i != queue.end();) {
        if (i->type == EVENT_SPELL_IMPACT && i->instance.spell->id == spell->id && i->unit == unit && i->target == target)
            i = queue.erase(i);
        else
            ++i;
    }
}

void Simulation::removeBuffExpiration(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff)
{
    for (auto i = queue.begin(); i != queue.end(); ++i) {
        if (i->type == EVENT_BUFF_EXPIRE && i->buff->id == buff->id && i->unit == unit) {
            queue.erase(i);
            return;
        }
    }
}

void Simulation::removeDebuffExpiration(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff)
{
    for (auto i = queue.begin(); i != queue.end(); ++i) {
        if (i->type == EVENT_DEBUFF_EXPIRE && i->debuff->id == debuff->id && i->target == target) {
            queue.erase(i);
            return;
        }
    }
}

void Simulation::removeCooldownExpiration(std::shared_ptr<unit::Unit> unit, const cooldown::Cooldown &cooldown)
{
    for (auto i = queue.begin(); i != queue.end(); ++i) {
        if (i->type == EVENT_CD_EXPIRE && i->cooldown->id == cooldown.id && i->unit == unit) {
            queue.erase(i);
            return;
        }
    }
}

double Simulation::buffDuration(std::shared_ptr<unit::Unit> unit, buff::ID id) const
{
    for (auto const& i : queue)
        if (i.type == EVENT_BUFF_EXPIRE && i.buff->id == id && i.unit == unit)
            return i.t - state.t;

    return 0;
}

double Simulation::cooldownDuration(std::shared_ptr<unit::Unit> unit, cooldown::ID id) const
{
    for (auto const& i : queue)
        if (i.type == EVENT_CD_EXPIRE && i.cooldown->id == id && i.unit == unit)
            return i.t - state.t;

    return 0;
}

void Simulation::removeUnitEvents(std::shared_ptr<unit::Unit> unit)
{
    for (auto i = queue.begin(); i != queue.end();)
    {
        if (i->unit == unit && i->type != EVENT_SPELL_IMPACT)
            i = queue.erase(i);
        else
            ++i;
    }
}

double Simulation::hitChance(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target) const
{
    int dlevel = config.target_level - config.player_level;

    double hit = 96.0 - dlevel;

    if (dlevel > 2)
        hit -= (dlevel - 2) * 10.0;

    // Miss chance for binary spells
    // Based on targets non-level based resistance
    // Chance of resist is 75% of the resistScore/resistCap
    // https://royalgiraffe.github.io/resist-guide
    if (spell->binary)
        hit *= 1.0 - 0.75 * resistScore(unit, spell, false) / (config.target_level * 5.0);

    hit += unit->hitChance(spell);

    return std::min(hit, 99.0);
}

double Simulation::critChance(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target) const
{
    int dlevel = config.target_level - config.player_level;
    double crit = unit->critChance(spell);

    if (!unit->get_raid_debuffs)
        return crit;

    if (target->hasDebuff(debuff::WINTERS_CHILL) && spell->isSchool(SCHOOL_FROST) && spell->max_dmg > 0.0)
        crit += target->debuffStacks(debuff::WINTERS_CHILL) * 2.0;

    crit = std::min(crit, 100.0);

    // Crit suppression
    if (unit->crit_suppression && spell->max_dmg > 0.0) {
        if (dlevel == 3)
            crit -= 2.1;
        if (dlevel == 2)
            crit -= 0.3;
    }

    return crit;
}

double Simulation::critMultiplier(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell) const
{
    double base = 1.5;
    double multi = 1;

    base *= unit->baseCritMultiplier(spell);

    if (spell->proc)
        return base;

    multi *= unit->critMultiplierMod(spell);

    return (base - 1) * multi + 1;
}

double Simulation::buffHealMultiplier(const std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell) const
{
    double multi = 1;

    multi *= unit->buffHealMultiplier(spell, state);

    return multi;
}

double Simulation::buffDmgMultiplier(const std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell) const
{
    double multi = 1;

    multi *= unit->buffDmgMultiplier(spell, state);

    return multi;
}

double Simulation::debuffDmgMultiplier(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target) const
{
    if (!unit->get_raid_debuffs)
        return 1.0;

    std::vector<double> multiv;
    for (int i=SCHOOL_NONE; i != SCHOOL_PHYSICAL; i++)
        multiv.push_back(1);

    if (config.curse_of_shadow_eye) {
        multiv[SCHOOL_ARCANE] *= 1.1;
        multiv[SCHOOL_SHADOW] *= 1.1;
    }
    else if (config.curse_of_shadow) {
        if (config.player_level >= 56) {
            multiv[SCHOOL_ARCANE] *= 1.1;
            multiv[SCHOOL_SHADOW] *= 1.1;
        }
        else if (config.player_level >= 44) {
            multiv[SCHOOL_ARCANE] *= 1.08;
            multiv[SCHOOL_SHADOW] *= 1.08;
        }
    }

    if (config.curse_of_elements_eye) {
        multiv[SCHOOL_FIRE] *= 1.1;
        multiv[SCHOOL_FROST] *= 1.1;
    }
    else if (config.curse_of_elements) {
        if (config.player_level >= 56) {
            multiv[SCHOOL_FIRE] *= 1.1;
            multiv[SCHOOL_FROST] *= 1.1;
        }
        else if (config.player_level >= 44) {
            multiv[SCHOOL_FIRE] *= 1.08;
            multiv[SCHOOL_FROST] *= 1.08;
        }
        else if (config.player_level >= 32) {
            multiv[SCHOOL_FIRE] *= 1.06;
            multiv[SCHOOL_FROST] *= 1.06;
        }
    }

    // Doesnt stack with curses
    if (config.mekkatorques_arcano_shredder) {
        multiv[SCHOOL_ARCANE] = std::max(multiv[SCHOOL_ARCANE], 1.06);
        multiv[SCHOOL_FIRE] = std::max(multiv[SCHOOL_FIRE], 1.06);
        multiv[SCHOOL_FROST] = std::max(multiv[SCHOOL_FROST], 1.06);
        multiv[SCHOOL_SHADOW] = std::max(multiv[SCHOOL_SHADOW], 1.06);
        multiv[SCHOOL_NATURE] = std::max(multiv[SCHOOL_NATURE], 1.06);
        multiv[SCHOOL_HOLY] = std::max(multiv[SCHOOL_HOLY], 1.06);
    }

    if (target->hasDebuff(debuff::IMPROVED_SCORCH))
        multiv[SCHOOL_FIRE] *= 1 + (0.03 * target->debuffStacks(debuff::IMPROVED_SCORCH));

    // Find highest multiplier for spell school
    double multi_max = 1;
    for (int i=SCHOOL_NONE; i != SCHOOL_PHYSICAL; i++) {
        if (spell->isSchool(static_cast<School>(i)) && multiv[i] > multi_max)
            multi_max = multiv[i];
    }

    return multi_max;
}

double Simulation::spellDmg(const std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    double dmg;

    if (config.avg_spell_dmg)
        dmg = spell->avgDmg();
    else
        dmg = random<double>(spell->min_dmg, spell->max_dmg);

    if (spell->fixed_value)
        return dmg;

    if (spell->coeff) {
        auto const sp = unit->getSpellPower(spell->school, target);
        double coeff = spell->coeff + unit->spellCoeffMod(spell);

        dmg += sp * coeff;
    }

    dmg *= buffDmgMultiplier(unit, spell);

    return dmg;
}

double Simulation::spellHeal(const std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    double heal;

    if (config.avg_spell_dmg)
        heal = spell->avgHeal();
    else
        heal = random<double>(spell->min_heal, spell->max_heal);

    if (spell->fixed_value)
        return heal;

    if (spell->coeff) {
        auto const sp = unit->getSpellPower(spell->school);
        double coeff = spell->coeff + unit->spellCoeffMod(spell);

        heal += sp * coeff;
    }

    heal *= buffHealMultiplier(unit, spell);

    return heal;
}

/**
 * Source for resistance based mitigation
 * May not match exactly, but it is the best estimate we got
 * https://royalgiraffe.github.io/legacy-sim/
 */

double Simulation::spellDmgResist(std::shared_ptr<unit::Unit> unit, const spell::SpellInstance &instance)
{

    if (instance.spell->binary)
        return 0.0;

    double res_score = resistScore(unit, instance.spell, true);

    // Dots only use 10% of the resistance
    // But only if the dot has no initial damage (like fireball, pyroblast)
    // Unconfirmed for ignite/living bomb/living flame
    if (instance.spell->dot && (instance.spell->id != spell::FIREBALL_DOT || instance.spell->id != spell::PYROBLAST_DOT || instance.spell->id != spell::FROSTFIRE_BOLT_DOT))
        res_score*= 0.1;

    double cap = config.player_level * 5.0;
    double ratio = res_score / cap;
    int i = std::floor(ratio * 3.0);
    double frac = ratio * 3.0 - i;
    double percentages[4] = { 0, 0, 0, 0};

    double segments[4][4] = {
        { 100, 0, 0, 0},
        { 24, 55, 18, 3},
        { 0, 22, 56, 22 },
        { 0, 4, 16, 80 }
    };

    if (i >= 3) {
        for (int j=0; j<4; j++)
            percentages[j] = segments[3][j];
    }
    else {
        for (int j=0; j<4; j++)
            percentages[j] = std::round(segments[i][j] * (1.0 - frac) + segments[i+1][j] * frac);

        if (ratio < 2.0/3.0 - 0.000001)
            percentages[0] = std::max(1.0, percentages[0]);
    }

    int roll = random<int>(0, 99);
    double resistance_multiplier = 0.0;
    for (int i = 0; i < 4; i++) {
        if (roll < percentages[i]) {
            resistance_multiplier = ((float) i) * 0.25;
            break;
        }

        roll -= percentages[i];
    }

    // Debug resistance tables
    // printf("%.2f: Multi: %.2f -- score: %f -- ratio: %.2f\n", state.t, resistance_multiplier, res_score, ratio);
    // for (int j=0; j<4; j++)
    //     printf("%.2f: Resist %d = %f\n", state.t, j, percentages[j]);

    if (!resistance_multiplier)
        return 0.0;

    return round(instance.dmg * resistance_multiplier);
}

double Simulation::resistScore(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, bool level_based) const
{
    // Init target resistances
    std::vector<double> resistv;
    for (int i=SCHOOL_NONE; i != SCHOOL_PHYSICAL; i++)
        resistv.push_back(config.target_resistance - unit->getSpellPenetration(static_cast<School>(i)));

    // Apply target resistance debuffs (curses)
    if (config.curse_of_shadow_eye) {
        resistv[SCHOOL_ARCANE] -= 75.0;
        resistv[SCHOOL_SHADOW] -= 75.0;
    }
    else if (config.curse_of_shadow) {
        if (config.player_level >= 56) {
            resistv[SCHOOL_ARCANE] -= 75.0;
            resistv[SCHOOL_SHADOW] -= 75.0;
        }
        else if (config.player_level >= 44) {
            resistv[SCHOOL_ARCANE] -= 60.0;
            resistv[SCHOOL_SHADOW] -= 60.0;
        }
    }
    else if (config.mekkatorques_arcano_shredder) {
        resistv[SCHOOL_ARCANE] = std::min(resistv[SCHOOL_ARCANE], -45.0);
        resistv[SCHOOL_SHADOW] = std::min(resistv[SCHOOL_SHADOW], -45.0);
    }

    if (config.curse_of_elements_eye) {
        resistv[SCHOOL_FIRE] -= 75.0;
        resistv[SCHOOL_FROST] -= 75.0;
    }
    else if (config.curse_of_elements) {
        if (config.player_level >= 56) {
            resistv[SCHOOL_FIRE] -= 75.0;
            resistv[SCHOOL_FROST] -= 75.0;
        }
        else if (config.player_level >= 44) {
            resistv[SCHOOL_FIRE] -= 60.0;
            resistv[SCHOOL_FROST] -= 60.0;
        }
        else if (config.player_level >= 32) {
            resistv[SCHOOL_FIRE] -= 45.0;
            resistv[SCHOOL_FROST] -= 45.0;
        }
    }
    else if (config.mekkatorques_arcano_shredder) {
        resistv[SCHOOL_FIRE] = std::min(resistv[SCHOOL_FIRE], -45.0);
        resistv[SCHOOL_FROST] = std::min(resistv[SCHOOL_FROST], -45.0);
    }
    
    if (config.mekkatorques_arcano_shredder) {
        resistv[SCHOOL_NATURE] = std::min(resistv[SCHOOL_NATURE], -45.0);
        resistv[SCHOOL_HOLY] = std::min(resistv[SCHOOL_HOLY], -45.0);
    }

    // Find lowest resist for spell school
    double res_score = 50000;
    for (int i=SCHOOL_NONE; i != SCHOOL_PHYSICAL; i++) {
        if (spell->isSchool(static_cast<School>(i)) && resistv[i] < res_score)
            res_score = resistv[i];
    }

    // Minimum 0 resistance
    res_score = std::max(res_score, 0.0);

    // Level-based minimum resistance
    if (level_based && config.target_level > config.player_level) {
        double diff = config.target_level - config.player_level;

        // Not sure what to do here, the same source disagree's with itself
        // The guide says the level-based resistance is based on attacker level
        // The calculator (by the same author) uses a fixed value of 8.0
        // Let's go with the written guide for now
        res_score+= 2.0/15.0 * ((double) config.player_level) * diff;
        // res_score+= 8.0 * diff;
    }

    return res_score;
}

spell::Result Simulation::getSpellResult(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target) const
{
    if (unit->canMiss(spell) && random<double>(0, 100) > hitChance(unit, spell, target))
        return spell::MISS;

    if (unit->canCrit(spell) && random<double>(0, 100) <= critChance(unit, spell, target))
        return spell::CRIT;

    return spell::HIT;
}

spell::SpellInstance Simulation::getSpellInstance(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    spell::SpellInstance instance;
    instance.spell = spell;

    rollSpellInstance(unit, instance, target);

    return instance;
}

void Simulation::rollSpellInstance(std::shared_ptr<unit::Unit> unit, spell::SpellInstance& instance, std::shared_ptr<target::Target> target)
{
    if (instance.spell->max_dmg > 0) {
        instance.result = getSpellResult(unit, instance.spell, target);

        if (instance.result != spell::MISS) {
            instance.initial_dmg = instance.dmg = spellDmg(unit, instance.spell, target);

            if (!instance.spell->dot)
                instance.dmg *= debuffDmgMultiplier(unit, instance.spell, target);

            if (instance.result == spell::CRIT)
                instance.dmg *= critMultiplier(unit, instance.spell);

            if (unit->canResist(instance.spell) && !instance.spell->fixed_value) {
                instance.resist = spellDmgResist(unit, instance);
                instance.dmg -= instance.resist;
            }

            instance.dmg = round(instance.dmg);
        }
    }

    if (instance.spell->max_heal > 0) {
        instance.result = getSpellResult(unit, instance.spell, target);
        instance.heal = spellHeal(unit, instance.spell, target);

        if (instance.result == spell::CRIT)
            instance.heal *= critMultiplier(unit, instance.spell);

        instance.heal = round(instance.heal);
    }
}

void Simulation::logCastStart(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    if (!logging || !spell->active_use || !spell->cast_time)
        return;

    if (target)
        addLog(unit, LOG_CAST_START, unit->name + " started casting " + spell->name + " on "+target->name+".");
    else
        addLog(unit, LOG_CAST_START, unit->name + " started casting " + spell->name + ".");
}

void Simulation::logCastSuccess(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    if (!logging || !spell->active_use)
        return;

    if (target)
        addLog(unit, LOG_CAST_SUCCESS, unit->name + " successfully cast " + spell->name + " on "+target->name+".");
    else
        addLog(unit, LOG_CAST_SUCCESS, unit->name + " successfully cast " + spell->name + ".");
}

void Simulation::logCastMiss(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target)
{
    if (!logging || !spell->active_use)
        return;

    if (target)
        addLog(unit, LOG_CAST_SUCCESS, unit->name + "'s " + spell->name+" missed "+target->name+".");
    else
        addLog(unit, LOG_CAST_SUCCESS, unit->name + "'s " + spell->name+" missed.");
}

void Simulation::logSpellImpact(std::shared_ptr<unit::Unit> unit, const spell::SpellInstance &instance, std::shared_ptr<target::Target> target)
{
    if (!logging)
        return;

    std::string s = unit->name + "'s " + instance.spell->name;

    if (instance.spell->dot)
        s += " (dot)";

    if (instance.result == spell::MISS) {
        s += " missed";
    }
    else {
        if (instance.dmg) {
            if (instance.result == spell::CRIT)
                s += " crit for " + std::to_string(static_cast<unsigned int>(instance.dmg));
            else
                s += " hit for " + std::to_string(static_cast<unsigned int>(instance.dmg));
        }
        else if (instance.heal) {
            if (instance.result == spell::CRIT)
                s += " healed for *" + std::to_string(static_cast<unsigned int>(instance.heal)) + "*";
            else
                s += " healed for " + std::to_string(static_cast<unsigned int>(instance.heal));
        }
    }

    if (instance.resist)
        s += " (" + std::to_string(static_cast<unsigned int>(instance.resist)) + " resisted)";

    if (target)
        s += " on "+target->name;

    s += ".";

    addLog(unit, LOG_SPELL_IMPACT, s);
}

void Simulation::logBuffGain(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff, int stacks)
{
    if (!logging || buff->hidden)
        return;

    std::string s = unit->name + " gained " + buff->name;
    if (buff->max_stacks > 1)
        s += " (" + std::to_string(stacks) + ")";
    s += ".";

    addLog(unit, LOG_BUFF, s);
}

void Simulation::logBuffExpire(std::shared_ptr<unit::Unit> unit, std::shared_ptr<buff::Buff> buff)
{
    if (!logging || buff->hidden)
        return;

    addLog(unit, LOG_BUFF, unit->name + " lost " + buff->name + ".");
}

void Simulation::logDebuffGain(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff, int stacks)
{
    if (!logging || debuff->hidden)
        return;

    std::string s = target->name+" gained " + debuff->name;
    if (debuff->max_stacks > 1)
        s += " (" + std::to_string(stacks) + ")";
    s += ".";

    addLog(player, LOG_BUFF, s);
}

void Simulation::logDebuffExpire(std::shared_ptr<target::Target> target, std::shared_ptr<debuff::Debuff> debuff)
{
    if (!logging || debuff->hidden)
        return;

    addLog(player, LOG_BUFF, target->name+" lost " + debuff->name + ".");
}

void Simulation::logManaGain(std::shared_ptr<unit::Unit> unit, double mana, const std::string &source)
{
    if (!logging)
        return;

    std::string s = unit->name;

    if (mana < 0)
        s += " lost " + std::to_string(static_cast<unsigned int>(0 - mana));
    else
        s += " gained " + std::to_string(static_cast<unsigned int>(mana));

    s += " mana from " + source + ".";

    addLog(unit, LOG_MANA, s);
}

void Simulation::logUnitSpawn(std::shared_ptr<unit::Unit> unit)
{
    if (!logging)
        return;

    addLog(unit, LOG_UNIT, unit->name + " spawned");
}

void Simulation::logUnitDespawn(std::shared_ptr<unit::Unit> unit)
{
    if (!logging)
        return;

    addLog(unit, LOG_UNIT, unit->name + " despawned");
}

void Simulation::logInterruption(const Interruption* interruption)
{
    if (!logging)
        return;

    std::string s = "Interruption ";

    if (interruption->silence)
        s+= "(silence)";
    else
        s+= "(movement)";

    s+= " on player";

    if (interruption->affects_all)
        s+= " and pets";

    s+= " for "+std::to_string(static_cast<unsigned int>(interruption->duration))+" seconds.";

    addLog(player, LOG_INTERRUPTION, s);
}

std::string Simulation::jsonLog() const
{
    std::ostringstream s;

    s << "[";

    for (int i = 0; i < log.size(); i++) {
        if (i > 0)
            s << ",";
        s << "{";
        if (log[i].unit)
            s << "\"unit\":\"" << log[i].unit->name << "\"";
        else
            s << "\"unit\":\"Unknown\"";
        s << ",\"text\":\"" << log[i].text << "\"";
        s << ",\"t\":" << log[i].t;
        s << ",\"type\":" << log[i].type;
        s << ",\"dmg\":" << log[i].dmg;
        s << ",\"healed\":" << log[i].healed;
        s << ",\"mana\":" << log[i].mana;
        s << ",\"mana_percent\":" << log[i].mana_percent;
        s << "}";
    }

    s << "]";

    return s.str();
}

void Simulation::addLog(std::shared_ptr<unit::Unit> unit, LogType type, const std::string &text)
{
    if (!logging)
        return;

    log.emplace_back(type, unit, text, state.t, unit->mana, unit->manaPercent(), state.totalDmg(), state.healed);
}

void Simulation::clearLog()
{
    log.clear();
}

void Simulation::initSpellStats(std::shared_ptr<unit::Unit> unit, std::shared_ptr<spell::Spell> spell)
{
    if (state.spells.find(spell->id) == state.spells.end()) {
        state.spells[spell->id].name = spell->name;
        if (spell->dot)
            state.spells[spell->id].name += " (dot)";
        state.spells[spell->id].source = unit->name;
    }
}

std::string Simulation::spellStats(std::unordered_map<spell::ID, SpellStats> spells)
{
    std::string s{ "[" };

    for (auto itr = spells.begin(); itr != spells.end(); itr++) {
        if (itr != spells.begin())
            s += ",";
        s += "{";
        s += "\"id\":" + std::to_string(itr->first) + ",";
        s += "\"name\":\"" + itr->second.name + "\",";
        s += "\"source\":\"" + itr->second.source + "\",";
        s += "\"casts\":" + std::to_string(itr->second.casts) + ",";
        s += "\"misses\":" + std::to_string(itr->second.misses) + ",";
        s += "\"hits\":" + std::to_string(itr->second.hits) + ",";
        s += "\"crits\":" + std::to_string(itr->second.crits) + ",";
        s += "\"min_dmg\":" + std::to_string(itr->second.min_dmg) + ",";
        s += "\"max_dmg\":" + std::to_string(itr->second.max_dmg) + ",";
        s += "\"dmg\":" + std::to_string(itr->second.dmg);
        s += "}";
    }

    s += "]";

    return s;
}

