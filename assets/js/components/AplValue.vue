<template>
    <div class="apl-value">
        <select v-model="modelValue.type" @change="changed">
            <option v-for="opt in typeOptions" :value="opt.value">{{ opt.title }}</option>
        </select>

        <input type="text" v-model.number="modelValue.value" v-if="type.input == 'number'" @input="changed">

        <select v-model="modelValue.id" @change="changed" v-if="idOptions.length" @input="changed">
            <option v-for="opt in idOptions" :value="opt.value">{{ opt.title }}</option>
        </select>
    </div>
</template>

<script>
    import constants from "../constants";

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
                if (this.type.input == "talents")
                    return this.talent_options;
                if (this.type.input == "runes")
                    return this.rune_options;
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

            cooldownOptions() {
                var options = [
                    { value: constants.cooldowns.POTION, title: "Potion" },
                    { value: constants.cooldowns.CONE_OF_COLD, title: "Cone of Cold" },
                    { value: constants.cooldowns.EVOCATION, title: "Evocation" },
                    { value: constants.cooldowns.FIRE_BLAST, title: "Fire Blast" },
                    { value: constants.cooldowns.MANA_GEM, title: "Mana Gem" },
                ];

                if (this.$root.config.race == constants.races.RACE_TROLL)
                    options.push({ value: constants.cooldowns.BERSERKING, title: "Berserking" });

                if (this.$root.config.talents.arcane_power)
                    options.push({ value: constants.cooldowns.ARCANE_POWER, title: "Arcane Power" });
                if (this.$root.config.talents.blast_wave)
                    options.push({ value: constants.cooldowns.BLAST_WAVE, title: "Blast Wave" });
                if (this.$root.config.talents.cold_snap)
                    options.push({ value: constants.cooldowns.COLD_SNAP, title: "Cold Snap" });
                if (this.$root.config.talents.combustion)
                    options.push({ value: constants.cooldowns.COMBUSTION, title: "Combustion" });
                if (this.$root.config.talents.presence_of_mind)
                    options.push({ value: constants.cooldowns.PRESENCE_OF_MIND, title: "Presence of Mind" });

                if (this.$root.config.runes.arcane_surge)
                    options.push({ value: constants.cooldowns.ARCANE_SURGE, title: "Arcane Surge" });
                if (this.$root.config.runes.brain_freeze)
                    options.push({ value: constants.cooldowns.BRAIN_FREEZE, title: "Brain Freeze" });
                if (this.$root.config.runes.deep_freeze)
                    options.push({ value: constants.cooldowns.DEEP_FREEZE, title: "Deep Freeze" });
                if (this.$root.config.runes.icy_veins)
                    options.push({ value: constants.cooldowns.ICY_VEINS, title: "Icy Veins" });
                if (this.$root.config.runes.living_flame)
                    options.push({ value: constants.cooldowns.LIVING_FLAME, title: "Living Flame" });

                if (this.$root.config.item_gneuro_linked_monocle)
                    options.push({ value: constants.cooldowns.CHARGED_INSPIRATION, title: "Gneuro-Linked Arcano-Filament Monocle" });
                if (this.$root.config.item_hyperconductive_goldwrap)
                    options.push({ value: constants.cooldowns.COIN_FLIP, title: "Hyperconductive Goldwrap" });
                if (this.$root.config.item_electromagnetic_hyperflux_reactivator)
                    options.push({ value: constants.cooldowns.ELECTROMAGNETIC_GIGAFLUX_REACTIVATOR, title: "Electromagnetic Hyperflux Reactivator" });
                if (this.$root.config.item_robe_archmage)
                    options.push({ value: constants.cooldowns.ROBE_ARCHMAGE, title: "Robe of the Archmage" });
                if (this.$root.config.item_celestial_orb)
                    options.push({ value: constants.cooldowns.CELESTIAL_ORB, title: "Celestial Orb" });

                // TODO: Trinket

                return [{ value: constants.cooldowns.NONE, title: "None" }].concat(_.sortBy(options, "title"));
            },

            buffOptions() {
                var options = [
                    { value: constants.buffs.INNERVATE, title: "Innervate" },
                    { value: constants.buffs.MANA_TIDE, title: "Mana Tide" },
                    { value: constants.buffs.POWER_INFUSION, title: "Power Infusion" },
                ];

                if (this.$root.config.race == constants.races.RACE_TROLL)
                    options.push({ value: constants.buffs.BERSERKING, title: "Berserking" });
                if (this.$root.config.talents.arcane_power)
                    options.push({ value: constants.buffs.ARCANE_POWER, title: "Arcane Power" });
                if (this.$root.config.talents.clearcast)
                    options.push({ value: constants.buffs.CLEARCAST, title: "Clearcasting" });
                if (this.$root.config.talents.combustion)
                    options.push({ value: constants.buffs.COMBUSTION, title: "Combustion" });
                if (this.$root.config.talents.presence_of_mind)
                    options.push({ value: constants.buffs.PRESENCE_OF_MIND, title: "Presence of Mind" });

                if (this.$root.config.runes.arcane_blast)
                    options.push({ value: constants.buffs.ARCANE_BLAST, title: "Arcane Blast" });
                if (this.$root.config.runes.arcane_surge)
                    options.push({ value: constants.buffs.ARCANE_SURGE, title: "Arcane Surge" });
                if (this.$root.config.runes.balefire_bolt)
                    options.push({ value: constants.buffs.BALEFIRE_BOLT, title: "Balefire Bolt" });
                if (this.$root.config.runes.brain_freeze)
                    options.push({ value: constants.buffs.BRAIN_FREEZE, title: "Brain Freeze" });
                if (this.$root.config.runes.fingers_of_frost)
                    options.push({ value: constants.buffs.FINGERS_OF_FROST, title: "Fingers of Frost" });
                if (this.$root.config.runes.fingers_of_frost)
                    options.push({ value: constants.buffs.GHOST_FINGERS, title: "Fingers of Frost (batch 3rd spell)" });
                if (this.$root.config.runes.hot_streak) {
                    options.push({ value: constants.buffs.HOT_STREAK, title: "Hot Streak" });
                    options.push({ value: constants.buffs.HEATING_UP, title: "Heating Up" });
                }
                if (this.$root.config.runes.icy_veins)
                    options.push({ value: constants.buffs.ICY_VEINS, title: "Icy Veins" });
                if (this.$root.config.runes.missile_barrage)
                    options.push({ value: constants.buffs.MISSILE_BARRAGE, title: "Missile Barrage" });
                if (this.$root.config.runes.regeneration)
                    options.push({ value: constants.buffs.TEMPORAL_BEACON, title: "Temporal Beacon (single)" });
                if (this.$root.config.runes.mass_regeneration)
                    options.push({ value: constants.buffs.TEMPORAL_BEACON_PARTY, title: "Temporal Beacon (party)" });

                if (this.$root.config.item_gneuro_linked_monocle)
                    options.push({ value: constants.buffs.CHARGED_INSPIRATION, title: "Charged Inspiration" });
                if (this.$root.config.item_hyperconductive_goldwrap) {
                    options.push({ value: constants.buffs.COIN_FLIP_HEADS, title: "Coin Flip: Heads" });
                    options.push({ value: constants.buffs.COIN_FLIP_TAILS, title: "Coin Flip: Tails" });
                }
                if (this.$root.config.set_hyperconductive_wizard_3p)
                    options.push({ value: constants.buffs.ENERGIZED_HYPERCONDUCTOR, title: "Energized Hyperconductor" });
                if (this.$root.config.set_t2_8p)
                    options.push({ value: constants.buffs.NETHERWIND_FOCUS, title: "Netherwind Focus" });
                if (this.$root.config.set_aq40_5p)
                    options.push({ value: constants.buffs.ENIGMAS_ANSWER, title: "Enigma's Answer" });

                return [{ value: 0, title: "None" }].concat(_.sortBy(options, "title"));
            },

            debuffOptions() {
                var options = [
                    { value: constants.debuffs.IMPROVED_SCORCH, title: "Imp. Scorch" }
                ];

                if (this.$root.config.talents.pyroblast)
                    options.push({ value: constants.debuffs.PYROBLAST, title: "Pyroblast" });
                if (this.$root.config.talents.winters_chill)
                    options.push({ value: constants.debuffs.WINTERS_CHILL, title: "Winter's Chill" });
                if (this.$root.config.runes.living_bomb)
                    options.push({ value: constants.debuffs.LIVING_BOMB, title: "Living Bomb" });

                return [{ value: 0, title: "None" }].concat(_.sortBy(options, "title"));
            },

            spellOptions() {
                var options = [
                    { value: constants.spells.ARCANE_EXPLOSION, title: "Arcane Explosion" },
                    { value: constants.spells.ARCANE_MISIILES, title: "Arcane Missiles" },
                    { value: constants.spells.BLIZZARD, title: "Blizzard" },
                    { value: constants.spells.CONE_OF_COLD, title: "Cone of Cold" },
                    { value: constants.spells.FLAMESTRIKE, title: "Flamestrike" },
                    { value: constants.spells.FLAMESTRIKE_DR, title: "Flamestrike (Downranked)" },
                    { value: constants.spells.FIREBALL, title: "Fireball" },
                    { value: constants.spells.FIRE_BLAST, title: "Fire Blast" },
                    { value: constants.spells.FROSTBOLT, title: "Frostbolt" },
                ];

                if (this.$root.config.talents.blast_wave)
                    options.push({ value: constants.spells.BLAST_WAVE, title: "Blast Wave" });
                if (this.$root.config.talents.pyroblast)
                    options.push({ value: constants.spells.PYROBLAST, title: "Pyroblast" });

                if (this.$root.config.runes.arcane_blast)
                    options.push({ value: constants.spells.ARCANE_BLAST, title: "Arcane Blast" });
                if (this.$root.config.runes.arcane_surge)
                    options.push({ value: constants.spells.ARCANE_SURGE, title: "Arcane Surge" });
                if (this.$root.config.runes.balefire_bolt)
                    options.push({ value: constants.spells.BALEFIRE_BOLT, title: "Balefire Bolt" });
                if (this.$root.config.runes.deep_freeze)
                    options.push({ value: constants.spells.DEEP_FREEZE, title: "Deep Freeze" });
                if (this.$root.config.runes.frostfire_bolt)
                    options.push({ value: constants.spells.FROSTFIRE_BOLT, title: "Frostfire Bolt" });
                if (this.$root.config.runes.ice_lance)
                    options.push({ value: constants.spells.ICE_LANCE, title: "Ice Lance" });
                if (this.$root.config.runes.living_bomb)
                    options.push({ value: constants.spells.LIVING_BOMB, title: "Living Bomb" });
                if (this.$root.config.runes.living_flame)
                    options.push({ value: constants.spells.LIVING_FLAME, title: "Living Flame" });
                if (this.$root.config.runes.mass_regeneration)
                    options.push({ value: constants.spells.MASS_REGENERATION, title: "Mass Regeneration" });
                if (this.$root.config.runes.regeneration)
                    options.push({ value: constants.spells.REGENERATION, title: "Regeneration" });
                if (this.$root.config.runes.scorch)
                    options.push({ value: constants.spells.SCORCH, title: "Scorch" });
                if (this.$root.config.runes.spellfrost_bolt)
                    options.push({ value: constants.spells.SPELLFROST_BOLT, title: "Spellfrost Bolt" });

                return [{ value: 0, title: "None" }].concat(_.sortBy(options, "title"));
            },
        },

        methods: {
            changed() {
                this.$emit("update:modelValue", this.modelValue);
            },
        }
    }
</script>