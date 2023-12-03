<template>
    <div id="app">
        <div class="notice notice-alt" v-if="donation_open" @click="donation_open = false">
            <div class="inner">
                <div class="title">Thank you!</div>
                <div class="text mt-2">
                    For your donation.
                </div>
            </div>
        </div>

        <div class="notices">
            <div class="error-notice notice" v-if="error_notice.open" @click="error_notice.open = false">
                <div class="inner">
                    <div class="title mb-2" v-if="error_notice.title">{{ error_notice.title }}</div>
                    <div class="text" v-if="error_notice.text">
                        <div v-for="text in error_notice.text">{{ text }}</div>
                    </div>
                </div>
            </div>
            <div class="profile-status notice" v-if="profile_status.open" @click="profile_status.open = false">
                <div class="inner">
                    <div class="title">Profile loaded</div>
                    <div class="checklist">
                        <check-item :value="profile_status.items">Items</check-item>
                        <check-item :value="false" v-for="slot in profile_status.missing_items" :key="slot">{{ formatKey(slot) }}</check-item>
                        <check-item :value="profile_status.config">Config</check-item>
                    </div>
                </div>
            </div>
        </div>

        <div class="wrapper">
            <div class="sidebar">
                <div class="actions">
                    <div class="btn large block mt-n" @click="runMultiple" :class="[is_running ? 'disabled' : '']">
                        <div>Run</div>
                        <div>{{ config.iterations }} iterations</div>
                    </div>
                    <div class="btn block mt-n" @click="runSingle" :class="[is_running ? 'disabled' : '']">
                        <div>Single iteration</div>
                    </div>
                    <div class="btn block mt-n" @click="runEP" :class="[is_running && !is_running_ep ? 'disabled' : '']">
                        <div v-if="!is_running_ep">Stat weights</div>
                        <div v-else>Stop</div>
                    </div>
                </div>
                <div class="display-stats" v-if="display_stats">
                    <table class="simple">
                        <tbody>
                            <tr>
                                <td>Mana</td>
                                <td>{{ display_stats.mana }}</td>
                            </tr>
                            <tr>
                                <td :class="[config.custom_stats.intellect ? 'active' : '']">
                                    Intellect
                                    <span v-if="config.custom_stats.intellect">
                                        ({{ $plusMinus(config.custom_stats.intellect) }})
                                        <tooltip position="t">{{ config.custom_stats.intellect }} bonus intellect</tooltip>
                                    </span>
                                </td>
                                <td>{{ display_stats.intellect }}</td>
                            </tr>
                            <tr>
                                <td :class="[config.custom_stats.spirit ? 'active' : '']">
                                    Spirit
                                    <span v-if="config.custom_stats.spirit">
                                        ({{ $plusMinus(config.custom_stats.spirit) }})
                                        <tooltip position="t">{{ config.custom_stats.spirit }} bonus spirit</tooltip>
                                    </span>
                                </td>
                                <td>{{ display_stats.spirit }}</td>
                            </tr>
                            <tr>
                                <td :class="[config.custom_stats.mp5 ? 'active' : '']">
                                    Mp5
                                    <span v-if="config.custom_stats.mp5">
                                        ({{ $plusMinus(config.custom_stats.mp5) }})
                                        <tooltip position="t">{{ config.custom_stats.mp5 }} bonus mp5</tooltip>
                                    </span>
                                </td>
                                <td>{{ display_stats.mp5 }}</td>
                            </tr>
                            <tr>
                                <td :class="[config.custom_stats.sp ? 'active' : '']">
                                    Spell power
                                    <span v-if="config.custom_stats.sp">
                                        ({{ $plusMinus(config.custom_stats.sp) }})
                                        <tooltip position="t">{{ config.custom_stats.sp }} bonus spell power</tooltip>
                                    </span>
                                </td>
                                <td>{{ display_stats.sp }}</td>
                            </tr>
                            <tr>
                                <td :class="[config.custom_stats.crit ? 'active' : '']">
                                    Crit %
                                    <span v-if="config.custom_stats.crit">
                                        ({{ $plusMinus(config.custom_stats.crit) }})
                                        <tooltip position="t">{{ config.custom_stats.crit }}% bonus crit</tooltip>
                                    </span>
                                </td>
                                <td>
                                    <span>{{ $round(display_stats.crit, 2) }}%</span>
                                </td>
                            </tr>
                            <tr>
                                <td :class="[config.custom_stats.hit ? 'active' : '']">
                                    Hit %
                                    <span v-if="config.custom_stats.hit">
                                        ({{ $plusMinus(config.custom_stats.hit) }})
                                        <tooltip position="t">{{ config.custom_stats.hit }}% bonus hit</tooltip>
                                    </span>
                                </td>
                                <td>
                                    <span>{{ $round(display_stats.hit, 2) }}%</span>
                                    <tooltip position="r">
                                        <div class="tal">
                                            <div v-if="config.talents.elemental_precision">+{{ config.talents.elemental_precision*2 }}% from Precision (fire/frost only)</div>
                                            <div v-if="config.talents.arcane_focus">+{{ config.talents.arcane_focus*2 }}% from Arcane Focus (arcane only)</div>
                                        </div>
                                    </tooltip>
                                </td>
                            </tr>
                        </tbody>
                    </table>
                    <div class="mt-1 tac">
                        <div class="btn small" @click="openCustomStats">
                            Add bonus stats
                        </div>
                    </div>
                </div>
                <div class="ep-stats" v-if="epCalc">
                    <div class="title">
                        <span>Stat weights</span>
                        <help>
                            Stat weights are calculated by running {{ config.iterations }} iterations with +{{ config.stat_weight_increment }} of each stat with the same RNG seed and comparing the dps gain.<br>
                            Calculated stat weights are based on your config. Any changes to it or your items can change the weights.<br>
                            The best way to find out if an item/enchant is better is to equip it and run simulations.
                        </help>
                    </div>
                    <select v-model="ep_weight">
                        <option value="dps">DPS</option>
                        <option value="intellect">Intellect (EP)</option>
                        <option value="spirit">Spirit (EP)</option>
                        <option value="mp5">Mp5 (EP)</option>
                        <option value="sp">Spell power (EP)</option>
                        <option value="crit">Crit % (EP)</option>
                        <option value="hit">Hit % (EP)</option>
                    </select>
                    <table class="simple mt-1">
                        <tbody>
                            <tr @click="ep_weight = 'intellect'">
                                <td>Intellect</td>
                                <td>{{ $nullRound(epCalc.intellect, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'spirit'">
                                <td>Spirit</td>
                                <td>{{ $nullRound(epCalc.spirit, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'mp5'">
                                <td>Mp5</td>
                                <td>{{ $nullRound(epCalc.mp5, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'sp'">
                                <td>Spell power</td>
                                <td>{{ $nullRound(epCalc.sp, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'crit'">
                                <td>Crit %</td>
                                <td>{{ $nullRound(epCalc.crit, 2) }}</td>
                            </tr>
                            <tr @click="ep_weight = 'hit'">
                                <td>Hit %</td>
                                <td>{{ $nullRound(epCalc.hit, 2) }}</td>
                            </tr>
                        </tbody>
                    </table>
                    <loader class="small mt-2" v-if="is_running" />
                </div>
                <div class="no-result mt-4" v-else-if="is_running">
                    <loader v-if="is_running" />
                </div>
                <div class="result" v-else-if="result">
                    <template v-if="result.iterations">
                        <div class="dps-result">
                            <div>DPS</div>
                            <div class="faded">{{ $round(result.min_dps, 2) }} - {{ $round(result.max_dps, 2) }}</div>
                            <div class="dps">{{ $round(result.avg_dps, 2) }}</div>
                            <div class="hps" v-if="result.avg_hps">{{ $round(result.avg_hps, 2) }} hps</div>
                            <div class="mb-1" v-if="result.t_gcd_capped >= 0.01 || result.n_oom">
                                <div class="faded" v-if="result.t_gcd_capped >= 0.01">
                                    <span>Wasted haste: {{ $round(result.t_gcd_capped, 2) }}s</span>
                                    <help>Time spent gcd capped</help>
                                </div>
                                <div class="faded warning" v-if="result.n_oom">
                                    <span>OOM: {{ $round(result.n_oom / result.iterations * 100) }}%</span>
                                    <help>
                                        Out of mana in {{ $round(result.n_oom) }} iterations<br>
                                        Common issues might be due to the number of Evocation ticks or timing of mana cooldowns.
                                    </help>
                                </div>
                            </div>
                        </div>
                        <div class="pinned" v-if="pin_dps">
                            <span class="update" @click="updatePin">
                                <span class="material-icons">&#xe5d5;</span>
                                <tooltip position="r">Update reference</tooltip>
                            </span>
                            <span class="diff" :class="[pin_dps > result.avg_dps ? 'lt' : 'gt']">
                                <template v-if="pin_dps <= result.avg_dps">+</template>{{ $roundFixed(result.avg_dps - pin_dps, 2) }}
                            </span>
                            <span class="remove" @click="removePin">
                                <span class="material-icons">&#xe5cd;</span>
                                <tooltip position="r">Remove reference</tooltip>
                            </span>
                        </div>
                        <div class="pin" v-else>
                            <span>
                                <span class="material-icons" @click="updatePin">&#xf10d;</span>
                                <tooltip position="r">Set reference</tooltip>
                            </span>
                        </div>
                        <div class="btn mt-2" v-if="!config.rng_seed" :class="[is_running ? 'disabled' : '']" @click="findAvg(result.avg_dps)">Find average fight</div>
                        <div class="btn mt-1" v-if="result.all_results" @click="allResults()">Simulation data</div>
                    </template>
                    <template v-else>
                        <div class="dps-result">
                            <div>DPS</div>
                            <div class="faded">Damage: {{ result.dmg }}</div>
                            <div class="dps">{{ $round(result.dps, 2) }}</div>
                            <div class="hps" v-if="result.hps">{{ $round(result.hps, 2) }} hps</div>
                            <div class="faded" v-if="result.t_gcd_capped">
                                <span>Wasted haste: {{ $round(result.t_gcd_capped, 2) }}s</span>
                                <help>Time spent gcd capped</help>
                            </div>
                            <div class="faded warning" v-if="result.t_oom">
                                <span>OOM at {{ $round(result.t_oom, 2) }}s</span>
                                <help>
                                    Ran out of mana at {{ $round(result.t_oom, 2) }}<br>
                                    Common issues might be the number of Evocation ticks or the timing of mana cooldowns.
                                </help>
                            </div>
                        </div>
                    </template>

                    <div class="warnings mt-2">
                        <div class="warning" v-if="numProfs() > 2">
                            <span class="material-icons">&#xe002;</span>
                            <tooltip position="right">You have selected bonuses from {{ numProfs() }} professions</tooltip>
                        </div>
                    </div>
                </div>
                <a class="github" href="https://github.com/Cheesehyvel/magesim-sod" target="_blank"></a>
                <div class="donate">
                    <a href="https://www.paypal.com/donate/?hosted_button_id=CU9RF4LCMW8W6" target="_blank">
                        Donate
                    </a>
                </div>
            </div>
            <div class="main">
                <div class="tabs">
                    <div class="tab" :class="{active: active_tab == 'gear'}" @click="setTab('gear')">Gear</div>
                    <div class="tab" :class="{active: active_tab == 'config'}" @click="setTab('config')">Config</div>
                    <div class="tab" @click="openImport()">Import</div>
                    <div class="tab" @click="openExport()">Export</div>
                    <template v-if="history.length">
                        <div class="tab" :class="{active: active_tab == 'history'}" @click="setTab('history')">History</div>
                    </template>
                    <template v-if="result && !result.iterations">
                        <div class="tab" :class="{active: active_tab == 'log'}" @click="setTab('log')">Combat log</div>
                        <div class="tab" :class="{active: active_tab == 'timeline'}" @click="setTab('timeline')">Timeline</div>
                    </template>
                    <template v-if="result && result.iterations">
                        <div class="tab" :class="{active: active_tab == 'histogram'}" @click="setTab('histogram')">Histogram</div>
                    </template>
                    <template v-if="result && result.hasOwnProperty('spells')">
                        <div class="tab" :class="{active: active_tab == 'spells'}" @click="setTab('spells')">Spells</div>
                    </template>
                </div>
                <div class="body">
                    <div class="gear" :class="{splitview}" v-if="active_tab == 'gear'">
                        <div class="slots">
                            <div
                                class="slot"
                                :class="[active_slot == slot ? 'active' : '']"
                                v-for="slot in slots"
                                @click="setActiveSlot(slot);"
                            >{{ formatKey(slot) }}</div>
                            <div class="btn btn-splitview" @click="toggleSplitview">
                                <template v-if="splitview">
                                    <span>Paperdoll</span>
                                    <span class="material-icons">&#xe5e1;</span>
                                </template>
                                <template v-else>
                                    <span class="material-icons">&#xe5e0;</span>
                                    <span>Paperdoll</span>
                                </template>
                            </div>
                        </div>
                        <div class="gear-wrapper">
                            <div class="items" ref="items">
                                <div class="items-wrapper">
                                    <div class="top clearfix">
                                        <div class="fl clearfix">
                                            <div class="form-item text-search">
                                                <input type="text" ref="search" v-model="search_item" placeholder="Search..." @input="onSearch">
                                                <tooltip position="bl">
                                                    <b>Text search with filters</b><br><br>
                                                    Yes/no filters: pvp, 2h, set<br>
                                                    Example: 'pvp:no' will exclude all pvp items.<br><br>
                                                    Number filters: phase, ilvl, sp, crit, hit, int, spi, mp5<br>
                                                    Example: 'ilvl:252+' will find items with ilvl 252 or higher.<br>
                                                    Example: 'ilvl:252-' will find items with ilvl 252 or lower.<br>
                                                    Example: 'ilvl:239-252' will find items with ilvl between 239 and 252.
                                                </tooltip>
                                            </div>
                                        </div>
                                        <div class="fr">
                                            <div class="btn" :class="[!hasComparisons || is_running ? 'disabled' : '']" @click="runComparison">
                                                Run item comparison
                                            </div>
                                            <div class="btn ml-n" @click="openEquiplist">
                                                Equipped items overview
                                            </div>
                                            <div class="btn ml-n" @click="openCustomItem">
                                                Add custom item
                                            </div>
                                        </div>
                                    </div>

                                    <table class="items-table large mt-2">
                                        <thead>
                                            <tr>
                                                <th class="min">
                                                    <span class="compare" @click.stop="compareAll()" v-if="activeItems">
                                                        <help icon="e915">Compare all items</help>
                                                    </span>
                                                </th>
                                                <th class="min"></th>
                                                <th class="title">
                                                    <sort-link v-model="item_sort" name="title">Name</sort-link>
                                                </th>
                                                <th v-if="hasComparisons">
                                                    <sort-link v-model="item_sort" name="dps" order="desc">DPS</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="ilvl" order="desc">ilvl</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="phase">{{ splitviewShort("Phase", "P") }}</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="sp" order="desc">{{ splitviewShort("Spell power", "SP") }}</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="crit" order="desc">{{ splitviewShort("Crit %", "Crit%") }}</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="hit" order="desc">{{ splitviewShort("Hit %", "Hit%") }}</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="int" order="desc">{{ splitviewShort("Intellect", "Int") }}</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="spi" order="desc">{{ splitviewShort("Spirit", "Spi") }}</sort-link>
                                                </th>
                                                <th>
                                                    <sort-link v-model="item_sort" name="mp5" order="desc">Mp5</sort-link>
                                                </th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <tr
                                                class="item"
                                                :class="[isEquipped(active_slot, item.id) ? 'active' : '']"
                                                v-for="item in activeItems"
                                                @click="equipToggle(active_slot, item)"
                                                :key="item.id"
                                            >
                                                <td class="min">
                                                    <span class="compare" :class="[isComparing(item) ? 'active' : '']" @click.stop="compareItem(item)">
                                                        <help icon="e915">Add to comparison</help>
                                                    </span>
                                                </td>
                                                <td class="min">
                                                    <span class="favorite" :class="[isFavorite(item) ? 'active' : '']" @click.stop="toggleFavorite(item)">
                                                        <help icon="e87d" v-if="isFavorite(item)">Favorite</help>
                                                        <help icon="e87d" :outlined="true" v-else>Favorite</help>
                                                    </span>
                                                </td>
                                                <td class="title">
                                                    <a :href="itemUrl(item)" :class="['quality-'+$get(item, 'q', 'epic')]" target="_blank" @click.prevent>
                                                        {{ item.title }}
                                                    </a>
                                                    <span class="link" @click.stop="openItem(item)">
                                                        <span class="material-icons">
                                                            &#xe895;
                                                        </span>
                                                    </span>
                                                    <span class="delete" @click.stop="deleteCustomItem(item)" v-if="$get(item, 'custom')">
                                                        <help icon="e872">Delete custom item</help>
                                                    </span>
                                                </td>
                                                <td v-if="hasComparisons">
                                                    <template v-if="comparisonDps(item)">
                                                        {{ comparisonDps(item) }}
                                                        <span v-if="pin_dps" class="diff" :class="[pin_dps > comparisonDps(item) ? 'lt' : 'gt']">
                                                            (<template v-if="pin_dps <= comparisonDps(item)">+</template>{{ $roundFixed(comparisonDps(item) - pin_dps, 2) }})
                                                        </span>
                                                    </template>
                                                </td>
                                                <td>{{ $get(item, "ilvl", "") }}</td>
                                                <td>{{ $get(item, "phase", 1) }}</td>
                                                <td>{{ formatSP(item) }}</td>
                                                <td>{{ $get(item, "crit", "") }}</td>
                                                <td>{{ $get(item, "hit", "") }}</td>
                                                <td>{{ $get(item, "int", "") }}</td>
                                                <td>{{ $get(item, "spi", "") }}</td>
                                                <td>{{ $get(item, "mp5", "") }}</td>
                                            </tr>
                                        </tbody>
                                    </table>

                                    <div class="mt-4" ref="enchantAnchor"></div>

                                    <table class="enchants-table large" v-if="activeEnchants.length">
                                        <thead>
                                            <tr>
                                                <th>Enchant</th>
                                                <th>Spell power</th>
                                                <th>Crit %</th>
                                                <th>Hit %</th>
                                                <th>Intellect</th>
                                                <th>Spirit</th>
                                                <th>Mp5</th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <tr
                                                class="item"
                                                :class="[isEnchanted(active_slot, item.id) ? 'active' : '']"
                                                v-for="item in activeEnchants"
                                                :key="item.id"
                                                @click="enchant(active_slot, item)"
                                            >
                                                <td>
                                                    <a :href="spellUrl(item)" :class="['quality-'+$get(item, 'q', 'uncommon')]" target="_blank" @click.prevent>
                                                        {{ item.title }}
                                                    </a>
                                                </td>
                                                <td>{{ $get(item, "sp", "") }}</td>
                                                <td>{{ $get(item, "crit", "") }}</td>
                                                <td>{{ $get(item, "hit", "") }}</td>
                                                <td>{{ $get(item, "int", "") }}</td>
                                                <td>{{ $get(item, "spi", "") }}</td>
                                                <td>{{ $get(item, "mp5", "") }}</td>
                                            </tr>
                                        </tbody>
                                    </table>
                                </div>
                            </div>
                            <div class="character" v-if="splitview">
                                <div class="paperdoll">
                                    <div :class="pos" v-for="pos in ['left', 'right']">
                                        <div class="paperslot" :class="[slot, active_slot == slot ? 'active' : '']" v-for="slot in dollSlots(pos)">
                                            <div class="paperv paperitem" @click="paperClick(slot)">
                                                <a
                                                    v-if="equipped[slot]"
                                                    :href="equippedUrl(slot)"
                                                    data-wh-icon-size="large"
                                                    @click="$event.preventDefault()"
                                                ></a>
                                            </div>
                                            <div class="papers">
                                                <div class="paperv paperenchant" v-if="items.enchants.hasOwnProperty(equipSlotToItemSlot(slot)) && pos == 'left'" @click="paperClick(slot, 'enchant')">
                                                    <a
                                                        v-if="enchants[slot]"
                                                        :href="spellUrl(enchants[slot])"
                                                        data-wh-icon-size="large"
                                                        @click="$event.preventDefault()"
                                                    ></a>
                                                </div>
                                                <div class="paperv paperenchant" v-if="items.enchants.hasOwnProperty(equipSlotToItemSlot(slot)) && pos == 'right'" @click="paperClick(slot, 'enchant')">
                                                    <a
                                                        v-if="enchants[slot]"
                                                        :href="spellUrl(enchants[slot])"
                                                        data-wh-icon-size="large"
                                                        @click="$event.preventDefault()"
                                                    ></a>
                                                </div>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </div>
                        </div>
                    </div>

                    <div class="log" v-if="active_tab == 'log'">
                        <div class="log-wrapper">
                            <div class="filter">
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter[1]"> <span>Show cast start</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter[2]"> <span>Show cast success</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter[3]"> <span>Show spell impact</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter[4]"> <span>Show mana gain</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter[5]"> <span>Show buffs</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter[8]"> <span>Show wait</span></label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="log_filter_player"> <span>Show player only</span></label>
                                </div>
                            </div>
                            <div class="form-item mb-2">
                                <input type="text" v-model="search_log" placeholder="Search...">
                            </div>
                            <table>
                                <thead>
                                    <th>Time</th>
                                    <th>Mana</th>
                                    <th>DPS</th>
                                    <th>Event</th>
                                </thead>
                                <tbody>
                                    <tr v-for="log in activeLog" :class="['type-'+log.type]">
                                        <td>{{ formatTime(log.t) }}</td>
                                        <td>{{ round(log.mana) }} ({{ round(log.mana_percent) }}%)</td>
                                        <td>{{ (log.t ? round(log.dmg/log.t) : "0") }}</td>
                                        <td>{{ log.text }}</td>
                                    </tr>
                                </tbody>
                            </table>
                        </div>
                    </div>

                    <div class="timel" v-if="active_tab == 'timeline'">
                        <timeline ref="timeline" :result="result"></timeline>
                    </div>

                    <div class="spells" v-if="active_tab == 'spells'">
                        <div class="spells-wrapper">
                            <table class="large">
                                <thead>
                                    <th>Caster</th>
                                    <th>Spell</th>
                                    <th>Casts</th>
                                    <th>Misses</th>
                                    <th>Hits</th>
                                    <th>Crits</th>
                                    <th>DPS</th>
                                    <th>Min dmg</th>
                                    <th>Avg dmg</th>
                                    <th>Max dmg</th>
                                </thead>
                                <tbody>
                                    <tr v-for="spell in spellStats">
                                        <td>{{ spell.source }}</td>
                                        <td>{{ spell.name }}</td>
                                        <td>{{ $round(spell.casts, 1) }} ({{ $round(spell.casts / numCasts * 100, 1) }}%)</td>
                                        <td>{{ $round(spell.misses, 1) }} ({{ $round(spell.misses/(spell.hits + spell.crits + spell.misses)*100, 2) }}%)</td>
                                        <td>{{ $round(spell.hits, 1) }}</td>
                                        <td>{{ $round(spell.crits, 1) }} ({{ $round(spell.crits/(spell.hits + spell.crits + spell.misses)*100, 2) }}%)</td>
                                        <td>
                                            <template v-if="result.hasOwnProperty('t')">{{ $round(spell.dmg / result.t, 2) }}</template>
                                            <template v-else>{{ $round(spell.dmg / spellDmg * result.avg_dps, 2) }}</template>
                                            &nbsp;
                                            <template v-if="result.hasOwnProperty('dmg')">({{ $round(spell.dmg / result.dmg * 100, 2) }}%)</template>
                                            <template v-else>({{ $round(spell.dmg / spellDmg * 100, 2) }}%)</template>
                                        </td>
                                        <td>{{ $round(spell.min_dmg, 0) }}</td>
                                        <td>{{ $round(spell.dmg / (spell.hits + spell.crits), 0) }}</td>
                                        <td>{{ $round(spell.max_dmg, 0) }}</td>
                                    </tr>
                                </tbody>
                            </table>
                        </div>
                    </div>

                    <div class="histog" v-if="active_tab == 'histogram'">
                        <histogram ref="histogram" :data="result.histogram" :avg="result.avg_dps"></histogram>
                    </div>

                    <div class="history" v-if="active_tab == 'history'">
                        <div class="history-wrapper">
                            <table class="history-table large">
                                <thead>
                                    <th></th>
                                    <th>DPS</th>
                                    <th>Min/Max</th>
                                    <th>Rotation</th>
                                    <th>Duration</th>
                                    <th>Iterations</th>
                                    <th>Execution time</th>
                                    <th>Time</th>
                                    <th></th>
                                </thead>
                                <tbody>
                                    <tr v-for="profile in history">
                                        <td>
                                            <div class="btn small my-n" @click="loadHistory(profile)">Load profile</div>
                                        </td>
                                        <td>
                                            <template v-if="profile.result">
                                                <b>{{ $round(profile.result.avg_dps, 2) }}</b>
                                            </template>
                                            <template v-else>-</template>
                                        </td>
                                        <td>
                                            <template v-if="profile.result">
                                                {{ $round(profile.result.min_dps) }} - {{ $round(profile.result.max_dps) }}
                                            </template>
                                            <template v-else>-</template>
                                        </td>
                                        <td>
                                            {{ getRotationString(profile.config.rotation) }}
                                        </td>
                                        <td>
                                            {{ profile.config.duration }}
                                            <span class="faded" v-if="profile.config.duration_variance"> &#177;{{ profile.config.duration_variance }}</span>
                                        </td>
                                        <td>{{ profile.config.iterations }}</td>
                                        <td>
                                            <template v-if="profile.end">{{ formatTimeDiff(profile.date, profile.end) }}</template>
                                            <template v-else>-</template>
                                        </td>
                                        <td>
                                            {{ formatDateTime(profile.date) }}
                                        </td>
                                        <td>
                                            <span v-if="profile.result && profile.result.all_results" class="btn small my-n" @click="allResults(profile.result)">
                                                Simulation data
                                            </span>
                                        </td>
                                    </tr>
                                </tbody>
                            </table>
                        </div>
                    </div>

                    <div class="config" v-if="active_tab == 'config'">
                        <div class="fieldsets">
                            <fieldset class="config-general">
                                <legend>General</legend>
                                <div class="form-item">
                                    <label>Quick spec</label>
                                    <span class="btn secondary" @click="setSpec('arcane')">Arcane</span>
                                    <span class="btn secondary" @click="setSpec('fire')">Fire</span>
                                    <span class="btn secondary" @click="setSpec('frost')">Frost</span>
                                    <span class="btn secondary" @click="setSpec('arcane_heal')">Healing</span>
                                </div>
                                <div class="form-item">
                                    <label>Race</label>
                                    <select v-model="config.race">
                                        <option :value="races.RACE_GNOME">Gnome</option>
                                        <option :value="races.RACE_HUMAN">Human</option>
                                        <option :value="races.RACE_TROLL">Troll</option>
                                        <option :value="races.RACE_UNDEAD">Undead</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label>Talents (<a :href="config.build" target="_blank">link</a>)</label>
                                    <input type="text" v-model="config.build" @input="onBuildInput">
                                </div>
                                <div class="form-item">
                                    <label>Number of sims</label>
                                    <input type="text" v-model.number="config.iterations">
                                </div>
                                <div class="form-item form-row">
                                    <div class="form-item">
                                        <label>Fight duration (sec)</label>
                                        <input type="text" v-model.number="config.duration">
                                    </div>
                                    <div class="form-item">
                                        <label>Variance +/-</label>
                                        <input type="text" v-model.number="config.duration_variance">
                                    </div>
                                </div>
                                <div class="form-item form-row">
                                    <div class="form-item">
                                        <label>Target level</label>
                                        <select v-model="config.target_level">
                                            <option :value="28">28</option>
                                            <option :value="27">27</option>
                                            <option :value="26">26</option>
                                            <option :value="25">25</option>
                                        </select>
                                    </div>
                                    <div class="form-item">
                                        <label>
                                            <span>Target hp%</span>
                                            <help>Starting health for all targets. The health will go down over time.</help>
                                        </label>
                                        <input type="text" v-model.number="config.target_hp">
                                    </div>
                                </div>
                                <div class="form-item form-row">
                                    <div class="form-item">
                                        <label>No. of targets</label>
                                        <input type="text" v-model.number="config.targets">
                                    </div>
                                    <div class="form-item">
                                        <label>
                                            <span>No. of dot targets</span>
                                            <help>
                                                Number of targets to multidot.<br>
                                                Leave this empty or set to 0 to use the same as No. of targets.
                                            </help>
                                        </label>
                                        <input type="text" v-model.number="config.dot_targets">
                                    </div>
                                </div>
                                <div class="form-item">
                                    <label>
                                        <span>Distance from target</span>
                                        <help>This only affects travel time.<br>No range checks are made.</help>
                                    </label>
                                    <input type="text" v-model.number="config.distance">
                                </div>
                                <div class="form-item">
                                    <label>
                                        <span>Reaction time (ms)</span>
                                        <help>
                                            This only affects certain mechanics where an instant reaction can have an important impact on the rotation, like:<br>
                                            Cooldowns with "wait for"<br>
                                            Missile Barrage<br>
                                            Hot Streak<br>
                                            Brain Freeze
                                        </help>
                                    </label>
                                    <input type="text" v-model.number="config.reaction_time">
                                </div>
                                <div class="form-item" v-if="false">
                                    <label>
                                        <span>Reaction time (ms)</span>
                                        <help>Affects cooldown usage when waiting for procs</help>
                                    </label>
                                    <input type="text" v-model.number="config.reaction_time">
                                </div>
                                <div class="form-item">
                                    <label>
                                        <span>RNG seed</span>
                                        <help>
                                            A number above 0 will give all runs the same random seed.<br>
                                            All iterations in the same run will still have different seeds.<br>
                                            This might be useful for certain analysis.
                                        </help>
                                    </label>
                                    <input type="text" v-model.number="config.rng_seed">
                                </div>
                                <div class="form-item">
                                    <label>
                                        <span>Stat weight increment</span>
                                        <help>
                                            Each stat will be increased by this value when calculating stat weights.
                                        </help>
                                    </label>
                                    <input type="text" v-model.number="config.stat_weight_increment">
                                </div>
                                <div class="form-item" v-if="config.targets > 1">
                                    <label><input type="checkbox" v-model="config.only_main_dmg">
                                        <span>Focus dps on main target</span>
                                        <help>
                                            This will ignore damage done to secondary targets in the result.<br>
                                            When in doubt it will focus damage on the main target rather than maximizing overall damage.<br>
                                            This is only applicable if the numbers of targets is more than 1.
                                        </help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.avg_spell_dmg">
                                        <span>Use average spell damage</span>
                                        <help>
                                            This will eliminate the random damage from spells.<br>
                                            This can be useful to verify calculations.
                                        </help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.additional_data">
                                        <span>Additional data</span>
                                        <help>
                                            This will save data about dps and duration for each simulation.<br>
                                            This will use more memory and can cause performance issues with a high number of sims.
                                        </help>
                                    </label>
                                </div>
                                <div class="form-item" v-if="false">
                                    <label><input type="checkbox" v-model="config.encounters">
                                        <span>Enable encounters</span>
                                        <help>
                                            This will enable certain buffs/debuffs from encounters.<br>
                                            They can be found under cooldowns.
                                        </help>
                                    </label>
                                </div>
                            </fieldset>
                            <fieldset class="config-rotation">
                                <legend>Rotation</legend>
                                <div class="form-item">
                                    <label>Main rotation</label>
                                    <select v-model="config.rotation">
                                        <option :value="rotations.ROTATION_ST_ARCANE">Arcane</option>
                                        <option :value="rotations.ROTATION_ST_FIRE">Fire</option>
                                        <option :value="rotations.ROTATION_ST_FIRE_SC">Fire (Scorch)</option>
                                        <option :value="rotations.ROTATION_ST_FROST">Frost</option>
                                        <option :value="rotations.ROTATION_AOE_AE">Arcane Explosion</option>
                                        <option :value="rotations.ROTATION_AOE_AE_FS">Flamestrike > Arcane Explosion</option>
                                        <option :value="rotations.ROTATION_AOE_BLIZZ">Blizzard</option>
                                        <option :value="rotations.ROTATION_AOE_BLIZZ_FS">Flamestrike > Blizzard</option>
                                        <option :value="rotations.ROTATION_AOE_FIRE">Fire AoE</option>
                                        <option :value="rotations.ROTATION_AOE_FS">Flamestrike</option>
                                    </select>
                                </div>
                                <div class="form-item" v-if="config.talents.imp_scorch">
                                    <label><input type="checkbox" v-model="config.maintain_imp_scorch">
                                        <span>Keep up imp. scorch</span>
                                        <help>Imp. Scorch from you</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label>
                                        <span>Scorching mages</span>
                                        <help>Not counting yourself</help>
                                    </label>
                                    <input type="text" v-model.number="config.scorching_mages">
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.rot_fire_blast_weave">
                                        <span>Fire Blast Weave</span>
                                    </label>
                                </div>
                                <template v-if="config.rotation == rotations.ROTATION_ST_FROST">
                                    <div class="form-item" v-if="config.runes.ice_lance && config.runes.fingers_of_frost">
                                        <label><input type="checkbox" v-model="config.rot_ice_lance">
                                            <span>Ice Lance at end of Fingers of Frost</span>
                                        </label>
                                    </div>
                                </template>
                            </fieldset>
                            <fieldset class="config-debuffs">
                                <legend>Debuffs</legend>
                                <div class="form-item" v-if="false">
                                    <label><input type="checkbox" v-model="config.curse_of_elements">
                                        <span>Curse of Elements</span>
                                        <help>6% fire/frost dmg, -45 resistance</help>
                                    </label>
                                </div>
                                <div class="form-item" v-if="false">
                                    <label><input type="checkbox" v-model="config.curse_of_shadow">
                                        <span>Curse of Shadow</span>
                                        <help>8% arcane dmg, -60 resistance</help>
                                    </label>
                                </div>
                            </fieldset>
                            <fieldset class="config-buffs">
                                <legend>Buffs</legend>
                                <div class="form-item">
                                    <label><input type="checkbox" :checked="true" :disabled="true">
                                        <span>Arcane Intellect</span>
                                        <help>7 intellect</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.mark_of_the_wild">
                                        <span>Mark of the Wild</span>
                                        <help>4 stats</help>
                                    </label>
                                </div>
                                <div class="form-item sub" v-if="config.mark_of_the_wild">
                                    <label><input type="checkbox" v-model="config.imp_mark_of_the_wild">
                                        <span class="material-icons">&#xe5da;</span>
                                        <span>Imp. Mark of the Wild</span>
                                        <help>5 stats instead of 4</help>
                                    </label>
                                </div>
                                <template v-if="faction == 'alliance'">
                                    <div class="form-item">
                                        <label><input type="checkbox" v-model="config.blessing_of_wisdom">
                                            <span>Blessing of Wisdom</span>
                                            <help>12 mp5</help>
                                        </label>
                                    </div>
                                    <div class="form-item sub" v-if="config.blessing_of_wisdom">
                                        <label><input type="checkbox" v-model="config.imp_blessing_of_wisdom">
                                            <span class="material-icons">&#xe5da;</span>
                                            <span>Imp. Blessing of Wisdom</span>
                                            <help>15 mp5 instead of 12</help>
                                        </label>
                                    </div>
                                </template>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.blessing_of_kings">
                                        <span>10% stats</span>
                                        <help>Blessing of Kings<br>Aspect of the Lion</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label>
                                        <input type="checkbox" v-model="config.demonic_pact">
                                        <span>Demonic Pact</span>
                                    </label>
                                </div>
                                <div class="form-item" v-if="config.demonic_pact">
                                    <label>
                                        <span>Demonic Pact Bonus</span>
                                        <help>10% of the Warlocks spell power or level/2.</help>
                                    </label>
                                    <input type="text" v-model.number="config.demonic_pact_bonus">
                                </div>
                                <div class="form-item" v-if="faction == 'horde'">
                                    <label><input type="checkbox" v-model="config.rising_spirit">
                                        <span>Rising Spirit</span>
                                        <help>25 spirit for 30min. Horde only</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.boon_blackfathom">
                                        <span>Boon of Blackfathom</span>
                                        <help>2% crit</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.ashenvale_cry">
                                        <span>Ashenvale Rallying Cry</span>
                                        <help>5% dmg/heal from Ashenvale pvp objective</help>
                                    </label>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.dmf_dmg">
                                        <span>Sayge's Dark Fortune of Damage</span>
                                        <help>10% dmg from Darkmoon Faire</help>
                                    </label>
                                </div>
                            </fieldset>
                            <fieldset class="config-consumes">
                                <legend>Consumes</legend>
                                <div class="form-item" v-if="false">
                                    <label>Flask</label>
                                    <select v-model="config.flask">
                                        <option :value="flasks.FLASK_NONE">None</option>
                                        <option :value="flasks.FLASK_SUPREME_POWER">Supreme Power (150 sp)</option>
                                        <option :value="flasks.FLASK_DISTILLED_WISDOM">Distilled Wisdom (2000 mana)</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label>Food</label>
                                    <select v-model="config.food">
                                        <option :value="foods.FOOD_NONE">None</option>
                                        <option :value="foods.FOOD_SPIRIT8">+8 Spirit/stam</option>
                                        <option :value="foods.FOOD_SPIRIT6">+6 Spirit/stam</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label>Weapon oil</label>
                                    <select v-model="config.weapon_oil">
                                        <option :value="weapon_oils.OIL_NONE">None</option>
                                        <option :value="weapon_oils.OIL_BLACKFATHOM">Blackfathom Oil (12 mp5 / 2% hit)</option>
                                        <!-- <option :value="weapon_oils.OIL_BRILLIANT_WIZARD">Brilliant Wizard Oil (36 sp / 1% crit)</option> -->
                                        <!-- <option :value="weapon_oils.OIL_WIZARD">Wizard Oil (24 sp)</option> -->
                                        <!-- <option :value="weapon_oils.OIL_LESSER_WIZARD">Lesser Wizard Oil (16 sp)</option> -->
                                        <option :value="weapon_oils.OIL_MINOR_WIZARD">Minor Wizard Oil (8 sp)</option>
                                        <!-- <option :value="weapon_oils.OIL_BRILLIANT_MANA">Brilliant Mana Oil (12 mp5)</option> -->
                                        <!-- <option :value="weapon_oils.OIL_LESSER_MANA">Lesser Mana Oil (8 mp5)</option> -->
                                        <option :value="weapon_oils.OIL_MINOR_MANA">Minor Mana Oil (4 mp5)</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label>Arcane scroll</label>
                                    <select v-model="config.mage_scroll">
                                        <option :value="mage_scrolls.SCROLL_NONE">None</option>
                                        <option :value="mage_scrolls.SCROLL_ACCURACY1">Accuracy (1% hit)</option>
                                        <option :value="mage_scrolls.SCROLL_POWER1">Power (1% crit)</option>
                                        <option :value="mage_scrolls.SCROLL_RECOVERY1">Recovery (8 mp5)</option>
                                    </select>
                                </div>
                                <div class="form-item">
                                    <label><input type="checkbox" v-model="config.elixir_firepower">
                                        <span>Elixir of Firepower</span>
                                        <help>10 fire spell power</help>
                                    </label>
                                </div>
                            </fieldset>
                            <fieldset class="config-precombat">
                                <legend>Pre-combat</legend>
                                <div class="form-item" v-if="!aoeRotation">
                                    <label><input type="checkbox" v-model="config.pre_cast">
                                        <span>Pre-cast main spell</span>
                                    </label>
                                </div>
                            </fieldset>
                            <fieldset class="config-cooldowns">
                                <legend>Cooldowns</legend>
                                <div class="timings">
                                    <table class="items">
                                        <thead>
                                            <tr>
                                                <th class="icon">CD</th>
                                                <th class="t">Pop at</th>
                                                <th class="wait-for-buff">Wait for</th>
                                                <th class="wait-t">Wait max</th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <tr v-for="timing in config.timings" :key="timing.id">
                                                <template v-if="timingEnabled(timing.name)">
                                                    <td class="icon">
                                                        <span>
                                                            <img :src="getTiming(timing.name, 'icon')">
                                                            <tooltip>{{ getTiming(timing.name, 'title') }}</tooltip>
                                                        </span>
                                                        <div class="remove" @click="removeTiming(timing.id)">
                                                            <span class="material-icons">&#xe5cd;</span>
                                                        </div>
                                                    </td>
                                                    <td class="t">
                                                        <input type="text" v-model.number="timing.t">
                                                    </td>
                                                    <td class="wait-for-buff">
                                                        <select v-model="timing.wait_for_buff" v-if="timingCanWait(timing.name)">
                                                            <option :value="0">Nothing</option>
                                                            <option v-for="buff in waitBuffs" :value="buff.id" :key="buff.id">{{ buff.name }}</option>
                                                        </select>
                                                    </td>
                                                    <td class="wait-t">
                                                        <input type="text" v-model.number="timing.wait_t" v-if="timingCanWait(timing.name)">
                                                    </td>
                                                </template>
                                            </tr>
                                        </tbody>
                                    </table>
                                    <div class="add-timing">
                                        <div class="plus">
                                            <span class="material-icons">&#xe145;</span>
                                        </div>
                                        <div class="menu">
                                            <template v-for="timing in timings">
                                                <div v-if="timingEnabled(timing.name)" @click="addTiming(timing.name)">
                                                    <img :src="timing.icon">
                                                    <tooltip>{{ timing.title }}</tooltip>
                                                </div>
                                            </template>
                                        </div>
                                    </div>
                                </div>
                            </fieldset>
                            <fieldset class="config-interruptions">
                                <legend>Interruptions</legend>
                                <div class="interruptions">
                                    <table class="items">
                                        <thead>
                                            <tr>
                                                <th class="type">Type</th>
                                                <th class="affects">Affects</th>
                                                <th class="t">Time</th>
                                                <th class="duration">Duration</th>
                                            </tr>
                                        </thead>
                                        <tbody>
                                            <tr v-for="interruption in config.interruptions" :key="interruption.id">
                                                <td class="type">
                                                    <select v-model="interruption.silence">
                                                        <option :value="false">Movement</option>
                                                        <option :value="true">Silence</option>
                                                    </select>
                                                    <div class="remove" @click="removeInterruption(interruption.id)">
                                                        <span class="material-icons">&#xe5cd;</span>
                                                    </div>
                                                </td>
                                                <td class="affects">
                                                    <select v-model="interruption.affects_all">
                                                        <option :value="false">Player</option>
                                                        <option :value="true">Player and pets</option>
                                                    </select>
                                                </td>
                                                <td class="t">
                                                    <input type="text" v-model.number="interruption.t">
                                                </td>
                                                <td class="t">
                                                    <input type="text" v-model.number="interruption.duration">
                                                </td>
                                            </tr>
                                        </tbody>
                                    </table>
                                    <div class="add-timing">
                                        <div class="plus" @click="addInterruption">
                                            <span class="material-icons">&#xe145;</span>
                                        </div>
                                    </div>
                                </div>
                            </fieldset>
                            <fieldset class="config-profiles">
                                <legend>Your profiles</legend>
                                <div class="profiles">
                                    <div class="profile" v-for="(profile, index) in profiles" :key="profile.id">
                                        <div class="name" @click="loadProfile(profile)">{{ profile.name }}</div>
                                        <div class="actions">
                                            <div class="move move-up" @click="moveProfile(index, -1)">
                                                <span class="material-icons">&#xe316;</span>
                                                <tooltip position="t">Move up</tooltip>
                                            </div>
                                            <div class="move move-down" @click="moveProfile(index, 1)">
                                                <span class="material-icons">&#xe313;</span>
                                                <tooltip position="t">Move down</tooltip>
                                            </div>
                                            <div class="load-items" @click="loadProfile(profile, 'items')">
                                                <span class="material-icons">&#xe84e;</span>
                                                <tooltip position="t">Load items only</tooltip>
                                            </div>
                                            <div class="load-config" @click="loadProfile(profile, 'config')">
                                                <span class="material-icons">&#xe8b8;</span>
                                                <tooltip position="t">Load config only</tooltip>
                                            </div>
                                            <div class="save" @click="saveProfile(profile)">
                                                <span class="material-icons">&#xe161;</span>
                                                <tooltip position="t">Save profile</tooltip>
                                            </div>
                                            <div class="delete" @click="deleteProfile(profile)">
                                                <span class="material-icons">&#xe872;</span>
                                                <tooltip position="t">Delete profile</tooltip>
                                            </div>
                                        </div>
                                    </div>
                                    <div class="new-profile mt-1">
                                        <input type="text" v-model="new_profile" placeholder="Enter the name of your new profile" @keydown.enter="newProfile()">
                                        <div class="btn" :class="[new_profile ? '' : 'disabled']" @click="newProfile()">
                                            <span>
                                                New profile
                                                <tooltip position="r">Save your items and config</tooltip>
                                            </span>
                                        </div>
                                    </div>
                                    <div class="export-import clearfix mt-2">
                                        <div class="btn fl" @click="openExport()">Export</div>
                                        <div class="btn fl ml-n" @click="openImport()">Import</div>
                                        <div class="btn danger fr" @click="nukeSettings()">Nuke settings</div>
                                    </div>
                                </div>
                            </fieldset>
                            <fieldset class="config-quick-profiles">
                                <legend>Presets</legend>
                                <div class="profiles">
                                    <div class="profile" v-for="(profile, index) in default_profiles" :key="profile.id">
                                        <div class="name" @click="loadProfile(profile)">{{ profile.name }}</div>
                                        <div class="actions">
                                            <div class="load-items" @click="loadProfile(profile, 'items')">
                                                <span class="material-icons">&#xe84e;</span>
                                                <tooltip position="t">Load items only</tooltip>
                                            </div>
                                            <div class="load-config" @click="loadProfile(profile, 'config')">
                                                <span class="material-icons">&#xe8b8;</span>
                                                <tooltip position="t">Load config only</tooltip>
                                            </div>
                                        </div>
                                    </div>
                                </div>
                            </fieldset>
                        </div>
                    </div>
                </div>
            </div>

            <div class="lightbox small warning" v-if="beta_warning_open">
                <div class="closer" @click="closeBetaWarning(false)"></div>
                <div class="inner">
                    <div class="title">BETA</div>
                    <div class="text">This sim is under construction. Results may not be accurate as we discover more things in the SoD.</div>
                    <div class="text left mt-2">
                        <ul>
                            <li>Rotations are very basic and not optimized</li>
                            <li>Ignite is personal and works like in WotLK</li>
                            <li>Healing is not shown in graph or spell overview</li>
                            <li>Many items are missing. Use Custom Items for now</li>
                            <li>Imports might be buggy</li>
                        </ul>
                    </div>
                    <div class="btn mt-2" @click="closeBetaWarning(true)">I understand</div>
                </div>
            </div>

            <div class="lightbox" v-if="export_profile.open">
                <div class="closer" @click="closeExport"></div>
                <div class="inner">
                    <div class="title">Export</div>
                    <div class="form-item">
                        <textarea v-model="export_profile.string" ref="export_input"></textarea>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="export_profile.items" @input="updateExport"> <span>Include items</span></label>
                        <label><input type="checkbox" v-model="export_profile.config" @input="updateExport"> <span>Include config</span></label>
                    </div>
                    <div class="btn mt-2 wide" @click="closeExport">Close</div>
                </div>
            </div>

            <div class="lightbox" v-if="import_profile.open">
                <div class="closer" @click="closeImport"></div>
                <div class="inner">
                    <div class="title">Import</div>
                    <div class="description">
                        Paste the data from any of these supported sources:<br>
                        MageSim, SixtyUpgrades, WowSims Exporter Addon, Warcraft Logs.
                    </div>
                    <div class="form-item">
                        <textarea v-model="import_profile.string" ref="import_input" @input="checkImportString"></textarea>
                    </div>
                    <div class="form-item">
                        <label><input type="checkbox" v-model="import_profile.items" :disabled="!import_status.items"> <span>Include items</span></label>
                        <label><input type="checkbox" v-model="import_profile.config" :disabled="!import_status.config"> <span>Include config</span></label>
                    </div>
                    <div class="btn mt-2 wide" :class="[import_profile.string ? '' : 'disabled']" @click="doImport">Import</div>
                </div>
            </div>

            <div class="lightbox small" v-if="import_wcl.open">
                <div class="closer" @click="closeImportWCL"></div>
                <div class="inner">
                    <div class="title">Import raid from WCL</div>
                    <template v-if="!wcl.access_token">
                        <div class="description my-2">
                            In order to fetch data from the API, you need to authorize the application.<br><br>
                        </div>
                        <div class="btn btn-primary" @click="wcl.oauthInit()">Continue to WCL</div>
                    </template>
                    <template v-else-if="!import_wcl.fight">
                        <div class="description">
                            <div>This import will attempt to find the following settings in the raid log:</div>
                            <div class="checklist mt-n">
                                <check-item :value="import_profile.items">Player gear</check-item>
                                <check-item :value="import_profile.config">Fight duration</check-item>
                                <check-item :value="import_profile.config">Bloodlust timing</check-item>
                                <check-item :value="import_profile.config">Demonic Pact values</check-item>
                            </div>
                            <div class="mt-2">It will <b>NOT</b> import the following:</div>
                            <div class="checklist mt-n mb-2">
                                <check-item :value="false">Player talents</check-item>
                                <check-item :value="false">Rotation</check-item>
                                <check-item :value="false">Buffs/Debuffs</check-item>
                                <check-item :value="false">Personal Cooldowns</check-item>
                                <check-item :value="false">Interruptions such as silence or movement</check-item>
                            </div>
                        </div>
                        <template v-if="import_wcl.raid">
                            <div class="form-item">
                                <label>Choose player</label>
                                <select v-model="import_wcl.player_id">
                                    <option :value="null">- Choose -</option>
                                    <option :value="player.id" v-for="player in import_wcl.raid.players.filter(p => p.type == 'Mage')">{{ player.name }}</option>
                                </select>
                            </div>
                            <div class="form-item">
                                <label>Choose fight</label>
                                <select v-model="import_wcl.fight_id">
                                    <option :value="null">- Choose -</option>
                                    <option :value="fight.id" v-for="fight in import_wcl.raid.fights">{{ fight.name }}</option>
                                </select>
                            </div>
                        </template>
                        <div v-else class="mt-2">Loading log data...</div>
                        <div class="btn btn-primary mt-2" :class="{disabled: !import_wcl.fight_id || !import_wcl.player_id || import_wcl.loading}" @click="importWCLFight()">Continue</div>
                    </template>
                    <template v-else>
                        <div class="description">
                            Successfully retrieved the following data:
                            <div class="checklist mt-n">
                                <check-item :value="import_profile.items && import_wcl.fight.player.gear.length">
                                    Player gear
                                </check-item>
                                <check-item :value="import_profile.config && import_wcl.fight.duration">
                                    Fight duration 
                                    <template v-if="import_wcl.fight.duration">({{ $round(import_wcl.fight.duration/1000) }}s)</template>
                                </check-item>
                                <check-item :value="import_profile.config && import_wcl.fight.timings.length">
                                    Bloodlust timing 
                                    <template v-if="import_wcl.fight.timings.length">({{ $round(import_wcl.fight.timings[0].t/1000) }}s)</template>
                                </check-item>
                                <check-item :value="import_profile.config && import_wcl.fight.dp_avg">
                                    Demonic Pact values
                                    <template v-if="import_wcl.fight.dp_avg">({{ import_wcl.fight.dp_avg }})</template>
                                </check-item>
                            </div>
                        </div>
                        <div class="btn btn-primary mt-2" @click="importWCLConfirm()">Confirm and import</div>
                    </template>
                </div>
            </div>

            <div class="lightbox" v-if="equiplist_open">
                <div class="closer" @click="closeEquiplist"></div>
                <div class="inner">
                    <div class="title">Equipped items</div>
                    <table>
                        <thead>
                            <tr>
                                <th>Slot</th>
                                <th>Item</th>
                                <th>Enchant</th>
                            </tr>
                        </thead>
                        <tbody>
                            <tr class="equipped-item" v-for="(item_id, slot) in equipped">
                                <template v-if="item_id">
                                    <td>{{ formatKey(slot) }}</td>
                                    <td>
                                        <a :href="itemUrl(item_id)" target="_blank" :class="['quality-'+$get(getItem(slot, item_id), 'q', 'epic')]">
                                            {{ getItem(slot, item_id).title }}
                                        </a>
                                    </td>
                                    <td>
                                        <template v-if="$get(enchants, slot)">
                                            <a :href="spellUrl(enchants[slot])" target="_blank" :class="['quality-'+$get(getEnchant(slot, enchants[slot]), 'q', 'uncommon')]">
                                                {{ getEnchant(slot, enchants[slot]).title }}
                                            </a>
                                        </template>
                                    </td>
                                </template>
                            </tr>
                        </tbody>
                    </table>
                    <div class="mt-2">
                        <div class="btn wide" @click="copyEquiplist">Copy</div>
                    </div>
                </div>
            </div>

            <div class="lightbox small" v-if="custom_stats_open">
                <div class="closer" @click="closeCustomStats"></div>
                <div class="inner">
                    <div class="title">Bonus stats</div>
                    <div class="description">Add additional stats to your character.</div>
                    <div class="form-item form-row">
                        <label>Intellect</label>
                        <input type="number" v-model.number="config.custom_stats.intellect">
                    </div>
                    <div class="form-item form-row">
                        <label>Spirit</label>
                        <input type="number" v-model.number="config.custom_stats.spirit">
                    </div>
                    <div class="form-item form-row">
                        <label>Spell Power</label>
                        <input type="number" v-model.number="config.custom_stats.sp">
                    </div>
                    <div class="form-item form-row">
                        <label>Crit %</label>
                        <input type="number" v-model.number="config.custom_stats.crit">
                    </div>
                    <div class="form-item form-row">
                        <label>Hit %</label>
                        <input type="number" v-model.number="config.custom_stats.hit">
                    </div>
                    <div class="form-item form-row">
                        <label>Mp5</label>
                        <input type="number" v-model.number="config.custom_stats.mp5">
                    </div>
                </div>
            </div>

            <div class="lightbox small" v-if="custom_item_open">
                <div class="closer" @click="closeCustomItem"></div>
                <div class="inner">
                    <div class="title">Add custom item</div>
                    <div class="description">Custom items will only be added for your browser.</div>
                    <div class="form">
                        <div class="form-item form-row">
                            <label>
                                ID
                                <help>Leave empty to generate a random ID</help>
                            </label>
                            <input type="number" v-model.number="custom_item.id">
                        </div>
                        <div class="form-item form-row">
                            <label>Name</label>
                            <input type="text" v-model="custom_item.title">
                        </div>
                        <div class="form-item form-row">
                            <label>Slot</label>
                            <select v-model="custom_item.slot">
                                <option :value="null">- Choose -</option>
                                <option :value="slot" v-for="slot in itemSlots">{{ formatKey(slot) }}</option>
                            </select>
                        </div>
                        <div class="form-item form-row" v-if="custom_item.slot == 'weapon'">
                            <label>Twohanded</label>
                            <div>
                                <input type="checkbox" v-model="custom_item.twohand">
                            </div>
                        </div>
                        <div class="form-item form-row">
                            <label>Quality</label>
                            <select v-model="custom_item.q">
                                <option value="epic">Epic</option>
                                <option value="rare">Rare</option>
                                <option value="uncommon">Uncommon</option>
                                <option value="common">Common</option>
                            </select>
                        </div>
                        <div class="form-item form-row">
                            <label>Intellect</label>
                            <input type="number" v-model.number="custom_item.int">
                        </div>
                        <div class="form-item form-row">
                            <label>Spirit</label>
                            <input type="number" v-model.number="custom_item.spi">
                        </div>
                        <div class="form-item form-row">
                            <label>Spell Power</label>
                            <input type="number" v-model.number="custom_item.sp">
                        </div>
                        <div class="form-item form-row">
                            <label>Spell Power (arcane)</label>
                            <input type="number" v-model.number="custom_item.sp_arcane">
                        </div>
                        <div class="form-item form-row">
                            <label>Spell Power (fire)</label>
                            <input type="number" v-model.number="custom_item.sp_fire">
                        </div>
                        <div class="form-item form-row">
                            <label>Spell Power (frost)</label>
                            <input type="number" v-model.number="custom_item.sp_frost">
                        </div>
                        <div class="form-item form-row">
                            <label>Crit %</label>
                            <input type="number" v-model.number="custom_item.crit">
                        </div>
                        <div class="form-item form-row">
                            <label>Hit %</label>
                            <input type="number" v-model.number="custom_item.hit">
                        </div>
                        <div class="form-item form-row">
                            <label>Mp5</label>
                            <input type="number" v-model.number="custom_item.mp5">
                        </div>
                    </div>
                    <div class="mt-2 text-error" v-if="custom_item_error">
                        {{ custom_item_error }}
                    </div>
                    <div class="mt-2">
                        <div class="btn block" @click="addCustomItem">Save</div>
                    </div>
                </div>
            </div>

            <div class="lightbox small warning" v-if="quick_start_open">
                <div class="inner">
                    <div class="title">Quick start</div>
                    <div class="text">
                        Select a profile below to start your simming!<br>
                        <span class="faded">You can access all of these profiles later and even create your own under the</span> <b>Config</b> <span class="faded">tab.</span>
                    </div>
                    <div class="profile-choices">
                        <div class="profile-choice btn" v-for="profile in default_profiles" @click="loadProfile(profile); quick_start_open = false">
                            <img :src="profile.icon" v-if="profile.icon">
                            {{ profile.name }}
                        </div>
                    </div>
                    <div class="btn mt-2" @click="quick_start_open = false">
                        No thanks, I'll start from scratch
                    </div>
                </div>
            </div>

        </div>
    </div>
</template>

<script>
    import { SimulationWorker, SimulationWorkers } from "./simulation";
    import { Wcl } from "./wcl";
    import items from "./items";
    import runes from "./runes";
    import constants from "./constants";
    import default_profiles from "./default_profiles";
    
    const DEFAULT_DESIGN = 2;
    // const WCL_CLIENT_ID = process.env.MIX_WCL_CLIENT_ID;
    const WCL_CLIENT_ID = "9970a7cf-9127-424a-a18d-a0503fb2dc1d";

    export default {
        beforeCreate() {
            if (window.location.search.length) {
                var params = new URLSearchParams(window.location.search.substr(1));
                if (params.get("code")) {
                    document.documentElement.classList.add("callback");
                    var wcl = new Wcl(WCL_CLIENT_ID);
                    wcl.oauthConfirm(params.get("code"))
                    .then(r => {
                        var msg = {
                            type: "oauth_token",
                            data: r,
                        };
                        if (window.opener) {
                            window.opener.postMessage(msg, "*");
                            window.close();
                        }
                        else {
                            window.postMessage(msg, "*");
                            window.history.replaceState(null, null, wcl.redirectUri());
                            document.documentElement.classList.remove("callback");
                        }
                    })
                    .catch(e => {
                        console.error(e);
                        if (window.opener)
                            window.close();
                        else
                            window.history.replaceState(null, null, wcl.redirectUri());
                    });
                }
            }
        },

        created() {
            this.checkNewUser();
            window.addEventListener("message", this.onMessage);
        },

        mounted() {
            this.loadCustomItems();
            this.loadFavorites();
            this.loadCurrentProfile();
            this.loadDefaultProfiles();
            this.loadProfiles();
            this.calcStats();
            this.checkDonation();
            this.betaWarning();
        },

        data() {
            var splitview = localStorage.getItem("splitview") == "true" ? true : false;

            var default_config = {
                iterations: 20000,
                race: 0,

                duration: 120,
                duration_variance: 10,
                rng_seed: 0,
                stat_weight_increment: 20,
                only_main_dmg: false,
                avg_spell_dmg: false,
                additional_data: false,
                targets: 1,
                dot_targets: 0,
                target_resistance: 0,
                target_level: 28,
                target_hp: 100,
                distance: 20,
                reaction_time: 300,
                encounters: false,

                // Buffs
                mark_of_the_wild: false,
                imp_mark_of_the_wild: false,
                blessing_of_kings: false,
                blessing_of_wisdom: false,
                imp_blessing_of_wisdom: false,
                demonic_pact: false,
                demonic_pact_bonus: 0,
                rising_spirit: false,
                boon_blackfathom: false,
                ashenvale_cry: false,
                dmf_dmg: false,

                // Debuffs
                curse_of_elements: false,
                curse_of_shadow: false,

                // Consumes
                elixir_firepower: false,
                weapon_oil: 0,
                mage_scroll: 0,
                flask: 0,
                food: 0,

                pre_cast: false,

                trinket1: 0,
                trinket2: 0,

                rotation: constants.rotations.ROTATION_ST_FROST,
                maintain_imp_scorch: false,
                scorching_mages: 0,
                rot_fire_blast_weave: false,
                rot_ice_lance: false,

                timings: Array(),
                interruptions: Array(),

                build: "https://www.wowhead.com/classic/talent-calc/mage/--053500021_156jf76vca6ja",

                stats: {
                    intellect: 0,
                    spirit: 0,
                    mp5: 0,
                    crit: 0,
                    hit: 0,
                    sp: 0,
                    sp_arcane: 0,
                    sp_fire: 0,
                    sp_frost: 0,
                    mana: 0,
                },

                custom_stats: {
                    intellect: 0,
                    spirit: 0,
                    mp5: 0,
                    crit: 0,
                    hit: 0,
                    sp: 0,
                    sp_arcane: 0,
                    sp_fire: 0,
                    sp_frost: 0,
                    mana: 0,
                },

                talents: {
                    arcane_subtlety: 0,
                    arcane_focus: 0,
                    clearcast: 0,
                    imp_arcane_explosion: 0,
                    arcane_meditation: 0,
                    presence_of_mind: 0,
                    arcane_mind: 0,
                    arcane_instability: 0,
                    arcane_power: 0,

                    imp_fireball: 0,
                    ignite: 0,
                    imp_fire_blast: 0,
                    incinerate: 0,
                    imp_flamestrike: 0,
                    pyroblast: 0,
                    imp_scorch: 0,
                    master_of_elements: 0,
                    critical_mass: 0,
                    blast_wave: 0,
                    fire_power: 0,
                    combustion: 0,

                    imp_frostbolt: 0,
                    elemental_precision: 0,
                    ice_shards: 0,
                    piercing_ice: 0,
                    cold_snap: 0,
                    frost_channeling: 0,
                    shatter: 0,
                    imp_cone_of_cold: 0,
                    winters_chill: 0,
                },

                runes: {
                    burnout: false,
                    enlightenment: false,
                    fingers_of_frost: false,
                    regeneration: false,
                    arcane_blast: false,
                    ice_lance: false,
                    living_bomb: false,
                    rewind_time: false,
                    arcane_surge: false,
                    icy_veins: false,
                    living_flame: false,
                    mass_regeneration: false,
                },

                tooltips: false,
            };

            var data = {
                ...constants,
                donation_open: false,
                beta_warning_open: false,
                quick_start_open: false,
                items: items,
                equipped: {},
                enchants: {},
                item_comparison: [],
                item_off_hand: null,
                item_sort: {
                    name: null,
                    order: null,
                },
                default_profiles: [],
                profiles: [],
                history: [],
                favorites: [],
                active_slot: "weapon",
                new_profile: null,
                import_profile: {
                    open: false,
                    string: null,
                    items: true,
                    config: true,
                },
                import_wcl: {
                    open: false,
                    code: null,
                    player_id: null,
                    fight_id: null,
                    fight: null,
                    raid: null,
                },
                export_profile: {
                    open: false,
                    string: null,
                    items: true,
                    config: true,
                },
                import_status: {
                    items: true,
                    config: true,
                },
                profile_status: {
                    open: false,
                    timeout: null,
                    items: true,
                    missing_items: [],
                    config: true,
                },
                error_notice: {
                    open: false,
                    timeout: null,
                    title: null,
                    text: [],
                },
                custom_item: {
                    id: null,
                    title: null,
                    slot: null,
                    q: "rare",
                    int: null,
                    spi: null,
                    sp: null,
                    sp_arcane: null,
                    sp_fire: null,
                    sp_frost: null,
                    crit: null,
                    hit: null,
                    mp5: null,
                    twohand: false,
                },
                custom_item_open: false,
                custom_item_error: null,
                custom_stats_open: false,
                equiplist_open: false,
                equiplist_string: null,
                display_stats: null,
                result: null,
                pin_dps: null,
                ep_result: null,
                ep_weight: "dps",
                is_running: false,
                is_running_ep: false,
                active_tab: "gear",
                item_source: "wowhead",
                search_item: this.loadFilters("phase:1")+" ",
                search_log: "",
                log_filter: {
                    "0": true,
                    "1": false,
                    "2": false,
                    "3": true,
                    "4": false,
                    "5": true,
                    "6": true,
                    "7": true,
                    "8": false,
                    "9": true,
                    "10": true,
                },
                log_filter_player: false,
                splitview: splitview,
                talent_map: [[],[],[]],
                default_config: default_config,
                config: _.cloneDeep(default_config),
                wcl: new Wcl(WCL_CLIENT_ID),
            };

            var slots = [
                "weapon", "off_hand", "ranged",
                "head", "neck", "shoulder", "back", "chest", "wrist",
                "hands", "waist", "legs", "feet",
                "finger1", "finger2", "trinket1", "trinket2",
            ];

            for (var slot of slots) {
                var islot = slot;
                var i = 0;
                if (slot.indexOf("finger") === 0 || slot.indexOf("trinket") === 0) {
                    islot = slot.substr(0, slot.length-1);
                    i = parseInt(slot.substr(slot.length-1))-1;
                }
                data.equipped[slot] = null;
                data.enchants[slot] = null;
            }

            data.slots = [...slots];

            data.talent_map[0][0] = "arcane_subtlety";
            data.talent_map[0][1] = "arcane_focus";
            data.talent_map[0][5] = "clearcast";
            data.talent_map[0][7] = "imp_arcane_explosion";
            data.talent_map[0][11] = "arcane_meditation";
            data.talent_map[0][13] = "torment_of_the_weak";
            data.talent_map[0][14] = "presence_of_mind";
            data.talent_map[0][15] = "arcane_mind";
            data.talent_map[0][16] = "arcane_instability";
            data.talent_map[0][17] = "arcane_power";

            data.talent_map[1][0] = "imp_fireball";
            data.talent_map[1][2] = "ignite";
            data.talent_map[1][4] = "imp_fire_blast";
            data.talent_map[1][5] = "incinerate";
            data.talent_map[1][6] = "imp_flamestrike";
            data.talent_map[1][7] = "pyroblast";
            data.talent_map[1][9] = "imp_scorch";
            data.talent_map[1][11] = "master_of_elements";
            data.talent_map[1][12] = "critical_mass";
            data.talent_map[1][13] = "blast_weave";
            data.talent_map[1][14] = "fire_power";
            data.talent_map[1][15] = "combustion";

            data.talent_map[2][1] = "imp_frostbolt";
            data.talent_map[2][2] = "elemental_precision";
            data.talent_map[2][3] = "ice_shards";
            data.talent_map[2][7] = "piercing_ice";
            data.talent_map[2][8] = "cold_snap";
            data.talent_map[2][11] = "frost_channeling";
            data.talent_map[2][12] = "shatter";
            data.talent_map[2][14] = "imp_cone_of_cold";
            data.talent_map[2][15] = "winters_chill";

            return data;
        },

        computed: {
            faction() {
                var alliance = [
                    this.races.RACE_GNOME,
                    this.races.RACE_HUMAN,
                ];

                if (alliance.indexOf(this.config.race) != -1)
                    return "alliance";
                return "horde";
            },

            spellStats() {
                if (!this.result.spells)
                    return [];
                return this.result.spells.sort(function(a, b) {
                    if (a.source == "Player" && b.source != "Player")
                        return -1;
                    if (b.source == "Player" && a.source != "Player")
                        return 1;
                    return a.name.localeCompare(b.name);
                });
            },

            activeItems() {
                var self = this;
                var slot = this.equipSlotToItemSlot(this.active_slot);

                var items = this.items.equip[slot];

                if (!items)
                    return [];

                items = items.filter(function(item) {
                    if (!item.hasOwnProperty("faction"))
                        return true;
                    return self.faction.substr(0, 1) == item.faction;
                });

                if (this.search_item) {
                    var terms = this.search_item.toLowerCase().split(" ");
                    var str = [];
                    var last_str = false;
                    var arr, value, op;

                    var fnBool = function(str) {
                        if (str[0] == "y")
                            return true;
                        return false;
                    };

                    var filterBool = function(str, val) {
                        if (str[0] == "y")
                            return val;
                        return !val;
                    };

                    var filterInt = function(str, val) {
                        var m;
                        if (m = str.match(/^([0-9]+)\-([0-9]+)$/)) {
                            return val >= m[1] && val <= m[2];
                        }
                        else if (m = str.match(/^([0-9]+)$/)) {
                            return val == m[1];
                        }
                        else if (m = str.match(/^([0-9]+)\+$/)) {
                            return val >= m[1];
                        }
                        else if (m = str.match(/^([0-9]+)\-$/)) {
                            return val <= m[1];
                        }

                        return true;
                    };

                    for (var i=0; i<terms.length; i++) {
                        arr = terms[i].split(":");
                        if (arr.length == 2) {
                            if (!arr[1].length)
                                continue;
                            if (arr[0] == "pvp") {
                                value = fnBool(arr[1]);
                                if (value)
                                    items = items.filter(item => _.get(item, "pvp", false) === true);
                                else if (value !== null)
                                    items = items.filter(item => _.get(item, "pvp", false) === false);
                            }
                            else if (arr[0] == "ilvl") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "ilvl", 0)));
                            }
                            else if (arr[0] == "p" || arr[0] == "phase") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "phase", 1)));
                            }
                            else if (arr[0] == "sp") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "sp", 0)));
                            }
                            else if (arr[0] == "crit") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "crit", 0)));
                            }
                            else if (arr[0] == "hit") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "hit", 0)));
                            }
                            else if (arr[0] == "spi" || arr[0] == "spirit") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "spi", 0)));
                            }
                            else if (arr[0] == "int" || arr[0] == "intellect") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "int", 0)));
                            }
                            else if (arr[0] == "mp5") {
                                items = items.filter(item => filterInt(arr[1], _.get(item, "mp5", 0)));
                            }
                            else if (arr[0] == "2h") {
                                items = items.filter(item => filterBool(arr[1], _.get(item, "twohand", false)));
                            }
                            else if (arr[0] == "set" || arr[0] == "itemset") {
                                items = items.filter(item => filterBool(arr[1], _.get(item, "itemset", false)));
                            }

                            last_str = false;
                        }
                        else {
                            if (last_str)
                                str[str.length-1]+= " "+terms[i];
                            else
                                str.push(terms[i]);
                            last_str = true;
                        }
                    }

                    if (str.length) {
                        for (var i=0; i<str.length; i++)
                            items = items.filter(item => item.title.toLowerCase().indexOf(str[i]) != -1);
                    }
                }

                return this.sort(items, this.item_sort);
            },

            activeEnchants() {
                var slot = this.equipSlotToEnchantSlot(this.active_slot);

                if (!this.items.enchants.hasOwnProperty(slot))
                    return [];

                return this.items.enchants[slot];
            },

            activeLog() {
                if (!this.result || !this.result.log)
                    return [];

                var log = this.result.log;

                if (this.log_filter_player)
                    log = log.filter(l => l.unit == "Player");

                if (this.search_log)
                    log = log.filter(l => l.text.toLowerCase().indexOf(this.search_log.toLowerCase()) != -1);

                log = log.filter(l => this.log_filter[l.type]);

                return log;
            },

            hasComparisons() {
                return this.item_comparison.length > 1;
            },

            itemSlots() {
                return _.keys(this.items.equip);
            },

            numCasts() {
                if (!this.result || !this.result.spells)
                    return 0;
                return _.sumBy(this.result.spells, "casts");
            },

            spellDmg() {
                if (!this.result || !this.result.spells)
                    return 1;
                return _.sumBy(this.result.spells, "dmg");
            },

            epCalc() {
                if (!this.ep_result)
                    return null;

                var ep = {
                    intellect: null,
                    spirit: null,
                    mp5: null,
                    sp: null,
                    crit: null,
                    hit: null,
                };

                if (!this.ep_result.base)
                    return ep;

                if (this.ep_weight == "dps") {
                    for (var stat in ep) {
                        if (this.ep_result[stat])
                            ep[stat] = (this.ep_result[stat] - this.ep_result.base) / 10;
                    }
                }
                else {
                   if (_.get(this.ep_result, this.ep_weight, 0) - this.ep_result.base < 0.1)
                        return ep;

                    for (var stat in ep) {
                        if (this.ep_result[stat]) {
                            if (stat == this.ep_weight)
                                ep[stat] = 1;
                            else
                                ep[stat] = (this.ep_result[stat] - this.ep_result.base) / (this.ep_result[this.ep_weight] - this.ep_result.base);
                        }
                    }
                }

                return ep;
            },

            timings() {
                var timings = [];

                timings.push({
                    name: "power_infusion",
                    title: "Power Infusion",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_holy_powerinfusion.jpg",
                });
                timings.push({
                    name: "mana_tide",
                    title: "Mana Tide",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_frost_summonwaterelemental.jpg",
                });
                timings.push({
                    name: "innervate",
                    title: "Innervate",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_nature_lightning.jpg",
                });
                timings.push({
                    name: "mana_gem",
                    title: "Mana Gem",
                    icon: "https://www.wowhead.com/images/wow/icons/large/inv_misc_gem_sapphire_02.jpg",
                });
                timings.push({
                    name: "evocation",
                    title: "Evocation",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_nature_purge.jpg",
                });
                timings.push({
                    name: "berserking",
                    title: "Berserking",
                    icon: "https://www.wowhead.com/images/wow/icons/large/racial_troll_berserk.jpg",
                });
                timings.push({
                    name: "presence_of_mind",
                    title: "Presence of Mind",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_nature_enchantarmor.jpg",
                });
                timings.push({
                    name: "arcane_power",
                    title: "Arcane Power",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_nature_lightning.jpg",
                });
                timings.push({
                    name: "combustion",
                    title: "Combustion",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_fire_sealoffire.jpg",
                });
                timings.push({
                    name: "icy_veins",
                    title: "Icy Veins",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_frost_coldhearted.jpg",
                });
                timings.push({
                    name: "cold_snap",
                    title: "Cold Snap",
                    icon: "https://www.wowhead.com/images/wow/icons/large/spell_frost_wizardmark.jpg",
                });

                var trinkets = [
                    {
                        id: this.items.ids.TRINKET_MQG,
                        title: "Mind Quickening Gem",
                        icon: "https://www.wowhead.com/images/wow/icons/large/spell_nature_wispheal.jpg",
                    },
                    {
                        id: this.items.ids.TRINKET_RECOMBO,
                        title: "Minor Recombobulator",
                        icon: "https://wow.zamimg.com/images/wow/icons/large/inv_gizmo_07.jpg",
                    },
                ];

                var slots = ["trinket1", "trinket2"];
                var timing;
                for (var i=0; i<trinkets.length; i++) {
                    for (var j=0; j<slots.length; j++) {
                        if (this.equipped[slots[j]] == trinkets[i].id) {
                            timing = _.clone(trinkets[i]);
                            delete timing.id;
                            timing.name = slots[j];
                            timings.push(timing);
                            slots.splice(j, 1);
                            j--;
                        }
                    }
                }

                return timings;
            },

            waitBuffs() {
                var buffs = [];

                // buffs = _.sortBy(buffs, "name");

                return buffs;
            },

            aoeRotation() {
                var aoe = [
                    constants.rotations.ROTATION_AOE_AE,
                    constants.rotations.ROTATION_AOE_BLIZZ,
                    constants.rotations.ROTATION_AOE_BLIZZ_FS,
                    constants.rotations.ROTATION_AOE_FIRE,
                    constants.rotations.ROTATION_AOE_FS,
                ];

                return aoe.indexOf(this.config.rotation) != -1;
            }
        },

        methods: {
            onMessage(e) {
                if (_.get(e, "data.type") == "oauth_token") {
                    this.wcl.setAccessToken(e.data.data);
                    this.errorNotice("Success!", "WCL authentication successful");

                    if (this.import_wcl.open)
                        this.importWCLReport(this.import_wcl.code);
                }
            },

            setSplitview(value) {
                localStorage.setItem("splitview", value);
                this.splitview = value;
                if (this.splitview)
                    this.refreshTooltips();
            },

            toggleSplitview(e) {
                this.setSplitview(!this.splitview);
            },

            splitviewShort(long, short) {
                return this.splitview ? short : long;
            },

            newTimingId() {
                var id = 0;
                for (var i=0; i<this.config.timings.length; i++)
                    id = Math.max(id, this.config.timings[i].id);
                return id+1;
            },

            addTiming(name) {
                var timing = {
                    id: this.newTimingId(),
                    name: name,
                    t: 0,
                    wait_t: 0,
                    wait_for_buff: 0,
                };
                this.config.timings.push(timing);
                this.config.timings = _.sortBy(this.config.timings, "name");

                return timing;
            },

            removeTiming(id) {
                var index = _.findIndex(this.config.timings, {id: id});
                if (index != -1)
                    this.config.timings.splice(index, 1);
            },

            getTiming(name, key, def) {
                if (typeof(def) == "undefined")
                    def = null;
                var timing = _.find(this.timings, {name: name});
                if (!timing)
                    return def;
                return _.get(timing, key, def);
            },

            timingEnabled(name) {
                var always = [
                    // "mana_tide", "power_infusion", "innervate",
                    // "mana_gem", 
                    "evocation",
                ];
                if (always.indexOf(name) != -1)
                    return true;

                if (name == "berserking")
                    return this.config.race == constants.races.RACE_TROLL;
                if (name == "presence_of_mind")
                    return this.config.talents.presence_of_mind > 0;
                if (name == "arcane_power")
                    return this.config.talents.arcane_power > 0;
                if (name == "combustion")
                    return this.config.talents.combustion > 0;
                if (name == "icy_veins")
                    return this.config.runes.icy_veins > 0;
                if (name == "cold_snap")
                    return this.config.talents.cold_snap > 0;
                if (name == "trinket1")
                    return this.equipped.trinket1 && _.get(this.equippedItem("trinket1"), "use");
                if (name == "trinket2")
                    return this.equipped.trinket2 && _.get(this.equippedItem("trinket2"), "use");

                return false;
            },

            timingCanWait(name) {
                var nowait = ["mana_tide", "power_infusion", "innervate"];
                if (nowait.indexOf(name) != -1)
                    return false;

                return true;
            },

            newInterruptionId() {
                var id = 0;
                for (var i=0; i<this.config.interruptions.length; i++)
                    id = Math.max(id, this.config.interruptions[i].id);
                return id+1;
            },

            addInterruption() {
                this.config.interruptions.push({
                    id: this.newInterruptionId(),
                    silence: false,
                    affects_all: false,
                    t: 0,
                    duration: 0,
                });

                this.config.interruptions = _.sortBy(this.config.interruptions, "t");
            },

            removeInterruption(id) {
                var index = _.findIndex(this.config.interruptions, {id: id});
                if (index != -1)
                    this.config.interruptions.splice(index, 1);
            },

            sort(items, sorting) {
                var self = this;

                if (!sorting || !sorting.name) {
                    return items.sort(function(a, b) {
                        var fa = self.isFavorite(a);
                        var fb = self.isFavorite(b);
                        if (fa && !fb)
                            return -1;
                        if (!fa && fb)
                            return 1;
                        return 0;
                    });
                }

                var type = null;
                for (var i=0; i<items.length; i++) {
                    var value = _.get(items[i], sorting.name, null);
                    if (value !== null) {
                        type = typeof(value);
                        if (type == "object") {
                            if (_.isArray(value))
                                type = "array";
                            else
                                continue;
                        }
                        break;
                    }
                }

                if (sorting.name == "dps")
                    type = "number";

                if (type === null)
                    return items;

                return items.sort(function(a, b) {
                    var av = _.get(a, sorting.name, null);
                    var bv = _.get(b, sorting.name, null);

                    if (sorting.name == "phase") {
                        if (!av) av = 1;
                        if (!bv) bv = 1;
                    }

                    if (sorting.name == "dps") {
                        av = _.get(_.find(self.item_comparison, {id: a.id}), "dps", 0);
                        bv = _.get(_.find(self.item_comparison, {id: b.id}), "dps", 0);
                    }

                    var result = 0;
                    if (type == "string") {
                        try { av = av.toString(); } catch(e) { av = ""; };
                        try { bv = bv.toString(); } catch(e) { bv = ""; };
                        result = av.localeCompare(bv);
                    }
                    else if (type == "number") {
                        av = parseFloat(av);
                        bv = parseFloat(bv);
                        if (isNaN(av)) av = 0;
                        if (isNaN(bv)) bv = 0;
                        result = av - bv;
                    }
                    else if (type == "array") {
                        av = _.get(av, "length", 0);
                        bv = _.get(bv, "length", 0);
                        result = av - bv;
                    }

                    if (sorting.order == "desc" && result != 0)
                        result = result < 0 ? 1 : -1;

                    return result;
                });
            },

            checkDonation() {
                if (window.location.hash == "#donation") {
                    window.location.hash = "";
                    this.donation_open = true;
                }
            },

            betaWarning() {
                if (!localStorage.getItem("beta_warning")) {
                    this.beta_warning_open = true;
                }
            },

            closeBetaWarning(accept) {
                if (accept)
                    localStorage.setItem("beta_warning", 1);
                this.beta_warning_open = false;
            },

            checkNewUser() {
                if (!default_profiles.length)
                    return;

                var storages = ["magesim_sod_profile", "magesim_sod_profiles", "magesim_sod_custom_items"];
                for (var key of storages) {
                    if (localStorage.getItem(key) !== null)
                        return;
                }

                this.quick_start_open = true;
            },

            runMultiple() {
                var self = this;

                if (["log", "timeline", "spells"].indexOf(this.active_tab) != -1)
                    this.setTab("gear");

                this.ep_result = null;
                this.prepare();
                var save_history = this.saveHistory();
                this.is_running = true;

                var sim = new SimulationWorkers(this.config.iterations, (result) => {
                    self.is_running = false;
                    self.result = result;
                    if (save_history) {
                        self.history[0].result = result;
                        self.history[0].end = new Date;
                    }
                }, (error) => {
                    self.is_running = false;
                    console.error(error);
                });

                sim.start(this.config);
            },

            runSingle() {
                var self = this;
                var sim = new SimulationWorker((result) => {
                    self.is_running = false;
                    self.result = result;
                    if (self.result.spells)
                        self.result.spells = _.sortBy(self.result.spells, "casts").reverse();
                }, (error) => {
                    self.is_running = false;
                    console.error(error);
                });

                if (this.active_tab == "histogram")
                    this.setTab("gear");

                this.ep_result = null;
                this.prepare();
                this.is_running = true;
                sim.start(this.config);
            },

            async findAvg(avg) {
                if (this.active_tab == "histogram")
                    this.setTab("gear");

                this.ep_result = null;
                this.prepare();
                this.is_running = true;

                var result;
                while (true) {
                    result = await this.runAvg(avg);
                    if (Math.abs(result.dps - avg) <= avg*0.005) {
                        this.is_running = false;
                        this.result = result;
                        break;
                    }
                }
            },

            async runAvg(avg) {
                var self = this;

                return new Promise(function(resolve, reject) {
                    var sim = new SimulationWorker((result) => {
                        resolve(result);
                    }, (error) => {
                        self.is_running = false;
                        console.error(error);
                    });

                    self.is_running = true;
                    sim.start(self.config);
                });
            },

            async runStat(stat, value, rng_seed) {
                var self = this;

                return new Promise((resolve, reject) => {
                    var sim = new SimulationWorkers(self.config.iterations, (result) => {
                        self.is_running = false;
                        resolve(result);
                    }, (error) => {
                        self.is_running = false;
                        console.error(error);
                        reject(error);
                    });

                    if (self.config.custom_stats[stat] == "")
                        self.config.custom_stats[stat] = 0;
                    self.config.custom_stats[stat]+= value;
                    self.prepare();
                    var config = _.cloneDeep(self.config);
                    self.config.custom_stats[stat]-= value;
                    if (rng_seed)
                        config.rng_seed = rng_seed;
                    self.is_running = true;
                    sim.start(config);
                });
            },

            async runEP() {
                if (this.is_running_ep)
                    this.is_running_ep = false;

                if (this.is_running)
                    return;

                if (["log", "timeline", "spells"].indexOf(this.active_tab) != -1)
                    this.setTab("gear");

                this.is_running_ep = true;
                this.result = null;
                this.ep_result = {
                    base: null,
                    intellect: null,
                    spirit: null,
                    mp5: null,
                    sp: null,
                    crit: null,
                    hit: null,
                };

                var rng_seed = Math.round(Math.random() * 100000);
                var result;
                var increment = this.config.stat_weight_increment;
                if (increment < 1)
                    increment = 20;
                for (var stat in this.ep_result) {
                    result = await this.runStat(stat, stat == "base" ? 0 : increment, rng_seed);
                    this.ep_result[stat] = result.avg_dps;
                    if (!this.is_running_ep)
                        break;
                }

                this.is_running_ep = false;
            },

            async runComparisonFor(item_id) {
                var self = this;
                this.equip(this.active_slot, item_id, false);

                return new Promise((resolve, reject) => {
                    var sim = new SimulationWorkers(self.config.iterations, (result) => {
                        self.is_running = false;
                        resolve(result);
                    }, (error) => {
                        self.is_running = false;
                        console.error(error);
                        reject(error);
                    });

                    self.prepare();
                    self.is_running = true;
                    sim.start(self.config);
                });
            },

            async runComparison() {
                if (!this.hasComparisons || this.is_running)
                    return;

                for (var i in this.item_comparison)
                    this.item_comparison[i].dps = null;

                var old_item_id = this.equipped[this.active_slot];
                var result, cmp;
                for (var i in this.item_comparison) {
                    cmp = this.item_comparison[i];
                    result = await this.runComparisonFor(cmp.id);
                    this.item_comparison[i].dps = result.avg_dps;
                }

                this.equip(this.active_slot, old_item_id);
            },

            prepare() {
                this.fillEmptyFields();
                this.sortTimings();
                this.saveCurrentProfile();
                this.itemStats();
                this.itemConfig();
                this.calcStats();
            },

            clearSearchText() {
                var str = [];
                var terms = this.search_item.toLowerCase().split(" ");

                for (var i=0; i<terms.length; i++) {
                    if (terms[i].split(":").length == 2)
                        str.push(terms[i]);
                }

                this.search_item = str.join(" ")+" ";
            },

            setActiveSlot(slot) {
                if (this.is_running)
                    return;

                this.active_slot = slot;
                this.item_comparison = [];

                this.$nextTick(function() {
                    this.clearSearchText();
                    this.$refs.search.focus();
                    this.refreshTooltips();
                });
            },

            equipSlotToItemSlot(slot) {
                if (slot.indexOf("finger") === 0)
                    slot = "finger";
                if (slot.indexOf("trinket") === 0)
                    slot = "trinket";

                return slot;
            },

            equipSlotToEnchantSlot(slot) {
                return this.equipSlotToItemSlot(slot);
            },

            otherSlot(slot) {
                var n = slot.substr(-1);
                n = parseInt(n);
                if (isNaN(n))
                    return slot;
                n = n == 1 ? 2 : 1;
                return slot.substr(0, slot.length-1)+n;
            },

            dontStack(input, config) {
                if (!Array.isArray(config))
                    config = [config];

                if (input.target.checked) {
                    for (var i=0; i<config.length; i++) {
                        var key = config[i];
                        if (_.get(this.config, key)) {
                            _.set(this.config, key, false);
                        }
                    }
                }
            },

            findItem(id) {
                var item;
                for (var i=0; i<this.itemSlots.length; i++) {
                    item =_.find(this.items.equip[this.itemSlots[i]], {id: id});
                    if (item)
                        return item;
                }

                return null;
            },

            getSlotAndItem(id) {
                var item;
                for (var key in this.items.equip) {
                    item = _.find(this.items.equip[key], {id: id});
                    if (item)
                        return [key, item];
                }

                return null;
            },

            getItem(slot, id) {
                var eslot = this.equipSlotToItemSlot(slot);
                return _.find(this.items.equip[eslot], {id: id}, null);
            },

            searchItem(slot, title) {
                var eslot = this.equipSlotToItemSlot(slot);
                return _.find(this.items.equip[eslot], {title: title}, null);
            },

            getEnchant(slot, id) {
                var eslot = this.equipSlotToItemSlot(slot);
                return _.find(this.items.enchants[eslot], {id: id}, null);
            },

            getEnchantFromEnchantmentId(slot, id) {
                var map = {
                    3872: 3719,
                    3873: 3721,
                };

                if (map.hasOwnProperty(id))
                    id = map[id];

                var eslot = this.equipSlotToItemSlot(slot);
                return _.find(this.items.enchants[eslot], {enchantmentId: id}, null);
            },

            searchEnchant(slot, title) {
                var eslot = this.equipSlotToItemSlot(slot);
                return _.find(this.items.enchants[eslot], {title: title}, null);
            },

            equippedItem(slot) {
                var id = this.equipped[slot];
                if (!id)
                    return null;

                return this.getItem(slot, id);
            },

            equippedEnchant(slot) {
                var id = this.enchants[slot];
                if (!id)
                    return null;

                return this.getEnchant(slot, id);
            },

            dollSlots(pos) {
                if (pos == "left") {
                    return [
                        "head", "neck", "shoulder",
                        "back", "chest", "wrist",
                        "weapon", "off_hand", "ranged",
                    ];
                }
                if (pos == "right") {
                    return [
                        "hands", "waist", "legs", "feet",
                        "finger1", "finger2", "trinket1", "trinket2",
                    ];
                }
            },

            fillEmptyFields() {
                for (var key in this.config) {
                    if (this.config[key] === "" || this.config[key] === null)
                        this.config[key] = this.default_config.hasOwnProperty(key) ? this.default_config[key] : 0;
                }
            },

            sortTimings() {
                var fn = function(a, b) {
                    if (a.name == b.name)
                        return a.t - b.t;
                    return a.name.localeCompare(b.name);
                };

                this.config.timings = this.config.timings.filter(t => this.timingEnabled(t.name) || t.name.indexOf("trinket") == 0);

                this.config.timings.sort(fn);
            },

            baseStats() {
                // Undead default
                var stats = {
                    intellect: 53,
                    spirit: 58,
                    mp5: 0,
                    crit: 0.91,
                    hit: 0,
                    sp: 0,
                    sp_arcane: 0,
                    sp_fire: 0,
                    sp_frost: 0,
                    mana: 0,
                };

                if (this.config.race == this.races.RACE_TROLL) {
                    stats.intellect = 51;
                    stats.spirit = 54;
                }
                if (this.config.race == this.races.RACE_GNOME) {
                    stats.intellect = 61;
                    stats.spirit = 53;
                }
                if (this.config.race == this.races.RACE_HUMAN) {
                    stats.intellect = 55;
                    stats.spirit = 57;
                }

                return stats;
            },

            itemStats() {
                var stats = this.baseStats();

                var item_stats = {
                    int: 0,
                    spi: 0,
                    mp5: 0,
                    crit: 0,
                    hit: 0,
                    sp: 0,
                    sp_arcane: 0,
                    sp_fire: 0,
                    sp_frost: 0,
                    mana: 0,
                };

                var addStats = function(itm) {
                    for (var stat in itm) {
                        if (item_stats.hasOwnProperty(stat))
                            item_stats[stat]+= itm[stat];
                    }
                };

                var slot, item, has_bonus, get_bonus, enchant, set, setbonus;
                var sets = {};

                for (var key in this.equipped) {
                    slot = this.equipSlotToItemSlot(key);
                    item = _.find(this.items.equip[slot], {id: this.equipped[key]});
                    if (item) {
                        has_bonus = item.hasOwnProperty("bonus");
                        get_bonus = true;

                        addStats(item);

                        if (this.items.enchants[slot] && this.enchants[key]) {
                            enchant = _.find(this.items.enchants[slot], {id: this.enchants[key]});
                            if (enchant)
                                addStats(enchant);
                        }

                        if (item.itemset) {
                            if (!sets.hasOwnProperty(item.itemset)) {
                                sets[item.itemset] = 1;
                            }
                            else {
                                sets[item.itemset]++;
                                set = _.find(this.items.itemsets, {id: item.itemset})
                                if (set) {
                                    setbonus = _.get(set, "set"+sets[item.itemset]);
                                    if (setbonus)
                                        addStats(setbonus);
                                }
                            }
                        }
                    }
                }

                stats.intellect+= item_stats.int;
                stats.spirit+= item_stats.spi;
                stats.mp5+= item_stats.mp5;
                stats.crit = item_stats.crit;
                stats.hit = item_stats.hit;
                stats.sp+= item_stats.sp;
                stats.sp_arcane+= item_stats.sp_arcane;
                stats.sp_fire+= item_stats.sp_fire;
                stats.sp_frost+= item_stats.sp_frost;
                stats.mana+= item_stats.mana;

                this.config.stats = stats;
            },

            itemConfig() {
                this.config.trinket1 = 0;
                this.config.trinket2 = 0;
                if (this.isSpecialItem(this.equipped.trinket1))
                    this.config.trinket1 = this.equipped.trinket1;
                if (this.isSpecialItem(this.equipped.trinket2))
                    this.config.trinket2 = this.equipped.trinket2;

            },

            simStats() {
                var x;
                this.itemStats();
                this.itemConfig();

                var stats = this.config.stats;

                // Custom stats
                for (var key in this.config.custom_stats) {
                    if (this.config.custom_stats[key])
                        stats[key]+= this.config.custom_stats[key];
                }

                // Arcane intellect
                stats.intellect+= 7;

                if (this.config.mark_of_the_wild) {
                    x = 4;
                    if (this.config.imp_mark_of_the_wild)
                        x*= 1.35;
                    stats.intellect+= x;
                    stats.spirit+= x;
                }

                // Runes
                if (this.config.runes.burnout)
                    stats.crit+= 15;

                // Consumes
                if (this.config.elixir_firepower)
                    stats.sp_fire+= 10;
                if (this.config.food == this.foods.FOOD_SPIRIT6)
                    stats.spirit+= 6;
                if (this.config.food == this.foods.FOOD_SPIRIT8)
                    stats.spirit+= 8;

                // Flasks
                if (this.config.flask == this.flasks.FLASK_SUPREME_POWER)
                    stats.sp+= 150;
                if (this.config.flask == this.flasks.FLASK_DISTILLED_WISDOM)
                    stats.mana+= 2000;

                // Weapon oils
                if (this.config.weapon_oil == this.weapon_oils.OIL_BLACKFATHOM) {
                    stats.mp5+= 12;
                    stats.hit+= 2;
                }
                if (this.config.weapon_oil == this.weapon_oils.OIL_BRILLIANT_WIZARD) {
                    stats.sp+= 36;
                    stats.crit+= 1;
                }
                if (this.config.weapon_oil == this.weapon_oils.OIL_WIZARD)
                    stats.sp+= 24;
                if (this.config.weapon_oil == this.weapon_oils.OIL_LESSER_WIZARD)
                    stats.sp+= 16;
                if (this.config.weapon_oil == this.weapon_oils.OIL_MINOR_WIZARD)
                    stats.sp+= 8;
                if (this.config.weapon_oil == this.weapon_oils.OIL_BRILLIANT_MANA)
                    stats.mp5+= 12;
                if (this.config.weapon_oil == this.weapon_oils.OIL_LESSER_MANA)
                    stats.mp5+= 8;
                if (this.config.weapon_oil == this.weapon_oils.OIL_MINOR_MANA)
                    stats.mp5+= 4;

                // Scroll
                if (this.config.mage_scroll == this.mage_scrolls.SCROLL_ACCURACY1)
                    stats.hit+= 1;
                if (this.config.mage_scroll == this.mage_scrolls.SCROLL_POWER1)
                    stats.crit+= 1;
                if (this.config.mage_scroll == this.mage_scrolls.SCROLL_RECOVERY1)
                    stats.mp5+= 8;

                // Mana Restoration
                if (this.config.blessing_of_wisdom && this.faction == "alliance") {
                    x = 15;
                    if (this.config.imp_blessing_of_wisdom)
                        x*= 1.2;
                    stats.mp5+= x;
                }

                // World buffs
                if (this.config.rising_spirit && this.faction == "horde")
                    stats.spirit+= 25;
                if (this.config.boon_blackfathom)
                    stats.crit+= 2;

                // Attribute multipliers
                if (this.config.race == this.races.RACE_GNOME)
                    stats.intellect*= 1.05;
                if (this.config.race == this.races.RACE_HUMAN)
                    stats.spirit*= 1.05;
                if (this.config.blessing_of_kings) {
                    stats.intellect*= 1.1;
                    stats.spirit*= 1.1;
                }

                stats.intellect = Math.round(stats.intellect);
                stats.spirit = Math.round(stats.spirit);

                stats.crit+= this.config.talents.arcane_instability;

                // Calculate percentages
                stats.crit+= stats.intellect/19.6; // UNCONFIRMED

                this.config.stats = stats;
            },

            displayStats() {
                var x;
                var stats = _.cloneDeep(this.config.stats);

                // Demonic pact
                if (this.config.demonic_pact && this.config.demonic_pact_bonus > 0)
                    stats.sp+= this.config.demonic_pact_bonus;

                // Mana
                stats.mana+= 493 + stats.intellect*15 - 280;

                this.display_stats = stats;
            },

            calcStats() {
                this.simStats();
                this.displayStats();
            },

            openItem(item) {
                var a = document.createElement("a");
                a.href = this.itemUrl(item);
                a.target = "_blank";
                a.click();
            },

            itemUrl(item) {
                if (typeof(item) != "object")
                    item = this.findItem(item);

                if (item.id > 999999)
                    return null;

                var id = item.id;
                if (typeof(id) == "string")
                    id = id.replace(":", "&rand=");

                var url = "https://www.wowhead.com/classic/item="+id;

                return url;
            },

            spellUrl(id) {
                if (typeof(id) == "object")
                    id = id.id;
                return "https://www.wowhead.com/classic/spell="+id;
            },

            equippedUrl(slot) {
                if (!this.equipped[slot])
                    return null;
                var url = this.itemUrl(this.equipped[slot]);
                var item = this.getItem(slot, this.equipped[slot]);

                if (_.get(this.enchants, slot)) {
                    var enchant = this.getEnchant(slot, this.enchants[slot]);
                    if (enchant)
                        url+= "&ench="+enchant.enchantmentId;
                }

                if (item.itemset) {
                    var pcs = [];
                    for (var key in this.equipped) {
                        var itm = this.getItem(key, this.equipped[key]);
                        if (_.get(itm, "itemset") == item.itemset)
                            pcs.push(itm.id);
                    }
                    if (pcs.length)
                        url+= "&pcs="+pcs.join(":");
                }

                return url;
            },

            isSpecialItem(item_id) {
                for (var key in this.items.ids) {
                    if (this.items.ids[key] == item_id)
                        return true;
                }
                return false;
            },

            onUnequip(slot) {
                if (slot == "weapon" && this.equipped.off_hand) {
                    this.item_off_hand = this.equipped.off_hand;
                }
            },

            equipToggle(slot, item) {
                if (this.equipped[slot] == item.id)
                    this.unequip(slot);
                else
                    this.equip(slot, item);
            },

            unequip(slot, save) {
                this.onUnequip(slot);
                this.equipped[slot] = null;

                this.calcStats();
                if (typeof(save) == "undefined" || save)
                    this.saveCurrentProfile();
            },

            equip(slot, item, save) {
                if (!_.isObject(item))
                    item = this.getItem(slot, item);

                if (this.equipped[slot] == item.id)
                    return;

                if (slot == "off_hand") {
                    var weapon = this.equippedItem("weapon");
                    if (weapon.twohand)
                        return;
                    this.item_off_hand = item.id;
                }

                if (item.unique) {
                    var other = this.otherSlot(slot);
                    if (other != slot && this.equipped[other]) {
                        if (this.isEquipped(other, item.id))
                            return;
                        // A unique category
                        if (item.unique !== true) {
                            var other_item = this.getItem(other, this.equipped[other]);
                            if (_.get(other_item, "unique", null) === item.unique)
                                return;
                        }
                    }
                }

                this.onUnequip(slot);

                if (slot == "weapon") {
                    if (item.twohand)
                        this.equipped.off_hand = null;
                    else if (!this.equipped.off_hand && this.item_off_hand)
                        this.equipped.off_hand = this.item_off_hand;

                    var enchant = this.equippedEnchant(slot);
                    if (!item.twohand && enchant && enchant.twohand)
                        this.enchants[slot] = null;
                }

                this.equipped[slot] = item.id;

                this.calcStats();
                if (typeof(save) == "undefined" || save)
                    this.saveCurrentProfile();

                this.refreshTooltips();
            },

            isEquipped(slot, id) {
                if (slot == "trinket" || slot == "finger")
                    return this.isEquipped(slot+"1", id) || this.isEquipped(slot+"2", id);

                return _.get(this.equipped, slot) == id;
            },

            numEquippedSet(id) {
                var num = 0;

                for (var key in this.equipped) {
                    var slot = this.equipSlotToItemSlot(key);
                    var item = _.find(this.items.equip[slot], {id: this.equipped[key]});
                    if (item && _.get(item, "itemset") == id)
                        num++;
                }

                return num;
            },

            enchant(slot, enchant) {
                if (enchant.twohand) {
                    var item = this.equippedItem(slot);
                    if (!item || !item.twohand)
                        return;
                }

                if (this.enchants[slot] == enchant.id)
                    this.enchants[slot] = null;
                else
                    this.enchants[slot] = enchant.id;

                this.saveCurrentProfile();
                this.calcStats();

                this.refreshTooltips();
            },

            isEnchanted(slot, id) {
                if (slot == "trinket" || slot == "finger")
                    return this.isEnchanted(slot+"1") || this.isEnchanted(slot+"2");

                return _.get(this.enchants, slot) == id;
            },

            numProfs() {
                var num = 0;

                return num;
            },

            hasUseTrinket(nr) {
                var slot = "trinket"+nr;
                var item = this.equippedItem(slot);

                if (item && item.use)
                    return true;

                return false;
            },

            paperClick(slot, what) {
                this.setActiveSlot(slot);

                if (what == "enchant") {
                    this.$nextTick(function() {
                        this.scrollToEnchant();
                    });
                }
                else {
                    this.$nextTick(function() {
                        this.scrollToItems();
                    });
                }
            },

            scrollToItems() {
                if (this.$refs.enchantAnchor && this.$refs.items) {
                    this.$refs.items.scrollTo(0, 0);
                }
            },

            scrollToEnchant() {
                if (this.$refs.enchantAnchor && this.$refs.items) {
                    this.$refs.items.scrollTo({
                        top: this.$refs.enchantAnchor.offsetTop - this.$refs.items.offsetTop - 30,
                        behavior: "smooth"
                    });
                }
            },

            updatePin() {
                if (this.result && this.result.avg_dps)
                    this.pin_dps = this.result.avg_dps;
            },

            removePin() {
                this.pin_dps = null;
            },

            isComparing(item) {
                return _.findIndex(this.item_comparison, {id: item.id}) != -1;
            },

            compareAll() {
                if (this.item_comparison.length == this.activeItems.length && _.find(this.item_comparison, {id: this.activeItems[0].id})) {
                    this.item_comparison = [];
                }
                else {
                    this.item_comparison = [];
                    for (var i in this.activeItems)
                        this.item_comparison.push({id: this.activeItems[i].id, dps: null});
                }
            },

            compareItem(item) {
                var index = _.findIndex(this.item_comparison, {id: item.id});
                if (index == -1)
                    this.item_comparison.push({id: item.id, dps: null});
                else
                    this.item_comparison.splice(index, 1);
            },

            comparisonDps(item) {
                var cmp = _.find(this.item_comparison, {id: item.id});
                return cmp && cmp.dps ? _.round(cmp.dps, 2) : null;
            },

            setSpec(spec) {
                if (spec == "arcane") {
                    this.config.build = "https://www.wowhead.com/classic/talent-calc/mage/250025001001_156j976jha6j8";
                    this.config.rotation = constants.rotations.ROTATION_ST_ARCANE;
                }
                else if (spec == "arcane_heal") {
                    this.config.build = "https://www.wowhead.com/classic/talent-calc/mage/250025001001_156jg76rfa6j8";
                    this.config.rotation = constants.rotations.ROTATION_ST_ARCANE;
                }
                else if (spec == "fire") {
                    this.config.build = "https://www.wowhead.com/classic/talent-calc/mage/-505012-003_156j976jha6rb";
                    this.config.rotation = constants.rotations.ROTATION_ST_FIRE;
                }
                else if (spec == "frost") {
                    this.config.build = "https://www.wowhead.com/classic/talent-calc/mage/--053500021_156jf76vca6ja";
                    this.config.rotation = constants.rotations.ROTATION_ST_FROST;
                }

                this.parseTalents();
            },

            onBuildInput() {
                this.parseTalents();
            },

            resetTalents() {
                for (var key in this.config.talents)
                    this.config.talents[key] = 0;
                for (var key in this.config.runes)
                    this.config.runes[key] = false;
            },

            parseTalents() {
                this.resetTalents();
                var m;
                if (m = this.config.build.match(/talent-calc\/mage\/(.*)/i))
                    this.parseWowheadTalents(m[1]);
                else
                    alert("Unknown talent calculator");
            },

            parseWowheadTalents(build) {
                var encoding = "0123456789abcdefghjkmnpqrstvwxyz";
                var arr = build.split("_");
                var has_runes = arr.length > 1;
                var t = arr[0];
                var ch, value;
                var tree = 0, talent = 0;

                // Reset current talents
                for (var key in this.config.talents)
                    this.config.talents[key] = 0;

                for (var i=0; i<t.length; i++) {
                    ch = t.charAt(i);
                    if (ch == "-") {
                        tree++;
                        talent = 0;
                    }
                    else {
                        if (this.talent_map[tree][talent]) {
                            value = parseInt(ch);
                            this.config.talents[this.talent_map[tree][talent]] = value;
                        }
                        talent++;
                    }
                }

                if (has_runes) {
                    var g = arr[1];
                    var id, key, rune;
                    for (var i=1; i < g.length; i+= 4) {
                        id = (encoding.indexOf(g[i + 1]) << 10) | (encoding.indexOf(g[i + 2]) << 5) | (encoding.indexOf(g[i + 3]) << 0);
                        rune = _.find(runes, {id: id});
                        if (rune) {
                            key = rune.name.replace(/ /g, "_").toLowerCase();
                            if (this.config.runes.hasOwnProperty(key))
                                this.config.runes[key] = true;
                        }
                    }
                }
            },

            formatStats(item) {
                var stats = [];

                if (item.sp)
                    stats.push(item.sp+" sp");
                if (item.sp_arcane)
                    stats.push(item.sp+" sp (a)");
                if (item.sp_fire)
                    stats.push(item.sp+" sp (fi)");
                if (item.sp_frost)
                    stats.push(item.sp+" sp (fr)");
                if (item.hit)
                    stats.push(item.hit+"% hit");
                if (item.crit)
                    stats.push(item.crit+"% crit");
                if (item.int)
                    stats.push(item.int+" int");
                if (item.spi)
                    stats.push(item.spi+" spi");
                if (item.mp5)
                    stats.push(item.mp5+" mp5");
                if (item.mana)
                    stats.push(item.mana+" mana");
                if (item.desc)
                    stats.push(item.desc);

                return stats.join(" / ");
            },

            formatSP(data) {
                var arr = [];

                if (data.sp)
                    arr.push(data.sp);
                if (data.sp_arcane)
                    arr.push(data.sp_arcane+" arc");
                if (data.sp_frost)
                    arr.push(data.sp_frost+" frost");
                if (data.sp_fire)
                    arr.push(data.sp_fire+" fire");

                return arr.join(" / ");
            },

            formatKey(str) {
                return _.startCase(str);
            },

            formatTime(s) {
                var sign = s < 0 ? "-" : "";
                s = Math.abs(s);
                var dec = Math.round((s%1) * 100);
                var sec = Math.floor(s);
                if (dec == 100) {
                    dec = 0;
                    sec++;
                }

                return sign+(sec < 10 ? "0"+sec : sec)+"."+(dec < 10 ? "0"+dec : dec);
            },

            formatTimeDiff(start, end) {
                return this.$round(Math.abs(start.getTime() - end.getTime())/1000, 2)+"s";
            },

            formatDateTime(date) {
                return date.toLocaleTimeString("sv");
            },

            round(num) {
                return Math.round(num);
            },

            errorNotice(title, text) {
                if (typeof(text) == "string")
                    text = [text];

                this.error_notice.open = true;
                this.error_notice.title = title;
                this.error_notice.text = text;

                var self = this;
                clearTimeout(this.error_notice.timeout);
                this.error_notice.timeout = setTimeout(function() {
                    self.error_notice.open = false;
                }, 10000);
            },


            /**
             * Sim export/import
             */
            exportString() {
                var data = {
                    equipped: this.export_profile.items ? _.cloneDeep(this.equipped) : null,
                    enchants: this.export_profile.items ? _.cloneDeep(this.enchants) : null,
                    config: this.export_profile.config ? _.cloneDeep(this.config) : null,
                };

                return btoa(JSON.stringify(data));
            },

            checkImportString() {
                try {
                    var json = atob(this.import_profile.string);
                    if (!json)
                        return;
                }
                catch (e) {
                    return;
                }

                try {
                    var data = JSON.parse(json);
                }
                catch (e) {
                    return;
                }

                if (!data)
                    return;

                this.import_status.items = _.get(data, "equipped", null) !== null;
                this.import_status.config = _.get(data, "config", null) !== null;
            },

            importString(str) {
                var import_type = null;
                try {
                    var data = JSON.parse(str);
                    if (data.phase)
                        import_type = "60up";
                    else if (data.gear && data.gear.items)
                        import_type = "wse";
                }
                catch (e) {
                    var m = str.match(/https\:\/\/classic\.warcraftlogs\.com\/reports\/([a-z0-9]+)/i);
                    if (m)
                        import_type = "wcl";
                    else
                        import_type = "native";
                }

                if (import_type == "60up")
                    return this.importSixtyUpgradesString(str);
                else if (import_type == "wse")
                    return this.importWSEString(str);
                else if (import_type == "wcl") {
                    this.importWCLReport(m[1]);
                    return true;
                }
                else if (import_type != "native")
                    return this.importError("Could not parse import string");

                try {
                    var json = atob(str);
                    if (!json)
                        return this.importError("Could not parse import string");
                }
                catch (e) {
                    return this.importError("Could not parse import string");
                }

                try {
                    var data = JSON.parse(json);
                }
                catch (e) {
                    return this.importError("Could not parse import string");
                }

                if (!data)
                    return this.importError("Could not parse import string");

                if (!data.equipped && !data.enchants && !data.config)
                    return this.importError("Invalid import string");

                if (!this.import_profile.items)
                    data.equipped = data.enchants = null;
                if (!this.import_profile.config)
                    data.config = null;

                this.loadProfile(data);

                return true;
            },

            importError(err) {
                alert(err);
                this.import_profile.string = null;
                this.$refs.import_input.focus();
                return false;
            },

            doImport() {
                if (this.import_profile.string && this.importString(this.import_profile.string))
                    this.closeImport();
            },

            openExport() {
                this.export_profile.string = this.exportString();
                this.export_profile.open = true;

                this.$nextTick(function() {
                    this.$refs.export_input.select();
                });
            },

            updateExport() {
                var self = this;
                setTimeout(function() {
                    self.export_profile.string = self.exportString();

                    self.$nextTick(function() {
                        self.$refs.export_input.select();
                    });
                }, 100);
            },

            openImport() {
                this.import_profile.string = null;
                this.import_profile.open = true;

                this.$nextTick(function() {
                    this.$refs.import_input.focus();
                });
            },

            closeExport() {
                this.export_profile.open = false;
                this.export_profile.string = null;
            },

            closeImport() {
                this.import_profile.open = false;
                this.import_profile.string = null;
            },


            /**
             * Sixty upgrades import
             */
            importSixtyUpgradesString(str) {
                try {
                    var data = JSON.parse(str);
                }
                catch (e) {
                    return this.importError("Could not parse import string");
                }

                if (!data)
                    return this.importError("Could not parse import string");

                if (!data.items)
                    return this.importError("Invalid import string");

                var profile = {
                    items: null,
                    enchants: null,
                    config: _.cloneDeep(this.config),
                };

                var errors = [];

                if (this.import_profile.items) {
                    profile.equipped = {};
                    profile.enchants = {};

                    for (var key in this.equipped) {
                        profile.equipped[key] = null;
                        profile.enchants[key] = null;
                    }

                    for (var i=0; i<data.items.length; i++) {
                        var slot = this.getSlotFromSixtyUpgrades(data.items[i]);
                        if (!slot)
                            continue;
                        var id = data.items[i].id;
                        if (data.items[i].hasOwnProperty("suffixId"))
                            id+= ":"+data.items[i].suffixId;
                        var item = this.getItem(slot, id);
                        if (!item)
                            item = this.searchItem(data.items[i].name);
                        if (!item) {
                            errors.push(data.items[i].name);
                            continue;
                        }
                        profile.equipped[slot] = item.id;

                        if (data.items[i].enchant) {
                            var enchant = this.getEnchantFromSixtyUpgrades(slot, data.items[i].enchant);
                            if (!enchant)
                                errors.push(data.items[i].enchant.name);
                            else
                                profile.enchants[slot] = enchant.id;
                        }
                    }
                }

                // 60up has not implemented this yet
                if (this.import_profile.config && data.exportOptions.buffs) {

                }

                if (this.import_profile.config && data.exportOptions.talents) {
                    var talents = [
                        "000000000000000000000000000000",
                        "0000000000000000000000000000",
                        "0000000000000000000000000000",
                    ];

                    var tmap = {
                        "Arcane Subtlety": [0, 0],
                        "Arcane Focus": [0, 1],
                        "Improved Arcane Missiles": [0, 2],
                        "Wand Specialization": [0, 3],
                        "Magic Absorption": [0, 4],
                        "Arcane Concentration": [0, 5],
                        "Magic Attunement": [0, 6],
                        "Improved Arcane Explosion": [0, 7],
                        "Arcane Resilience": [0, 8],
                        "Improved Mana Shield": [0, 9],
                        "Improved Counterspell": [0, 10],
                        "Arcane Meditation": [0, 11],
                        "Presence of Mind": [0, 12],
                        "Arcane Mind": [0, 13],
                        "Arcane Instability": [0, 14],
                        "Arcane Power": [0, 15],
                        "Improved Fireball": [1, 0],
                        "Impact": [1, 1],
                        "Ignite": [1, 2],
                        "Flame Throwing": [1, 3],
                        "Improved Fire Blast": [1, 4],
                        "Incineration": [1, 5],
                        "Improved Flamestrike": [1, 6],
                        "Pyroblast": [1, 7],
                        "Burning Soul": [1, 8],
                        "Improved Scorch": [1, 9],
                        "Improved Fire Ward": [1, 10],
                        "Master of Elements": [1, 11],
                        "Critical Mass": [1, 12],
                        "Blast Wave": [1, 13],
                        "Fire Power": [1, 14],
                        "Combustion": [1, 15],
                        "Frost Warding": [2, 0],
                        "Improved Frostbolt": [2, 1],
                        "Elemental Precision": [2, 2],
                        "Ice Shards": [2, 3],
                        "Frostbite": [2, 4],
                        "Improved Frost Nova": [2, 5],
                        "Permafrost": [2, 6],
                        "Piercing Ice": [2, 7],
                        "Cold Snap": [2, 8],
                        "Improved Blizzard": [2, 9],
                        "Arctic Reach": [2, 10],
                        "Frost Channeling": [2, 11],
                        "Shatter": [2, 12],
                        "Ice Block": [2, 13],
                        "Improved Cone of Cold": [2, 14],
                        "Winter's Chill": [2, 15],
                        "Ice Barrier": [2, 16],
                    };

                    for (var talent of data.talents) {
                        if (!tmap.hasOwnProperty(talent.name))
                            return this.importError("Unknown talent: "+talent.name);
                        var t = tmap[talent.name]
                        talents[t[0]] = talents[t[0]].substr(0, t[1]) + talent.rank + talents[t[0]].substr(t[1]+1);
                    }

                    var tstring = talents[0]+"-"+talents[1]+"-"+talents[2];
                    tstring = tstring.replace(/0+\-/g, "-");
                    tstring = tstring.replace(/0+$/g, "");
                    tstring = "https://wowhead.com/classic/talent-calc/mage/"+tstring;

                    // TODO: 60up runes
                    if (data.runes && data.runes.length && false) {
                        var encoding = "0123456789abcdefghjkmnpqrstvwxyz";
                        var glyph, str, id;
                        tstring+= "_0";
                        for (var i=0, n=0; i<data.glyphs.length; i++, n++) {
                            glyph = _.find(glyphs, {itemId: data.glyphs[i].id});
                            if (!glyph)
                                continue;
                            if (data.glyphs[i].type == "MINOR" && i < 3)
                                n = 3;
                            id = glyph.spellId;
                            str = encoding[(id >> 15) & 31] + encoding[(id >> 10) & 31] + encoding[(id >> 5) & 31] + encoding[(id >> 0) & 31];
                            tstring+= n + str;
                        }
                    }

                    profile.config.build = tstring;
                }

                if (errors.length) {
                    errors.unshift("Following item(s) could not be found:");
                    this.errorNotice("Warning", errors);
                }

                this.loadProfile(profile);

                return true;
            },

            getSlotFromSixtyUpgrades(data) {
                var slot = _.isString(data) ? data : data.slot;
                slot = slot.toLowerCase();
                slot = slot.replace("finger_", "finger");
                slot = slot.replace("trinket_", "trinket");
                slot = slot.replace("shoulders", "shoulder");
                slot = slot.replace("wrists", "wrist");
                if (slot == "main_hand")
                    slot = "weapon";
                if (!this.equipped.hasOwnProperty(slot))
                    return null;
                return slot;
            },

            getEnchantFromSixtyUpgrades(slot, data) {
                if (!data.spellId) {
                    if (data.itemId) {
                        var map = {
                            28886: 35406,
                            28909: 35437,
                            23545: 29467,
                            28881: 35405,
                            28903: 35436,
                            29191: 35447,
                            19787: 24164,
                            24274: 31372,
                            24273: 31371
                        };
                        if (map.hasOwnProperty(data.itemId))
                            return this.getEnchant(slot, map[data.itemId]);
                        return this.searchEnchant(slot, data.name);
                    }
                    else {
                        return this.searchEnchant(slot, data.name);
                    }
                }

                var enchant = this.getEnchant(slot, data.spellId);
                if (!enchant)
                    enchant = this.searchEnchant(slot, data.name);
                return enchant;
            },


            /**
             * WowSims Exporter
             */
            importWSEString(str) {
                try {
                    var data = JSON.parse(str);
                }
                catch (e) {
                    return this.importError("Could not parse import string");
                }

                if (!data)
                    return this.importError("Could not parse import string");

                if (!data.gear || !data.gear.items)
                    return this.importError("Invalid import string");

                var profile = {
                    items: null,
                    enchants: null,
                    config: _.cloneDeep(this.config),
                };

                var errors = [];

                var findRune = function(g) {
                    if (_.isObject(g)) {
                        if (g.spellID) {
                            var gl = _.find(runes, {spellId: g.spellID});
                            if (gl)
                                return gl;
                        }
                        g = g.name;
                    }
                    return _.find(runes, {name: g});
                };

                if (this.import_profile.items) {
                    profile.equipped = {};
                    profile.enchants = {};

                    for (var key in this.equipped) {
                        profile.equipped[key] = null;
                        profile.enchants[key] = null;
                    }

                    var slots = [
                        "head", "neck", "shoulder", "back", "chest",
                        "wrist", "hands", "waist", "legs", "feet",
                        "finger1", "finger2", "trinket1", "trinket2",
                        "weapon", "off_hand", "ranged",
                    ];

                    for (var i=0; i<data.gear.items.length; i++) {
                        if (data.gear.items[i]) {
                            var slot = slots[i];
                            var item = this.getItem(slot, data.gear.items[i].id);
                            if (!item) {
                                errors.push(data.gear.items[i].id);
                                continue;
                            }
                            profile.equipped[slot] = data.gear.items[i].id;

                            if (data.gear.items[i].enchant) {
                                var enchant = this.getEnchantFromEnchantmentId(slot, data.gear.items[i].enchant);
                                if (!enchant)
                                    errors.push(data.gear.items[i].enchant);
                                else
                                    profile.enchants[slot] = enchant.id;
                            }
                        }
                    }
                }

                if (this.import_profile.config && data.race) {
                    var race = data.race.toLowerCase().replace(/\s+/, "");
                    if (race == "undead")
                        profile.config.race = constants.races.RACE_UNDEAD;
                    else if (race == "troll")
                        profile.config.race = constants.races.RACE_TROLL;
                    else if (race == "gnome")
                        profile.config.race = constants.races.RACE_GNOME;
                    else if (race == "human")
                        profile.config.race = constants.races.RACE_HUMAN;
                }

                if (this.import_profile.config && data.talents) {
                    var tstring = "https://wowhead.com/classic/talent-calc/mage/"+data.talents;
                    var rotation = null;

                    // TODO: Runes
                    if (data.runes && false) {
                        var encoding = "0123456789abcdefghjkmnpqrstvwxyz";
                        var rune, str, id;
                        tstring+= "_1";

                        if (data.glyphs.major) {
                            for (var i=0, n=0; i<data.glyphs.major.length; i++, n++) {
                                glyph = findGlyph(data.glyphs.major[i]);
                                if (!glyph)
                                    continue;
                                if (glyph.spellId == 56363)
                                    rotation = constants.rotations.ROTATION_ST_ARCANE_AM;
                                else if (glyph.spellId == 56368)
                                    rotation = constants.rotations.ROTATION_ST_FIRE;
                                else if (glyph.spellId == 61205)
                                    rotation = constants.rotations.ROTATION_ST_FROSTFIRE;
                                else if (glyph.spellId == 56370)
                                    rotation = constants.rotations.ROTATION_ST_FROST;
                                id = glyph.spellId;
                                str = encoding[(id >> 15) & 31] + encoding[(id >> 10) & 31] + encoding[(id >> 5) & 31] + encoding[(id >> 0) & 31];
                                tstring+= n + str;
                            }
                        }

                        if (data.glyphs.minor) {
                            for (var i=0, n=3; i<data.glyphs.minor.length; i++, n++) {
                                glyph = findGlyph(data.glyphs.minor[i]);
                                if (!glyph)
                                    continue;
                                id = glyph.spellId;
                                str = encoding[(id >> 15) & 31] + encoding[(id >> 10) & 31] + encoding[(id >> 5) & 31] + encoding[(id >> 0) & 31];
                                tstring+= n + str;
                            }
                        }
                    }

                    if (rotation && rotation != this.config.rotation) {
                        var main_rotations = [
                            constants.rotations.ROTATION_ST_ARCANE,
                            constants.rotations.ROTATION_ST_FIRE,
                            constants.rotations.ROTATION_ST_FROST,
                        ];

                        if (main_rotations.indexOf(this.config.rotation) != -1)
                            profile.config.rotation = rotation;
                    }

                    profile.config.build = tstring;
                }

                if (errors.length) {
                    errors.unshift("Following item(s) could not be found:");
                    this.errorNotice("Warning", errors);
                }

                this.loadProfile(profile);

                return true;
            },

            async importWCLReport(code) {
                this.import_wcl.open = true;
                this.import_wcl.code = code;

                if (this.wcl.hasAccessToken()) {
                    try {
                        this.import_wcl.loading = true;
                        var raid = await this.wcl.getRaid(this.import_wcl.code);
                        if (!raid)
                            throw new Error("Could not fetch raid info");
                        if (!raid.fights || !raid.fights.length)
                            throw new Error("Could not fetch any fights");
                    }
                    catch (e) {
                        return this.wclException(e);
                    }

                    this.import_wcl.loading = false;
                    this.import_wcl.raid = raid;
                }
            },

            async importWCLFight() {
                if (!this.import_wcl.fight_id)
                    return;

                this.import_wcl.loading = true;

                try {
                    var fight = await this.wcl.getFightData(this.import_wcl.code, this.import_wcl.raid, this.import_wcl.fight_id, this.import_wcl.player_id);
                }
                catch (e) {
                    return this.wclException(e);
                }

                this.import_wcl.loading = false;
                this.import_wcl.fight = fight;
            },

            importWCLConfirm() {
                var profile = {
                    equipped: null,
                    enchants: null,
                    config: null,
                };

                var fight = this.import_wcl.fight;

                if (this.import_profile.config) {
                    profile.config = _.cloneDeep(this.config);

                    if (fight.duration)
                        profile.config.duration = _.round(fight.duration/1000);

                    if (fight.dp_avg > 0) {
                        profile.config.demonic_pact = true;
                        profile.config.demonic_pact_bonus = fight.dp_avg;
                    }
                    else {
                        profile.config.demonic_pact = false;
                    }
                }

                if (this.import_profile.items && fight.player.gear && _.isArray(fight.player.gear)) {
                    profile.equipped = {};
                    profile.enchants = {};

                    var re, slot, item, eslot, enchant;
                    fight.player.gear.forEach(g => {
                        if (g.itemLevel <= 1)
                            return;
                        re = this.getSlotAndItem(g.id);
                        if (re) {
                            [slot, item] = re;
                            eslot = slot;
                            if (slot == "trinket" || slot == "finger")
                                eslot+= (profile.equipped.hasOwnProperty(slot+"1") ? "2" : "1");
                            profile.equipped[eslot] = item.id;
                            if (_.get(g, "permanentEnchant")) {
                                enchant = this.getEnchantFromEnchantmentId(slot, g.permanentEnchant);
                                if (enchant)
                                    profile.enchants[eslot] = enchant.id;
                            }
                        }
                        else {
                            console.log("Could not find item", g);
                        }
                    });
                }

                this.loadProfile(profile);
                this.closeImportWCL();
            },

            wclException(e) {
                this.import_wcl.loading = false;
                this.errorNotice("Error", e.message);
                console.error(e);
            },

            closeImportWCL() {
                this.import_wcl.open = false;
                this.import_wcl.raid = null;
                this.import_wcl.fight = null;
                this.import_wcl.fight_id = null;
                this.import_wcl.player_id = null;
            },


            nukeSettings() {
                if (!window.confirm("This will remove all profiles and configurations from this computer"))
                    return;

                localStorage.clear();
                window.location.reload(true);
            },

            saveHistory() {
                var profile = {
                    id: "",
                    name: "",
                    equipped: _.cloneDeep(this.equipped),
                    enchants: _.cloneDeep(this.enchants),
                    config: _.cloneDeep(this.config),
                }
                var str = JSON.stringify(profile);

                // Check if config is the same as last run
                if (this.history.length) {
                    var p = _.cloneDeep(this.history[0]);
                    p.id = p.name = "";
                    delete p.date;
                    delete p.end;
                    delete p.result;
                    if (JSON.stringify(p) == JSON.stringify(profile))
                        return false;
                }

                profile.date = new Date;
                profile.end = null;
                profile.result = null;

                this.history.unshift(profile);

                if (this.history.length > 30)
                    this.history.pop();

                return true;
            },

            loadHistory(profile) {
                var p = _.cloneDeep(profile);
                delete p.result;
                delete p.date;
                this.loadProfile(p);
            },

            toggleFavorite(item) {
                var id = _.isObject(item) ? item.id : item;
                var index = this.favorites.indexOf(id);
                if (index == -1)
                    this.favorites.push(id);
                else
                    this.favorites.splice(index, 1);
                this.saveFavorites();
            },

            isFavorite(item) {
                var id = _.isObject(item) ? item.id : item;
                return this.favorites.indexOf(id) != -1;
            },

            loadFavorites() {
                var str = window.localStorage.getItem("magesim_sod_favorites");
                if (!str)
                    return;

                var favorites = JSON.parse(str);
                if (!favorites || !Array.isArray(favorites))
                    return;

                this.favorites = favorites;
            },

            saveFavorites() {
                window.localStorage.setItem("magesim_sod_favorites", JSON.stringify(this.favorites));
            },

            getRotationString(rot) {
                if (rot == this.rotations.ROTATION_ST_FIRE || rot == this.rotations.ROTATION_ST_FIRE_SC)
                    return "Fire";
                if (rot == this.rotations.ROTATION_ST_FROST)
                    return "Frost";
                if (rot == this.rotations.ROTATION_ST_ARCANE)
                    return "Arcane";
                if (rot == this.rotations.ROTATION_AOE_AE)
                    return "Arcane Explosion";
                if (rot == this.rotations.ROTATION_AOE_AE_FS)
                    return "Arcane Explosion + FS";
                if (rot == this.rotations.ROTATION_AOE_BLIZZ)
                    return "Blizzard";
                if (rot == this.rotations.ROTATION_AOE_BLIZZ_FS)
                    return "Blizzard + FS";
                if (rot == this.rotations.ROTATION_AOE_FIRE)
                    return "AoE Fire";
                if (rot == this.rotations.ROTATION_AOE_FS)
                    return "Flamestrike";
                return "Unknown";
            },

            moveProfile(index, dir) {
                var pos = (this.profiles.length + index + dir) % this.profiles.length;
                this.profiles.splice(pos, 0, this.profiles.splice(index, 1)[0]);
                this.saveProfiles();
            },

            saveProfile(profile, confirm) {
                var index = _.findIndex(this.profiles, {id: profile.id});
                if (index != -1 && !window.confirm("Override "+profile.name+"?") && !confirm)
                    return;
                profile.equipped = _.cloneDeep(this.equipped);
                profile.enchants = _.cloneDeep(this.enchants);
                profile.config = _.cloneDeep(this.config);

                if (index != -1)
                    this.profiles.splice(index, 1, profile);
                else
                    this.profiles.push(profile);

                this.saveProfiles();
            },

            loadProfile(profile, only) {
                this.profile_status.open = true;
                this.profile_status.items = false;
                this.profile_status.config = false;
                this.profile_status.missing_items = [];

                if (profile.equipped && (!only || only == "items")) {
                    profile.equipped = _.pick(profile.equipped, _.keys(this.equipped));
                    delete profile.equipped.stat_weight;
                    for (var slot in profile.equipped) {
                        if (profile.equipped[slot] && !this.getItem(slot, profile.equipped[slot])) {
                            profile.equipped[slot] = null;
                            this.profile_status.missing_items.push(this.equipSlotToItemSlot(slot));
                        }
                    }
                    _.merge(this.equipped, profile.equipped);
                    this.profile_status.items = true;
                }

                if (profile.enchants && (!only || only == "items")) {
                    profile.enchants = _.pick(profile.enchants, _.keys(this.enchants));
                    for (var slot in profile.enchants) {
                        if (!this.getEnchant(slot, profile.enchants[slot]))
                            profile.enchants[slot] = null;
                    }
                    _.merge(this.enchants, profile.enchants);
                }

                if (profile.config && (!only || only == "config")) {
                    var config = _.cloneDeep(this.default_config);
                    this.config.timings = [];
                    this.config.interruptions = [];
                    _.merge(config, _.pick(profile.config, _.keys(config)));
                    _.merge(this.config, config);
                    this.onLoadConfig(profile.config);
                    this.profile_status.config = true;
                }

                this.calcStats();
                this.saveCurrentProfile();

                var self = this;
                clearTimeout(this.profile_status.timeout);
                this.profile_status.timeout = setTimeout(function() {
                    self.profile_status.open = false;
                }, 4000);
            },

            deleteProfile(profile) {
                var index = _.findIndex(this.profiles, {id: profile.id});
                if (index != -1) {
                    this.profiles.splice(index, 1);
                    this.saveProfiles();
                }
            },

            newProfile() {
                if (!this.new_profile)
                    return;

                var profile = {
                    id: this.uuid(),
                    name: this.new_profile,
                    equipped: {},
                    enchants: {},
                    config: {},
                };

                this.new_profile = null;

                this.saveProfile(profile);
            },

            uuid() {
                return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function(c) {
                    var r = Math.random() * 16 | 0, v = c == 'x' ? r : (r & 0x3 | 0x8);
                    return v.toString(16);
                });
            },

            copyEquiplist() {
                var arr = [];

                var str, item, enchant;
                for (var slot in this.equipped) {
                    if (!this.equipped[slot])
                        continue;
                    item = this.getItem(slot, this.equipped[slot]);
                    str = this.formatKey(slot)+": "+item.title;

                    if (_.get(this.enchants, slot)) {
                        enchant = this.getEnchant(slot, this.enchants[slot]);
                        str+= " ("+enchant.title+")";
                    }

                    arr.push(str);
                }

                str = arr.join("\r\n");
                this.$copyToClipboard(str);
            },

            openEquiplist() {
                this.equiplist_open = true;
            },

            closeEquiplist() {
                this.equiplist_open = false;
            },

            openCustomStats() {
                this.custom_stats_open = true;
            },

            closeCustomStats() {
                this.custom_stats_open = false;
            },

            openCustomItem() {
                var slot = this.equipSlotToItemSlot(this.active_slot);
                this.custom_item.slot = slot;
                this.custom_item_open = true;
            },

            closeCustomItem() {
                this.custom_item.id = null;
                this.custom_item.title = null;
                this.custom_item.slot = null;
                this.custom_item.int = null;
                this.custom_item.spi = null;
                this.custom_item.sp = null;
                this.custom_item.sp_arcane = null;
                this.custom_item.sp_fire = null;
                this.custom_item.sp_frost = null;
                this.custom_item.crit = null;
                this.custom_item.hit = null;
                this.custom_item.mp5 = null;
                this.custom_item.twohand = false;
                this.custom_item_open = false;
                this.custom_item_error = null;
            },

            addCustomItem() {
                this.custom_item_error = null;

                try {
                    if (!this.custom_item.slot)
                        throw new Error("Choose a slot");
                    if (!this.custom_item.title)
                        throw new Error("Enter a title");
                    if (this.custom_item.id && this.findItem(this.custom_item.id))
                        throw new Error("Item id already exists");
                }
                catch(e) {
                    this.custom_item_error = e;
                    return;
                }

                var item = _.clone(this.custom_item);
                item.custom = true;

                if (!item.id)
                    item.id = this.createItemId();

                if (item.slot != "weapon" || !item.twohand)
                    delete item.twohand;
                delete item.slot;

                for (var key in item) {
                    if (!item[key])
                        delete item[key];
                }

                this.items.equip[this.custom_item.slot].push(item);
                this.saveCustomItems();
                this.closeCustomItem();

                this.$nextTick(function() {
                    this.refreshTooltips();
                });
            },

            deleteCustomItem(item) {
                var slot = this.equipSlotToItemSlot(this.active_slot);
                var index = _.findIndex(this.items.equip[slot], {id: item.id});
                if (index != -1) {
                    if (this.isEquipped(this.active_slot, item.id))
                        this.unequip(this.active_slot);
                    this.items.equip[slot].splice(index, 1);
                    this.saveCustomItems();
                    this.refreshTooltips();
                }
            },

            customItems() {
                var items = {};

                for (var slot in this.items.equip) {
                    var arr = this.items.equip[slot].filter(item => item.custom);
                    if (arr.length)
                        items[slot] = arr;
                }

                return items;
            },

            createItemId() {
                var id;
                while (true) {
                    id = 1000000 + Math.round(Math.random() * 500000);
                    if (!this.findItem(id))
                        return id;
                }
            },

            setTab(name) {
                if (this.active_tab == name)
                    this.active_tab = "gear";
                else
                    this.active_tab = name;

                if (this.active_tab == "gear")
                    this.refreshTooltips();
            },

            allResults(result) {
                if (!result)
                    result = this.result;
                var a = document.createElement("a");
                a.href = "data:text/csv,"+encodeURIComponent(result.all_results);
                a.download = "simdata.csv";
                a.click();
            },

            onSearch(e) {
                this.refreshTooltips();
                this.saveFilters();
            },

            saveFilters() {
                var terms = this.search_item.toLowerCase().split(" ");
                terms = terms.filter(t => t.indexOf(":") != -1);
                window.localStorage.setItem("magesim_sod_filters", terms.join(" "));
            },

            loadFilters(def) {
                var filters = window.localStorage.getItem("magesim_sod_filters");
                return filters ? filters : def;
            },

            refreshTooltips() {
                if (window.$WowheadPower) {
                    window.$WowheadPower.refreshLinks();
                    this.$nextTick(function() {
                        window.$WowheadPower.refreshLinks();
                    });
                }
            },

            saveCurrentProfile() {
                var profile = {};
                profile.equipped = _.cloneDeep(this.equipped);
                profile.enchants = _.cloneDeep(this.enchants);
                profile.config = _.cloneDeep(this.config);
                window.localStorage.setItem("magesim_sod_profile", JSON.stringify(profile));
            },

            loadCurrentProfile() {
                var str = window.localStorage.getItem("magesim_sod_profile");
                if (!str)
                    return;

                var profile = JSON.parse(str);
                if (!profile)
                    return;

                this.loadProfile(profile);
            },

            onLoadConfig(cfg) {
                this.parseTalents();
            },

            loadDefaultProfiles() {
                var prof;
                for (var i=0; i<default_profiles.length; i++) {
                    prof = JSON.parse(atob(default_profiles[i].str));
                    prof.name = default_profiles[i].name;
                    prof.id = "default-profile-"+i;
                    prof.icon = default_profiles[i].icon ? default_profiles[i].icon : null;
                    this.default_profiles.push(prof);
                }
            },

            saveProfiles() {
                window.localStorage.setItem("magesim_sod_profiles", JSON.stringify(this.profiles));
            },

            loadProfiles() {
                var str = window.localStorage.getItem("magesim_sod_profiles");
                if (str) {
                    var profiles = JSON.parse(str);
                    if (profiles)
                        this.profiles = profiles;
                }
            },

            saveCustomItems() {
                window.localStorage.setItem("magesim_sod_custom_items", JSON.stringify(this.customItems()));
            },

            loadCustomItems() {
                var str = window.localStorage.getItem("magesim_sod_custom_items");
                if (str) {
                    var items = JSON.parse(str);
                    if (items) {
                        for (var slot in items) {
                            for (var i=0; i<items[slot].length; i++)
                                this.items.equip[slot].push(items[slot][i]);
                        }
                    }
                }
            },
        }
    }
</script>
