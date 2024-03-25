importScripts("./magesim.js");

onmessage = (event) => {
    var data = event.data;

    if (data.type == "start") {
        const wasm = fetch("./magesim.wasm", {cache: "no-store"})
        .then(r => r.arrayBuffer())
        .then(binary => MageSim({wasmBinary: binary}))
        .then(w => w.ready)
        .then(m => {
            var config = m.allocConfig();
            for (var key in data.config) {
                if (key == "timings" || key == "interruptions" || key == "apl")
                    continue;
                if (typeof(config[key]) != "undefined")
                    config[key] = data.config[key];
            }
            if (m.Rotation.values.hasOwnProperty(data.config.rotation))
                config.rotation = m.Rotation.values[data.config.rotation];
            if (m.Potion.values.hasOwnProperty(data.config.potion))
                config.potion = m.Potion.values[data.config.potion];
            if (m.Trinket.values.hasOwnProperty(data.config.trinket1))
                config.trinket1 = m.Trinket.values[data.config.trinket1];
            if (m.Trinket.values.hasOwnProperty(data.config.trinket2))
                config.trinket2 = m.Trinket.values[data.config.trinket2];

            // APL
            var aplValue = function(val) {
                return {
                    type: m.APL_ValueType.values[val.type],
                    str: val.str,
                    value: val.value,
                    id: val.id,
                };
            };

            var aplAction = function(val) {
                var sequence = new m.APLActionVector();
                for (var v of val.sequence)
                    sequence.push_back(aplAction(v));

                return {
                    type: m.APL_ActionType.values[val.type],
                    str: val.str,
                    value: val.value,
                    id: val.id,
                    sequence: sequence,
                };
            };

            var aplCondition = function(val) {
                var conditions = new m.APLConditionVector();
                for (var v of val.conditions)
                    conditions.push_back(aplCondition(v));

                var values = new m.APLValueVector();
                for (var v of val.values)
                    values.push_back(aplValue(v));

                return {
                    type: m.APL_ConditionType.values[val.type],
                    op: m.APL_ConditionOp.values[val.op],
                    conditions: conditions,
                    values: values,
                };
            };

            var combat = new m.APLItemVector();
            for (var row of data.config.apl.combat) {
                if (row.status) {
                    combat.push_back({
                        condition: aplCondition(row.condition),
                        action: aplAction(row.action),
                    });
                }
            }

            config.apl = {
                combat: combat,
                precombat: new m.APLItemVector(), // TODO
            };

            for (var i=0; i<data.config.timings.length; i++) {
                m.addTiming(
                    config,
                    data.config.timings[i].name,
                    data.config.timings[i].t ? data.config.timings[i].t : 0,
                    data.config.timings[i].wait_for_buff,
                    data.config.timings[i].wait_t ? data.config.timings[i].wait_t : 0
                );
            }

            for (var i=0; i<data.config.interruptions.length; i++) {
                m.addInterruption(
                    config,
                    data.config.interruptions[i].silence,
                    data.config.interruptions[i].affects_all,
                    data.config.interruptions[i].t,
                    data.config.interruptions[i].duration
                );
            }

            var stats = JSON.parse(JSON.stringify(m.emptyStats()));
            for (var key in data.config.stats) {
                if (stats.hasOwnProperty(key))
                    stats[key] = data.config.stats[key];
            }

            var talents = JSON.parse(JSON.stringify(m.emptyTalents()));
            for (var key in data.config.talents) {
                if (talents.hasOwnProperty(key))
                    talents[key] = data.config.talents[key];
            }

            var runes = JSON.parse(JSON.stringify(m.emptyRunes()));
            for (var key in data.config.runes) {
                if (runes.hasOwnProperty(key))
                    runes[key] = data.config.runes[key];
            }

            var player = m.allocPlayer(config, stats, talents, runes);
            if (m.Race.values.hasOwnProperty(data.config.race))
                player.race = m.Race.values[data.config.race];

            if (data.iterations && data.iterations > 1)
                var result = m.runSimulations(config, player, data.iterations);
            else
                var result = m.runSimulation(config, player);

            if (result.log)
                result.log = JSON.parse(result.log);
            if (result.spells)
                result.spells = JSON.parse(result.spells);
            if (result.histogram)
                result.histogram = JSON.parse(result.histogram);

            postMessage({
                type: "success",
                result: result
            });
        })
        .catch(e => console.error(e));
    }
}