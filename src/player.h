#pragma once

#include "unit.h"
#include "talents.h"
#include "config.h"

#include <vector>

namespace unit
{
class Player : public Unit
{
private:
    std::vector<bool> used_timings;
    const Timing* getNextTiming(const std::string& name) const;
    void useTiming(const Timing* timing);

public:
    const Talents talents;
    const Runes runes;

    Race race = RACE_UNDEAD;

    int combustion;
    int fingers_of_frost;
    double t_flamestrike;
    double t_flamestrike_dr;
    double t_scorch;
    double t_mana_spent;
    double t_living_bomb;
    bool mana_ruby;
    bool mana_citrine;
    bool mana_jade;
    bool mana_agate;
    int ab_streak;
    bool has_pre_cast;

    Player(const Config& config, const Stats& stats, const Talents& talents, const Runes& runes);

    void reset();

    Stats getStats();

    void setStats(Stats _stats);

    Stats getSummonStats();

    void setSummonStats(Stats _stats);

    Talents getTalents();

    void setTalents(Talents _talents);

    Runes getRunes();

    void setRunes(Runes _runes);

    Faction faction() const;

    bool hasTrinket(Trinket trinket) const;
    bool hasTrinkets(Trinket trinket1, Trinket trinket2) const;

    void applyMana(const State& state, double _mana);

    double manaPerSecond(const State& state) const;

    double maxMana() const;

    double baseCastTime(std::shared_ptr<spell::Spell> spell) const;

    double castHaste() const;

    double hitChance(std::shared_ptr<spell::Spell> spell, double dlevel = 3) const;

    double critChance(std::shared_ptr<spell::Spell> spell) const;

    double critMultiplierMod(std::shared_ptr<spell::Spell> spell) const;

    double buffHealMultiplier(std::shared_ptr<spell::Spell> spell, const State& state) const;

    double buffDmgMultiplier(std::shared_ptr<spell::Spell> spell, const State& state) const;

    double manaCostMod(std::shared_ptr<spell::Spell> spell, double mana_cost) const;

    double manaCostMultiplier(std::shared_ptr<spell::Spell> spell) const;

    double getSpellPenetration(School school) const;

    bool shouldConsumeClearcast(std::shared_ptr<spell::Spell>  spell) const;

    bool isFrozen() const;

    bool hasChillEffect(std::shared_ptr<spell::Spell> spell) const;

    std::vector<action::Action> onBuffGain(const State& state, std::shared_ptr<buff::Buff> buff);

    std::vector<action::Action> onBuffExpire(const State& state, std::shared_ptr<buff::Buff> buff);

    std::vector<action::Action> onCastStartProc(const State& state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target);

    std::vector<action::Action> onCastSuccessProc(const State& state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target);

    std::vector<action::Action> onProcSuccess(const State& state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target);

    std::vector<action::Action> onSpellImpactProc(const State& state, const spell::SpellInstance &instance, std::shared_ptr<target::Target> target);

    std::vector<action::Action> onSpellTickProc(const State& state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, int tick);

    std::vector<action::Action> onCastOrTick(const State& state, std::shared_ptr<spell::Spell> spell, std::shared_ptr<target::Target> target, int tick = 0);

    bool hasManaGem() const;

    double manaGemMax() const;

    bool shouldUseManaGem(const State& state);

    bool shouldUseManaPotion(const State& state);

    bool shouldEvocate(const State& state);

    std::vector<action::Action> useManaGem();

    std::vector<action::Action> usePotion();

    bool isTrinketOnSharedCD(Trinket trinket) const;

    bool trinketSharesCD(Trinket trinket) const;

    bool isUseTrinket(Trinket trinket) const;

    bool isTimingReady(const Timing* timing, const State& state) const;

    bool useTimingIfPossible(const std::string& name, const State& state, bool expl = false);

    bool isTimingReadySoon(const std::string& name, const State& state, double t = 5);

    std::vector<action::Action> useTrinket(Trinket trinket, std::shared_ptr<cooldown::Cooldown> cooldown);

    action::Action useCooldown(const State& state);

    bool canBlast(const State& state) const;

    bool shouldPreCast() const;

    std::shared_ptr<spell::Spell> preCastSpell();

    double preCombatDuration(const State& state);

    action::Action preCombat(const State& state);

    action::Action nextAction(const State& state);
};
}