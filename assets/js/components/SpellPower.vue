<template>
    <span class="spell-power">
        <template v-for="(line, index) in lines">
            <template v-if="index > 0"> / </template>
            <span :class="['sp-'+line.type]">
                {{ line.value }}
                <tooltip v-if="line.type != 'all'" position="l">{{ line.type }} spell power</tooltip>
                <span class="material-icons" v-if="false && line.icon" v-html="'&#x'+line.icon"></span>
            </span>
        </template>
    </span>
</template>

<script>
    export default {
        props: ["item"],

        computed: {
            lines() {
                var arr = [];

                if (this.item.sp)
                    arr.push({value: this.item.sp, type: "all", icon: null});
                if (this.item.sp_arcane)
                    arr.push({value: this.item.sp_arcane, type: "arcane", icon: "ea25"});
                if (this.item.sp_fire)
                    arr.push({value: this.item.sp_fire, type: "fire", icon: "ef55"});
                if (this.item.sp_frost)
                    arr.push({value: this.item.sp_frost, type: "frost", icon: "eb3b"});

                return arr;
            }
        },
    }
</script>