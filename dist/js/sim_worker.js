importScripts("./magesim.js"),onmessage=function(n){var t=n.data;"start"==t.type&&fetch("./magesim.wasm",{cache:"no-store"}).then((function(n){return n.arrayBuffer()})).then((function(n){return MageSim({wasmBinary:n})})).then((function(n){return n.ready})).then((function(n){var i=n.allocConfig();for(var r in t.config)"timings"!=r&&void 0!==i[r]&&(i[r]=t.config[r]);n.Rotation.values.hasOwnProperty(t.config.rotation)&&(i.rotation=n.Rotation.values[t.config.rotation]),n.Trinket.values.hasOwnProperty(t.config.trinket1)&&(i.trinket1=n.Trinket.values[t.config.trinket1]),n.Trinket.values.hasOwnProperty(t.config.trinket2)&&(i.trinket2=n.Trinket.values[t.config.trinket2]);for(var e=0;e<t.config.timings.length;e++)n.addTiming(i,t.config.timings[e].name,t.config.timings[e].t?t.config.timings[e].t:0,t.config.timings[e].wait_for_buff,t.config.timings[e].wait_t?t.config.timings[e].wait_t:0);for(e=0;e<t.config.interruptions.length;e++)n.addInterruption(i,t.config.interruptions[e].silence,t.config.interruptions[e].affects_all,t.config.interruptions[e].t,t.config.interruptions[e].duration);var o=JSON.parse(JSON.stringify(n.emptyStats()));for(var r in t.config.stats)o.hasOwnProperty(r)&&(o[r]=t.config.stats[r]);var a=JSON.parse(JSON.stringify(n.emptyTalents()));for(var r in t.config.talents)a.hasOwnProperty(r)&&(a[r]=t.config.talents[r]);var s=JSON.parse(JSON.stringify(n.emptyRunes()));for(var r in t.config.runes)s.hasOwnProperty(r)&&(s[r]=t.config.runes[r]);var g=n.allocPlayer(i,o,a,s);if(n.Race.values.hasOwnProperty(t.config.race)&&(g.race=n.Race.values[t.config.race]),t.iterations&&t.iterations>1)var f=n.runSimulations(i,g,t.iterations);else f=n.runSimulation(i,g);f.log&&(f.log=JSON.parse(f.log)),f.spells&&(f.spells=JSON.parse(f.spells)),f.histogram&&(f.histogram=JSON.parse(f.histogram)),postMessage({type:"success",result:f})})).catch((function(n){return console.error(n)}))};