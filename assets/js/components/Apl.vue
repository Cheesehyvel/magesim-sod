<template>
    <div class="apl">
        <div class="apl-notice">
            BETA: APL lets you set up your own custom rotation.<br>
            Probably buggy as hell right now.
        </div>
        <div class="apl-items">
            <div class="apl-item" v-for="(item, index) in modelValue" :key="item.id">
                <div class="add" @click="createItem(index)">
                    <span>+ </span>
                    <span>Add action</span>
                </div>

                <apl-condition v-model="item.condition" />
                <apl-action v-model="item.action" />

                <div class="move move-up" v-if="index > 0" @click="move(index, -1)">
                    <span class="material-icons">&#xe5d8;</span>
                </div>
                <div class="move move-down" v-if="index < modelValue.length-1" @click="move(index, 1)">
                    <span class="material-icons">&#xe5db;</span>
                </div>
                <div class="status" :class="[item.status ? 'active' : 'inactive']" @click="statusItem(index)">
                    <span v-if="item.status">
                        <span class="material-icons">&#xe8f4;</span>
                        <tooltip position="l">Disable</tooltip>
                    </span>
                    <span v-else>
                        <span class="material-icons">&#xe8f5;</span>
                        <tooltip position="l">Enable</tooltip>
                    </span>
                </div>
                <div class="delete" @click="deleteItem(index)">
                    <span class="material-icons">&#xe5cd;</span>
                </div>
            </div>
            <div class="btn btn-primary small mt-1" @click="createItem()">Add action</div>
        </div>
    </div>
</template>

<script>
    import constants from "../constants";
    import items from "../items";
    import runes from "../runes";

    export default {
        props: ["modelValue"],

        mounted() {
            if (!this.modelValue.length)
                this.createItem();
        },

        methods: {
            changed() {
                this.$emit("update:modelValue", this.modelValue);
            },

            move(index, diff) {
                var arr = this.modelValue;
                arr.splice(index+diff, 0, arr.splice(index, 1)[0]);
                this.$emit("update:modelValue", arr);
            },

            statusItem(index) {
                this.modelValue[index].status = !this.modelValue[index].status;
                this.$emit("update:modelValue", this.modelValue);
            },

            deleteItem(index) {
                this.$emit("update:modelValue", this.modelValue.slice(0, index).concat(this.modelValue.slice(index+1)));

                this.$nextTick(() => {
                    if (!this.modelValue.length)
                        this.createItem();
                });
            },

            createItem(index) {
                if (typeof(index) != "undefined") {
                    var arr = this.modelValue;
                    arr.splice(index, 0, this.newItem());
                    this.$emit("update:modelValue", arr);
                }
                else {
                    this.$emit("update:modelValue", this.modelValue.concat([this.newItem()]));
                }
            },

            newItem() {
                return {
                    id: this.$root.uuid(),
                    status: true,
                    action: {
                        type: constants.apl_action_types.ACTION_TYPE_NONE,
                        str: "",
                        value: 0,
                        id: 0,
                        key: "nothing",
                        sequence: [],
                    },
                    condition: {
                        type: constants.apl_condition_types.CONDITION_TYPE_NONE,
                        op: constants.apl_condition_ops.CONDITION_OP_EQ,
                        conditions: [],
                        values: [],
                    },
                }
            }
        }
    }
</script>