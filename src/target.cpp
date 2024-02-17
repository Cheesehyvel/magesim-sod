#include "config.h"
#include "debuff.h"
#include "target.h"

#include <memory>
#include <string>

namespace target
{

Target::Target(const Config& _config, int _id)
    : config(_config), id(_id)
{
    name = "Target "+std::to_string(id);
}

void Target::reset()
{
    dmg = 0;
    t_ignite = -20;
    t_living_bomb = -20;
    t_pyroblast = -20;

    debuffs.clear();
}

int Target::debuffStacks(debuff::ID id) const
{
    auto const itr = debuffs.find(id);
    if (itr != debuffs.end())
        return itr->second->stacks;
    return 0;
}

double Target::debuffDuration(debuff::ID id, const State& state) const
{
    auto const itr = debuffs.find(id);
    if (itr == debuffs.end())
        return 0;

    return itr->second->t_expires - state.t;
}

bool Target::hasDebuff(debuff::ID id) const
{
    return debuffs.find(id) != debuffs.end();
}

int Target::addDebuff(std::shared_ptr<debuff::Debuff> debuff)
{
    if (hasDebuff(debuff->id)) {
        debuffs[debuff->id]->t_expires = debuff->t_expires;
        return debuffs[debuff->id]->addStack();
    }
    else {
        debuffs[debuff->id] = debuff;
    }

    return 1;
}

void Target::removeDebuff(debuff::ID id)
{
    debuffs.erase(id);
}

}