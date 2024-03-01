<template>
    <div class="apl-action">
        <select :value="modelValue.key" @change="changeAction(index, $event.target.value)">
            <option v-for="action in actionOptions" :value="action.key" :key="action.key">{{ action.title }}</option>
        </select>

        <div class="apl-sequence" v-if="hasSequence">
            <apl-action v-for="(action, index) in modelValue.sequence" v-model="modelValue.sequence[index]" :child="true" @delete="deleteAction(index)" />
            <div class="btn btn-primary smaller" @click="createAction">Add action</div>
        </div>

        <div class="delete" @click="$emit('delete')" v-if="child">
            <span class="material-icons">&#xe5cd;</span>
        </div>
    </div>
</template>

<script>
    import constants from "../constants";

    export default {
        props: ["modelValue", "child"],

        data() {
            return {
                model: this.modelValue,
                action_options: [
                    { 
                        title: "Do nothing",
                        key: "nothing", 
                        type: constants.apl_action_types.ACTION_TYPE_NONE,
                    },
                    { 
                        title: "Sequence",
                        key: "sequence", 
                        type: constants.apl_action_types.ACTION_TYPE_SEQUENCE,
                    },
                    { 
                        title: "Cast: Arcane Blast",
                        key: "arcane_blast", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.ARCANE_BLAST,
                        rune: "arcane_blast",
                    },
                    { 
                        title: "Cast: Arcane Explosion",
                        key: "arcane_explosion", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.ARCANE_EXPLOSION,
                    },
                    { 
                        title: "Cast: Arcane Missiles",
                        key: "arcane_missiles", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.ARCANE_MISSILES,
                    },
                    { 
                        title: "Cast: Arcane Power",
                        key: "arcane_power", 
                        type: constants.apl_action_types.ACTION_TYPE_BUFF,
                        id: constants.spells.ARCANE_POWER,
                        talent: "arcane_power",
                    },
                    { 
                        title: "Cast: Arcane Surge",
                        key: "arcane_surge", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.ARCANE_SURGE,
                        rune: "arcane_surge",
                    },
                    { 
                        title: "Cast: Balefire Bolt",
                        key: "balefire_bolt", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.BALEFIRE_BOLT,
                        rune: "balefire_bolt",
                    },
                    { 
                        title: "Cast: Berserking",
                        key: "berserking", 
                        type: constants.apl_action_types.ACTION_TYPE_BUFF,
                        id: constants.buffs.BERSERKING,
                        race: constants.races.RACE_TROLL,
                    },
                    { 
                        title: "Cast: Blast Wave",
                        key: "blast_wave", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.BLAST_WAVE,
                        talent: "blast_wave",
                    },
                    { 
                        title: "Cast: Blizzard",
                        key: "blizzard", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.BLIZZARD,
                    },
                    { 
                        title: "Cast: Cold Snap",
                        key: "cold_snap", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.COLD_SNAP,
                        talent: "cold_snap",
                    },
                    { 
                        title: "Cast: Combustion",
                        key: "combustion", 
                        type: constants.apl_action_types.ACTION_TYPE_BUFF,
                        id: constants.buffs.COMBUSTION,
                        talent: "combustion",
                    },
                    { 
                        title: "Cast: Cone of Cold",
                        key: "cone_of_cold", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.CONE_OF_COLD,
                    },
                    { 
                        title: "Cast: Deep Freeze",
                        key: "deep_freeze", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.DEEP_FREEZE,
                        rune: "deep_freeze",
                    },
                    { 
                        title: "Cast: Evocation",
                        key: "evocation", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.EVOCATION,
                    },
                    { 
                        title: "Cast: Flamestrike",
                        key: "flamestrike", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.FLAMESTRIKE,
                    },
                    { 
                        title: "Cast: Flamestrike (Downranked)",
                        key: "flamestrike_dr", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.FLAMESTRIKE_DR,
                    },
                    { 
                        title: "Cast: Fire Blast",
                        key: "fire_blast", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.FIRE_BLAST,
                    },
                    { 
                        title: "Cast: Fireball",
                        key: "fireball", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.FIREBALL,
                    },
                    { 
                        title: "Cast: Frostbolt",
                        key: "frostbolt", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.FROSTBOLT,
                    },
                    { 
                        title: "Cast: Frostfire Bolt",
                        key: "frostfire_bolt", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.FROSTFIRE_BOLT,
                        rune: "frostfire_bolt",
                    },
                    { 
                        title: "Cast: Ice Lance",
                        key: "ice_lance", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.ICE_LANCE,
                        rune: "ice_lance",
                    },
                    { 
                        title: "Cast: Icy Veins",
                        key: "icy_veins", 
                        type: constants.apl_action_types.ACTION_TYPE_BUFF,
                        id: constants.buffs.ICY_VEINS,
                        rune: "icy_veins",
                    },
                    { 
                        title: "Cast: Living Bomb",
                        key: "living_bomb", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.LIVING_BOMB,
                        rune: "living_bomb",
                    },
                    { 
                        title: "Cast: Living Flame",
                        key: "living_flame", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.LIVING_FLAME,
                        rune: "living_flame",
                    },
                    { 
                        title: "Cast: Mass Regeneration",
                        key: "mass_regeneration", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.MASS_REGENERATION,
                        rune: "mass_regeneration",
                    },
                    { 
                        title: "Cast: Pyroblast",
                        key: "pyroblast", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.PYROBLAST,
                        talent: "pyroblast",
                    },
                    { 
                        title: "Cast: Presence of Mind",
                        key: "presence_of_mind", 
                        type: constants.apl_action_types.ACTION_TYPE_BUFF,
                        id: constants.buffs.PRESENCE_OF_MIND,
                        talent: "presence_of_mind",
                    },
                    { 
                        title: "Cast: Regeneration",
                        key: "regeneration", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.REGENERATION,
                        rune: "regeneration",
                    },
                    { 
                        title: "Cast: Scorch",
                        key: "scorch", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.SCORCH,
                    },
                    { 
                        title: "Cast: Spellfrost Bolt",
                        key: "spellfrost_bolt", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.SPELLFROST_BOLT,
                        rune: "spellfrost_bolt",
                    },
                    { 
                        title: "Item: Celestial Orb",
                        key: "item_celestial_orb", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.CELESTIAL_ORB,
                        config: "item_celestial_orb",
                    },
                    { 
                        title: "Item: Electromagnetic Hyperflux Reactivator",
                        key: "item_electromagnetic_hyperflux_reactivator", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.ELECTROMAGNETIC_GIGAFLUX_REACTIVATOR,
                        config: "item_electromagnetic_hyperflux_reactivator",
                    },
                    { 
                        title: "Item: Gneuro-Linked Arcano-Filament Monocle",
                        key: "item_gneuro_linked_monocle", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.CHARGED_INSPIRATION,
                        config: "item_gneuro_linked_monocle",
                    },
                    { 
                        title: "Item: Hyperconductive Goldwrap",
                        key: "item_hyperconductive_goldwrap", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.COIN_FLIP,
                        config: "item_hyperconductive_goldwrap",
                    },
                    { 
                        title: "Item: Robe of the Archmage",
                        key: "item_robe_archmage", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.ROBE_ARCHMAGE,
                        config: "item_robe_archmage",
                    },
                    { 
                        title: "External: Innervate",
                        key: "innervate", 
                        type: constants.apl_action_types.ACTION_TYPE_BUFF,
                        id: constants.buffs.INNERVATE,
                    },
                    { 
                        title: "External: Mana Tide",
                        key: "mana_tide", 
                        type: constants.apl_action_types.ACTION_TYPE_BUFF,
                        id: constants.buffs.MANA_TIDE,
                    },
                    { 
                        title: "External: Power Infusion",
                        key: "power_infusion", 
                        type: constants.apl_action_types.ACTION_TYPE_BUFF,
                        id: constants.buffs.POWER_INFUSION,
                    },
                    { 
                        title: "Use: Mana Gem",
                        key: "mana_gem", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.MANA_GEM,
                    },
                    { 
                        title: "Use: Potion",
                        key: "potion", 
                        type: constants.apl_action_types.ACTION_TYPE_CUSTOM,
                        str: "potion",
                    },
                    { 
                        title: "Use: Mildly Irradiated Rejuvenation Potion",
                        key: "mildly_irradiated_potion", 
                        type: constants.apl_action_types.ACTION_TYPE_SPELL,
                        id: constants.spells.MILDLY_IRRADIATED_POTION,
                    },
                ],
            }
        },

        computed: {
            actionOptions() {
                var options = _.cloneDeep(this.action_options);

                options.forEach((opt) => {
                    if (opt.hasOwnProperty("rune") && !this.$root.config.runes[opt.rune] ||
                        opt.hasOwnProperty("talent") && !this.$root.config.talents[opt.talent] ||
                        opt.hasOwnProperty("race") && this.$root.config.race != opt.race ||
                        opt.hasOwnProperty("config") && !this.$root.config[opt.config] ||
                        opt.type == constants.apl_action_types.ACTION_TYPE_SEQUENCE && this.child)
                    {
                        opt.title+= " (inactive)";
                    }
                });

                return options;
            },

            hasSequence() {
                return this.modelValue.type == constants.apl_action_types.ACTION_TYPE_SEQUENCE;
            },
        },

        methods: {
            changed() {
                this.$emit("update:modelValue", this.modelValue);
            },

            changeAction(index, key) {
                var action = _.merge(this.newAction(), _.clone(_.find(this.action_options, {key: key})));
                if (action.type == constants.apl_action_types.ACTION_TYPE_SEQUENCE)
                    action.sequence.push(this.newAction());
                this.$emit("update:modelValue", action);
            },

            deleteAction(index) {
                this.modelValue.sequence.splice(index, 1);
                this.changed();
            },

            createAction() {
                this.modelValue.sequence.push(this.newAction());
                this.changed();
            },

            newAction() {
                return {
                    type: constants.apl_action_types.ACTION_TYPE_NONE,
                    str: "",
                    value: 0,
                    id: 0,
                    key: "nothing",
                    sequence: [],
                }
            }
        }
    }
</script>