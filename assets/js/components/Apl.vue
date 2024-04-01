<template>
    <div class="apl">
        <div class="btn smaller apl-collapse mb-2" @click="collapseAll()">
            <template v-if="isAllCollapsed()">Expand all</template>
            <template v-else>Collapse all</template>
        </div>
        <div class="apl-items">
            <div class="apl-item" :class="[isCollapsed(item) ? 'collapsed' : '']" v-for="(item, index) in modelValue" :key="item.id">
                <div class="add" @click="createItem(index)">
                    <span>+ </span>
                    <span>Add action</span>
                </div>

                <template v-if="!isCollapsed(item)">
                    <apl-condition v-model="item.condition" />
                    <apl-action v-model="item.action" />
                </template>
                <div class="apl-title" @click="collapseToggle(item)" v-else>
                    <span class="material-icons mr-n">&#xe5d7;</span>
                    <span>{{ title(item) }}</span>
                </div>

                <div class="move move-up" v-if="index > 0" @click="move(index, -1)">
                    <span class="material-icons">&#xe5d8;</span>
                </div>
                <div class="move move-down" v-if="index < modelValue.length-1" @click="move(index, 1)">
                    <span class="material-icons">&#xe5db;</span>
                </div>
                <div class="expand" @click="collapseToggle(item)" v-if="isCollapsed(item)">
                    <span class="material-icons">&#xe145;</span>
                    <tooltip position="l">Expand</tooltip>
                </div>
                <div class="collapse" @click="collapseToggle(item)" v-else>
                    <span class="material-icons">&#xe5d6;</span>
                    <tooltip position="r">Collapse</tooltip>
                </div>
                <div class="clone" @click="clone(index)">
                    <span class="material-icons">&#xe14d;</span>
                    <tooltip position="l">Clone</tooltip>
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

        data() {
            return {
                collapsed: [],
            }
        },

        methods: {
            isCollapsed(item) {
                return this.collapsed.indexOf(item.id) != -1;
            },

            isAllCollapsed() {
                for (var i=0; i<this.modelValue.length; i++) {
                    if (!this.isCollapsed(this.modelValue[i]))
                        return false;
                }

                return true;
            },

            collapseToggle(item) {
                var index = this.collapsed.indexOf(item.id);
                if (index == -1)
                    this.collapsed.push(item.id);
                else
                    this.collapsed.splice(index, 1);
            },

            collapseAll() {
                if (this.isAllCollapsed()) {
                    this.collapsed = [];
                }
                else {
                    for (var i=0; i<this.modelValue.length; i++) {
                        if (!this.isCollapsed(this.modelValue[i]))
                            this.collapseToggle(this.modelValue[i]);
                    }
                }
            },

            title(item) {
                var arr = [];

                if (item && item.action && item.action.title)
                    arr.push(item.action.title);
                else
                    arr.push("No action");

                var numCond = (cond) => {
                    if (!cond.hasOwnProperty("conditions"))
                        return 0;
                    var n = cond.conditions.length;
                    for (var i=0; i<cond.conditions.length; i++)
                        n+= numCond(cond.conditions[i]);
                    return n;
                };

                if (item.condition) {
                    var n_cond = numCond(item.condition) + 1;
                    if (n_cond == 1)
                        arr.push(n_cond+" condition");
                    else
                        arr.push(n_cond+" conditions");
                }

                return arr.length ? "("+arr.join(", ")+")" : null;
            },

            changed() {
                this.$emit("update:modelValue", this.modelValue);
            },

            move(index, diff) {
                var arr = this.modelValue;
                arr.splice(index+diff, 0, arr.splice(index, 1)[0]);
                this.$emit("update:modelValue", arr);
            },

            clone(index) {
                var item = _.cloneDeep(this.modelValue[index]);
                item.id = this.$root.uuid();
                this.modelValue.splice(index, 0, item);
                this.$emit("update:modelValue", this.modelValue);
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
                    collapsed: false,
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