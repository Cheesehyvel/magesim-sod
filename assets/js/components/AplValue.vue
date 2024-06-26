<template>
    <div class="apl-value">
        <select v-model="modelValue.type" @change="changed">
            <option v-for="opt in typeOptions" :value="opt.value">{{ opt.title }}</option>
        </select>

        <input type="text" v-model.number="modelValue.value" v-if="type.input == 'number'" @input="changed">

        <select v-model.number="modelValue.id" @change="changed" v-if="idOptions.length" @input="changed">
            <option v-for="opt in idOptions" :value="opt.value">{{ opt.title }}</option>
        </select>
        <select v-model="modelValue.str" @change="changed" v-if="strOptions.length" @input="changed">
            <option v-for="opt in strOptions" :value="opt.value">{{ opt.title }}</option>
        </select>
    </div>
</template>

<script>
    import constants from "../constants";
    import items from "../items";

    export default {
        props: ["modelValue", "expect"],

        watch: {
            expect(new_value, old_value) {
                var opt = _.find(this.typeOptions, {value: this.modelValue.type});
                if (!opt) {
                    this.modelValue.type = constants.apl_value_types.VALUE_TYPE_NONE;
                    this.changed();
                }
            },
        },

        data() {
            var talent_options = [{value: "none", title: "None"}];
            for (var key in this.$root.config.talents)
                talent_options.push({value: key, title: _.startCase(key)});

            var rune_options = [{value: "none", title: "None"}];
            for (var key in this.$root.config.runes)
                rune_options.push({value: key, title: _.startCase(key)});

            return {
                type_options: [
                    { value: constants.apl_value_types.VALUE_TYPE_NONE, title: "No value", input: "none" },
                    { value: constants.apl_value_types.VALUE_TYPE_CONST, title: "Constant", input: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_PLAYER_MANA, title: "Player mana", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_PLAYER_MANA_PERCENT, title: "Player mana %", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_PLAYER_MANA_DEFICIT, title: "Player mana deficit", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_PLAYER_MANA_DEFICIT_PERCENT, title: "Player mana % deficit", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_TALENT_COUNT, title: "Talent points", input: "talents", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_RUNE_EXISTS, title: "Rune equipped", input: "runes", provides: "bool" },
                    { value: constants.apl_value_types.VALUE_TYPE_COOLDOWN_EXISTS, title: "Cooldown active", input: "cooldowns", provides: "bool" },
                    { value: constants.apl_value_types.VALUE_TYPE_COOLDOWN_REACT, title: "Cooldown active (w/ reaction time)", input: "cooldowns", provides: "bool" },
                    { value: constants.apl_value_types.VALUE_TYPE_COOLDOWN_DURATION, title: "Cooldown duration", input: "cooldowns", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_BUFF_EXISTS, title: "Aura active", input: "buffs", provides: "bool" },
                    { value: constants.apl_value_types.VALUE_TYPE_BUFF_REACT, title: "Aura active (w/ reaction time)", input: "buffs", provides: "bool" },
                    { value: constants.apl_value_types.VALUE_TYPE_BUFF_STACKS, title: "Aura stacks", input: "buffs", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_BUFF_DURATION, title: "Aura duration", input: "buffs", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_DEBUFF_EXISTS, title: "Debuff active", input: "debuffs", provides: "bool" },
                    { value: constants.apl_value_types.VALUE_TYPE_DEBUFF_STACKS, title: "Debuff stacks", input: "debuffs", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_DEBUFF_DURATION, title: "Debuff duration", input: "debuffs", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_STATE_TIME, title: "Current Time", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_STATE_TIME_PERCENT, title: "Current Time %", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_STATE_DURATION, title: "Remaining Duration", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_STATE_DURATION_PERCENT, title: "Remaining Duration %", provides: "number" },
                    // { value: constants.apl_value_types.VALUE_TYPE_STATE_IS_MOVING, title: "" },
                    // { value: constants.apl_value_types.VALUE_TYPE_STATE_IS_SILENCED, title: "" },
                    // { value: constants.apl_value_types.VALUE_TYPE_STATE_IS_INTERRUPTED, title: "" },
                    { value: constants.apl_value_types.VALUE_TYPE_SPELL_TRAVEL_TIME, title: "Spell travel time", input:"spells", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_SPELL_CAST_TIME, title: "Spell cast time", input:"spells", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_SPELL_TRAVEL_CAST_TIME, title: "Spell cast + travel time", input:"spells", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_SPELL_MANA_COST, title: "Spell mana cost", input:"spells", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_SPELL_CAN_CAST, title: "Can cast spell", input:"spells", provides: "bool" },
                    { value: constants.apl_value_types.VALUE_TYPE_CONFIG_DISTANCE, title: "Target distance", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_CONFIG_REACTION_TIME, title: "Reaction time", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_CONFIG_TARGET_LEVEL, title: "Target level", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_CONFIG_PLAYER_LEVEL, title: "Player level", provides: "number" },
                    { value: constants.apl_value_types.VALUE_TYPE_CONFIG_DIFF_LEVEL, title: "Level difference", provides: "number" },
                ],
                talent_options: talent_options,
                rune_options: rune_options,
            }
        },

        computed: {
            type() {
                return _.find(this.type_options, {value:this.modelValue.type});
            },

            typeOptions() {
                return this.type_options.filter((op) => {
                    if (op.input == "none")
                        return true;
                    if (!this.expect)
                        return false;
                    if (op.hasOwnProperty("provides"))
                        return this.expect == op.provides;
                    return this.expect == op.input;
                });
            },

            idOptions() {
                if (this.type.input == "cooldowns")
                    return this.cooldownOptions;
                if (this.type.input == "buffs")
                    return this.buffOptions;
                if (this.type.input == "debuffs")
                    return this.debuffOptions;
                if (this.type.input == "spells")
                    return this.spellOptions;

                return [];
            },

            strOptions() {
                if (this.type.input == "talents")
                    return this.talent_options;
                if (this.type.input == "runes")
                    return this.rune_options;
                return [];
            },

            cooldownOptions() {
                var options = [
                    { value: constants.cooldowns.NONE, title: "None" },
                    { value: constants.cooldowns.ARCANE_POWER, title: "Arcane Power", talent: "arcane_power" },
                    { value: constants.cooldowns.ARCANE_SURGE, title: "Arcane Surge", rune: "arcane_surge" },
                    { value: constants.cooldowns.BERSERKING, title: "Berserking", race: constants.races.RACE_TROLL },
                    { value: constants.cooldowns.BRAIN_FREEZE, title: "Brain Freeze", rune: "brain_freeze" },
                    { value: constants.cooldowns.BLAST_WAVE, title: "Blast Wave", talent: "blast_wave" },
                    { value: constants.cooldowns.CELESTIAL_ORB, title: "Celestial Orb", config: "item_celestial_orb" },
                    { value: constants.cooldowns.COLD_SNAP, title: "Cold Snap", talent: "cold_snap" },
                    { value: constants.cooldowns.COMBUSTION, title: "Combustion", talent: "combustion" },
                    { value: constants.cooldowns.CONE_OF_COLD, title: "Cone of Cold" },
                    { value: constants.cooldowns.DEEP_FREEZE, title: "Deep Freeze", rune: "deep_freeze" },
                    { value: constants.cooldowns.EVOCATION, title: "Evocation" },
                    { value: constants.cooldowns.FIRE_BLAST, title: "Fire Blast" },
                    { value: constants.cooldowns.ICY_VEINS, title: "Icy Veins", rune: "icy_veins" },
                    { value: constants.cooldowns.LIVING_FLAME, title: "Living Flame", rune: "living_flame" },
                    { value: constants.cooldowns.MANA_GEM, title: "Mana Gem" },
                    { value: constants.cooldowns.MILDLY_IRRADIATED_POTION, title: "Mildly Irradiated Rejuvenation Potion" },
                    { value: constants.cooldowns.POTION, title: "Potion" },
                    { value: constants.cooldowns.PRESENCE_OF_MIND, title: "Presence of Mind", talent: "presence_of_mind" },
                    { value: constants.cooldowns.ELECTROMAGNETIC_GIGAFLUX_REACTIVATOR, title: "Item: "+this.itemTitle("head", items.ids.ELECTROMAGNETIC_GIGAFLUX_REACTIVATOR), config: "item_electromagnetic_hyperflux_reactivator" },
                    { value: constants.cooldowns.ECHOES_OF_MADNESS, title: "Item: "+this.itemTitle("shoulder", items.ids.FRACTURED_MIND_PAULDRONS), config: "item_fractured_mind_pauldrons" },
                    { value: constants.cooldowns.CHARGED_INSPIRATION, title: "Item: "+this.itemTitle("head", items.ids.GNEURO_LINKED_MONOCLE), config: "item_gneuro_linked_monocle" },
                    { value: constants.cooldowns.COIN_FLIP, title: "Item: "+this.itemTitle("waist", items.ids.HYPERCONDUCTIVE_GOLDWRAP), config: "item_hyperconductive_goldwrap" },
                    { value: constants.cooldowns.ECHOES_OF_INSANITY, title: "Item: "+this.itemTitle("shoulder", items.ids.MANTLE_INSANITY), config: "item_mantle_insanity" },
                    { value: constants.cooldowns.ROBE_ARCHMAGE, title: "Item: "+this.itemTitle("chest", items.ids.ROBE_ARCHMAGE), config: "item_robe_archmage" },
                    { value: constants.cooldowns.ECHOES_OF_DEPRAVED, title: "Item: "+this.itemTitle("shoulder", items.ids.SHOULDERPADS_DERANGED), config: "item_shoulderpads_deranged" },
                    { value: constants.cooldowns.UNRESTRAINED_POWER, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_ATALAI_BLOOD_RITUAL_CHARM), trinket: items.ids.TRINKET_ATALAI_BLOOD_RITUAL_CHARM },
                    { value: constants.cooldowns.BURST_OF_KNOWLEDGE, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_BURST_OF_KNOWLEDGE), trinket: items.ids.TRINKET_BURST_OF_KNOWLEDGE },
                    { value: constants.cooldowns.CHROMATIC_INFUSION, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_DRACONIC_EMBLEM), trinket: items.ids.TRINKET_DRACONIC_EMBLEM },
                    { value: constants.cooldowns.OBSIDIAN_INSIGHT, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_EYE_OF_MOAM), trinket: items.ids.TRINKET_EYE_OF_MOAM },
                    { value: constants.cooldowns.CHAOS_FIRE, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_FIRE_RUBY), trinket: items.ids.TRINKET_FIRE_RUBY },
                    { value: constants.cooldowns.ARCANE_POTENCY, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_HAZZARAH), trinket: items.ids.TRINKET_HAZZARAH },
                    { value: constants.cooldowns.MQG, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_MQG), trinket: items.ids.TRINKET_MQG },
                    { value: constants.cooldowns.NAT_PAGLE, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_NAT_PAGLE), trinket: items.ids.TRINKET_NAT_PAGLE },
                    { value: constants.cooldowns.ESSENCE_OF_SAPPHIRON, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_RESTRAINED_ESSENCE), trinket: items.ids.TRINKET_RESTRAINED_ESSENCE },
                    { value: constants.cooldowns.EPHEMERAL_POWER, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_TOEP), trinket: items.ids.TRINKET_TOEP },
                    { value: constants.cooldowns.MANA_INFUSION, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_WARMTH_OF_FORGIVENESS), trinket: items.ids.TRINKET_WARMTH_OF_FORGIVENESS },
                    { value: constants.cooldowns.UNSTABLE_POWER, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_ZHC), trinket: items.ids.TRINKET_ZHC },
                ];

                options.forEach((opt) => {
                    if (opt.hasOwnProperty("rune") && !this.$root.config.runes[opt.rune] ||
                        opt.hasOwnProperty("talent") && !this.$root.config.talents[opt.talent] ||
                        opt.hasOwnProperty("race") && this.$root.config.race != opt.race ||
                        opt.hasOwnProperty("config") && !this.$root.config[opt.config] ||
                        opt.hasOwnProperty("trinket") && !this.$root.isEquipped("trinket", opt.trinket))
                    {
                        opt.title+= " (inactive)";
                    }
                });

                return options;
            },

            buffOptions() {
                var options = [
                    { value: 0, title: "None" },
                    { value: constants.buffs.ARCANE_BLAST, title: "Arcane Blast", rune: "arcane_blast" },
                    { value: constants.buffs.ARCANE_POWER, title: "Arcane Power", talent: "arcane_power" },
                    { value: constants.buffs.ARCANE_SURGE, title: "Arcane Surge", rune: "arcane_surge" },
                    { value: constants.buffs.BALEFIRE_BOLT, title: "Balefire Bolt", rune: "balefire_bolt" },
                    { value: constants.buffs.BERSERKING, title: "Berserking", race: constants.races.RACE_TROLL },
                    { value: constants.buffs.BRAIN_FREEZE, title: "Brain Freeze", rune: "brain_freeze" },
                    { value: constants.buffs.CLEARCAST, title: "Clearcasting", talent: "clearcast" },
                    { value: constants.buffs.COMBUSTION, title: "Combustion", talent: "combustion" },
                    { value: constants.buffs.ENERGIZED_HYPERCONDUCTOR, title: "Energized Hyperconductor", config: "set_hyperconductive_wizard_3p" },
                    { value: constants.buffs.ENIGMAS_ANSWER, title: "Enigma's Answer", config: "set_aq40_5p" },
                    { value: constants.buffs.FINGERS_OF_FROST, title: "Fingers of Frost", rune: "fingers_of_frost" },
                    { value: constants.buffs.GHOST_FINGERS, title: "Fingers of Frost (batch 3rd spell)", rune: "fingers_of_frost" },
                    { value: constants.buffs.HEATING_UP, title: "Heating Up", rune: "hot_streak" },
                    { value: constants.buffs.HOT_STREAK, title: "Hot Streak", rune: "hot_streak" },
                    { value: constants.buffs.ICY_VEINS, title: "Icy Veins", rune: "icy_veins" },
                    { value: constants.buffs.INNERVATE, title: "Innervate" },
                    { value: constants.buffs.MANA_TIDE, title: "Mana Tide" },
                    { value: constants.buffs.MILDLY_IRRADIATED, title: "Mildly Irradiated" },
                    { value: constants.buffs.MISSILE_BARRAGE, title: "Missile Barrage", rune: "missile_barrage" },
                    { value: constants.buffs.NETHERWIND_FOCUS, title: "Netherwind Focus", config: "set_t2_8p" },
                    { value: constants.buffs.POWER_INFUSION, title: "Power Infusion" },
                    { value: constants.buffs.PRESENCE_OF_MIND, title: "Presence of Mind", talent: "presence_of_mind" },
                    { value: constants.buffs.TEMPORAL_BEACON, title: "Temporal Beacon (single)", rune: "regeneration" },
                    { value: constants.buffs.TEMPORAL_BEACON_PARTY, title: "Temporal Beacon (party)", rune: "mass_regeneration" },

                    { value: constants.buffs.ECHOES_OF_MADNESS, title: "Item: "+this.itemTitle("shoulder", items.ids.FRACTURED_MIND_PAULDRONS), config: "item_fractured_mind_pauldrons" },
                    { value: constants.buffs.CHARGED_INSPIRATION, title: "Item: "+this.itemTitle("head", items.ids.GNEURO_LINKED_MONOCLE), config: "item_gneuro_linked_monocle" },
                    { value: constants.buffs.COIN_FLIP_HEADS, title: "Item: "+this.itemTitle("waist", items.ids.HYPERCONDUCTIVE_GOLDWRAP)+" (Heads)", config: "item_hyperconductive_goldwrap" },
                    { value: constants.buffs.COIN_FLIP_TAILS, title: "Item: "+this.itemTitle("waist", items.ids.HYPERCONDUCTIVE_GOLDWRAP)+" (Tails)", config: "item_hyperconductive_goldwrap" },
                    { value: constants.buffs.ECHOES_OF_INSANITY, title: "Item: "+this.itemTitle("shoulder", items.ids.MANTLE_INSANITY), config: "item_mantle_insanity" },
                    { value: constants.buffs.ROAR_OF_THE_DREAM, title: "Item: "+this.itemTitle("finger", items.ids.ROAR_OF_THE_DREAM), config: "item_roar_of_the_dream" },
                    { value: constants.buffs.ECHOES_OF_DEPRAVED, title: "Item: "+this.itemTitle("shoulder", items.ids.SHOULDERPADS_DERANGED), config: "item_shoulderpads_deranged" },

                    { value: constants.buffs.UNRESTRAINED_POWER, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_ATALAI_BLOOD_RITUAL_CHARM), trinket: items.ids.TRINKET_ATALAI_BLOOD_RITUAL_CHARM },
                    { value: constants.buffs.BLUE_DRAGON, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_BLUE_DRAGON), trinket: items.ids.TRINKET_BLUE_DRAGON },
                    { value: constants.buffs.BURST_OF_KNOWLEDGE, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_BURST_OF_KNOWLEDGE), trinket: items.ids.TRINKET_BURST_OF_KNOWLEDGE },
                    { value: constants.buffs.CHROMATIC_INFUSION, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_DRACONIC_EMBLEM), trinket: items.ids.TRINKET_DRACONIC_EMBLEM },
                    { value: constants.buffs.OBSIDIAN_INSIGHT, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_EYE_OF_MOAM), trinket: items.ids.TRINKET_EYE_OF_MOAM },
                    { value: constants.buffs.CHAOS_FIRE, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_FIRE_RUBY), trinket: items.ids.TRINKET_FIRE_RUBY },
                    { value: constants.buffs.ARCANE_POTENCY, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_HAZZARAH), trinket: items.ids.TRINKET_HAZZARAH },
                    { value: constants.buffs.MQG, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_MQG), trinket: items.ids.TRINKET_MQG },
                    { value: constants.buffs.NAT_PAGLE, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_NAT_PAGLE), trinket: items.ids.TRINKET_NAT_PAGLE },
                    { value: constants.buffs.ESSENCE_OF_SAPPHIRON, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_RESTRAINED_ESSENCE), trinket: items.ids.TRINKET_RESTRAINED_ESSENCE },
                    { value: constants.buffs.EPHEMERAL_POWER, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_TOEP), trinket: items.ids.TRINKET_TOEP },
                    { value: constants.buffs.UNSTABLE_POWER, title: "Trinket: "+this.itemTitle("trinket", items.ids.TRINKET_ZHC), trinket: items.ids.TRINKET_ZHC },
                ];

                options.forEach((opt) => {
                    if (opt.hasOwnProperty("rune") && !this.$root.config.runes[opt.rune] ||
                        opt.hasOwnProperty("talent") && !this.$root.config.talents[opt.talent] ||
                        opt.hasOwnProperty("race") && this.$root.config.race != opt.race ||
                        opt.hasOwnProperty("config") && !this.$root.config[opt.config] ||
                        opt.hasOwnProperty("trinket") && !this.$root.isEquipped("trinket", opt.trinket))
                    {
                        opt.title+= " (inactive)";
                    }
                });

                return options;
            },

            debuffOptions() {
                var options = [
                    { value: 0, title: "None" },
                    { value: constants.debuffs.IMPROVED_SCORCH, title: "Imp. Scorch", talent: "imp_scorch" },
                    { value: constants.debuffs.LIVING_BOMB, title: "Living Bomb", rune: "living_bomb" },
                    { value: constants.debuffs.PYROBLAST, title: "Pyroblast", talent: "pyroblast" },
                    { value: constants.debuffs.WINTERS_CHILL, title: "Winter's Chill", talent: "winters_chill" }
                ];

                options.forEach((opt) => {
                    if (opt.hasOwnProperty("rune") && !this.$root.config.runes[opt.rune] ||
                        opt.hasOwnProperty("talent") && !this.$root.config.talents[opt.talent] ||
                        opt.hasOwnProperty("race") && this.$root.config.race != opt.race ||
                        opt.hasOwnProperty("config") && !this.$root.config[opt.config])
                    {
                        opt.title+= " (inactive)";
                    }
                });

                return options;
            },

            spellOptions() {
                var options = [
                    { value: 0, title: "None" },
                    { value: constants.spells.ARCANE_BLAST, title: "Arcane Blast", rune: "arcane_blast" },
                    { value: constants.spells.ARCANE_EXPLOSION, title: "Arcane Explosion" },
                    { value: constants.spells.ARCANE_MISIILES, title: "Arcane Missiles" },
                    { value: constants.spells.ARCANE_SURGE, title: "Arcane Surge", rune: "arcane_surge" },
                    { value: constants.spells.BALEFIRE_BOLT, title: "Balefire Bolt", rune: "balefire_bolt" },
                    { value: constants.spells.BLAST_WAVE, title: "Blast Wave", talent: "blast_wave" },
                    { value: constants.spells.BLIZZARD, title: "Blizzard" },
                    { value: constants.spells.CONE_OF_COLD, title: "Cone of Cold" },
                    { value: constants.spells.DEEP_FREEZE, title: "Deep Freeze", rune: "deep_freeze" },
                    { value: constants.spells.FLAMESTRIKE, title: "Flamestrike" },
                    { value: constants.spells.FLAMESTRIKE_DR, title: "Flamestrike (Downranked)" },
                    { value: constants.spells.FIREBALL, title: "Fireball" },
                    { value: constants.spells.FIRE_BLAST, title: "Fire Blast" },
                    { value: constants.spells.FROSTBOLT, title: "Frostbolt" },
                    { value: constants.spells.FROSTFIRE_BOLT, title: "Frostfire Bolt", rune: "frostfire_bolt" },
                    { value: constants.spells.ICE_LANCE, title: "Ice Lance", rune: "ice_lance" },
                    { value: constants.spells.LIVING_BOMB, title: "Living Bomb", rune: "living_bomb" },
                    { value: constants.spells.LIVING_FLAME, title: "Living Flame", rune: "living_flame" },
                    { value: constants.spells.MASS_REGENERATION, title: "Mass Regeneration", rune: "mass_regeneration" },
                    { value: constants.spells.PYROBLAST, title: "Pyroblast", talent: "pyroblast" },
                    { value: constants.spells.REGENERATION, title: "Regeneration", rune: "regeneration" },
                    { value: constants.spells.SCORCH, title: "Scorch" },
                    { value: constants.spells.SPELLFROST_BOLT, title: "Spellfrost Bolt", rune: "spellfrost_bolt" },
                ];

                options.forEach((opt) => {
                    if (opt.hasOwnProperty("rune") && !this.$root.config.runes[opt.rune] ||
                        opt.hasOwnProperty("talent") && !this.$root.config.talents[opt.talent] ||
                        opt.hasOwnProperty("race") && this.$root.config.race != opt.race ||
                        opt.hasOwnProperty("config") && !this.$root.config[opt.config])
                    {
                        opt.title+= " (inactive)";
                    }
                });

                return options;
            },
        },

        methods: {
            itemTitle(slot, id) {
                return _.get(this.item(slot, id), "title", null);
            },

            item(slot, id) {
                return _.find(items.equip[slot], {id: id});
            },

            changed() {
                this.$emit("update:modelValue", this.modelValue);
            },
        }
    }
</script>