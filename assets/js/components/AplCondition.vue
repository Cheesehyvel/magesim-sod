<template>
    <div class="apl-condition">
        <div class="apl-condition-values">
            <select v-model="modelValue.type" @change="changedType">
                <option v-for="opt in type_options" :value="opt.value">{{ opt.title }}</option>
            </select>
            
            <apl-value v-model="modelValue.values[0]" :expect="expectValue" v-if="modelValue.values.length > 0" />

            <select v-model="modelValue.op" @change="changed" v-if="hasOp">
                <option v-for="opt in op_options" :value="opt.value">{{ opt.title }}</option>
            </select>

            <apl-value v-model="modelValue.values[1]" :expect="expectValue" v-if="modelValue.values.length > 1" />
        </div>

        <template v-for="(condition, index) in modelValue.conditions" :key="condition.id">
            <apl-condition v-model="modelValue.conditions[index]" :deletable="true" @delete="deleteCondition(index)" />
        </template>

        <div class="btn btn-secondary smaller" @click="createCondition" v-if="canCreateCondition">Add condition</div>

        <div class="delete" @click="$emit('delete')" v-if="deletable">
            <span class="material-icons">&#xe5cd;</span>
        </div>
    </div>
</template>

<script>
    import constants from "../constants";

    export default {
        props: ["modelValue", "deletable"],

        data() {
            return {
                type_options: [
                    { value: constants.apl_condition_types.CONDITION_TYPE_NONE, title: "No condition", conditions: 0, values: 0 },
                    { value: constants.apl_condition_types.CONDITION_TYPE_CMP, title: "Compare", conditions: 0, values: 2 },
                    { value: constants.apl_condition_types.CONDITION_TYPE_NOT, title: "Not", conditions: 1, values: 0 },
                    { value: constants.apl_condition_types.CONDITION_TYPE_TRUE, title: "Is true", conditions: 0, values: 1 },
                    { value: constants.apl_condition_types.CONDITION_TYPE_FALSE, title: "Is false", conditions: 0, values: 1 },
                    { value: constants.apl_condition_types.CONDITION_TYPE_AND, title: "All of", values: 0 },
                    { value: constants.apl_condition_types.CONDITION_TYPE_OR, title: "Any of", values: 0 },
                ],
                op_options: [
                    { value: constants.apl_condition_ops.CONDITION_OP_EQ, title: "=" },
                    { value: constants.apl_condition_ops.CONDITION_OP_NEQ, title: "!=" },
                    { value: constants.apl_condition_ops.CONDITION_OP_GT, title: ">" },
                    { value: constants.apl_condition_ops.CONDITION_OP_GTE, title: ">=" },
                    { value: constants.apl_condition_ops.CONDITION_OP_LT, title: "<" },
                    { value: constants.apl_condition_ops.CONDITION_OP_LTE, title: "<=" },
                ],
            }
        },

        computed: {
            hasOp() {
                return this.modelValue.type == constants.apl_condition_types.CONDITION_TYPE_CMP;
            },

            type() {
                return _.find(this.type_options, {value:this.modelValue.type});
            },

            canCreateCondition() {
                return this.modelValue.type == constants.apl_condition_types.CONDITION_TYPE_AND || 
                    this.modelValue.type == constants.apl_condition_types.CONDITION_TYPE_OR;
            },

            expectValue() {
                if (this.modelValue.type == constants.apl_condition_types.CONDITION_TYPE_CMP)
                    return "number";
                if (this.modelValue.type == constants.apl_condition_types.CONDITION_TYPE_TRUE)
                    return "bool";
                if (this.modelValue.type == constants.apl_condition_types.CONDITION_TYPE_FALSE)
                    return "bool";
                return null;
            }
        },

        methods: {
            changed() {
                this.$emit("update:modelValue", this.modelValue);
            },

            changedType() {
                if (this.type.hasOwnProperty("conditions")) {
                    if (this.modelValue.conditions.length < this.type.conditions) {
                        for (var i=this.modelValue.conditions.length; i<this.type.conditions; i++)
                            this.modelValue.conditions.push(this.newCondition());
                    }
                    else if (this.modelValue.conditions.length > this.type.conditions) {
                        this.modelValue.conditions.splice(this.type.conditions);
                    }
                }

                if (this.type.hasOwnProperty("values")) {
                    if (this.modelValue.values.length < this.type.values) {
                        for (var i=this.modelValue.values.length; i<this.type.values; i++)
                            this.modelValue.values.push(this.newValue());
                    }
                    else if (this.modelValue.values.length > this.type.values) {
                        this.modelValue.values.splice(this.type.values);
                    }
                }

                this.changed();
            },

            deleteCondition(index) {
                this.modelValue.conditions.splice(index, 1);
                this.changed();
            },

            createCondition() {
                this.modelValue.conditions.push(this.newCondition());
                this.changed();
            },

            newCondition() {
                return {
                    id: this.$root.uuid(),
                    type: constants.apl_condition_types.CONDITION_TYPE_NONE,
                    op: constants.apl_condition_ops.CONDITION_OP_EQ,
                    values: [],
                    conditions: [],
                }
            },

            newValue() {
                return {
                    id: this.$root.uuid(),
                    type: constants.apl_value_types.VALUE_TYPE_NONE,
                    str: "",
                    value: 0,
                    id: 0,
                }
            }
        }
    }
</script>