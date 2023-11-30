var ids = {
    // Trinkets
    TRINKET_MQG: 19339,
    TRINKET_ALCHEMIST_STONE: 13503,
    TRINKET_RECOMBO: 4381,
    SET_TWILIGHT_INVOKER: 1570,
};

var equip = {
    weapon: [
        { id: 209561, title: "Rod of the Ancient Sleepwalker", ilvl: 30, int: 15, spi: 14, sp: 26, twohand: true },
        { id: 211456, title: "Dagger of Willing Sacrifice", ilvl: 33, int: 3, sp: 12, q: "rare", unique: true },
        { id: 209694, title: "Blackfathom Ritual Dagger", ilvl: 31, int: 5, sp: 4, q: "rare" },
        { id: 209818, title: "Sun-Touched Crescent", ilvl: 30, "sp_fire": 9, q: "rare" },
        { id: "2077:1809", title: "Magician Staff of Arcane Wrath", ilvl: 29, sp_arcane: 16, twohand: true, q: "uncommon" },
        { id: "2077:1885", title: "Magician Staff of Fiery Wrath", ilvl: 29, sp_fire: 16, twohand: true, q: "uncommon" },
        { id: "2077:1961", title: "Magician Staff of Frozen Wrath", ilvl: 29, sp_frost: 16, twohand: true, q: "uncommon" },
        { id: "2077:772", title: "Magician Staff of the Owl", ilvl: 29, int: 7, spi: 7, twohand: true, q: "uncommon" },
        { id: "2077:177", title: "Magician Staff of Intellect", ilvl: 29, int: 11, twohand: true, q: "uncommon" },
        { id: "2819:1803", title: "Cross Dagger of Arcane Wrath", ilvl: 28, sp_arcane: 7, q: "uncommon" },
        { id: "2819:1879", title: "Cross Dagger of Fiery Wrath", ilvl: 28, sp_fire: 7, q: "uncommon" },
        { id: "2819:1955", title: "Cross Dagger of Frozen Wrath", ilvl: 28, sp_frost: 7, q: "uncommon" },
        { id: 212584, title: "Advisor's Gnarled Staff", ilvl: 30, int: 6, mp5: 4, twohand: true, q: "rare", pvp: true },
        { id: 6318, title: "Odo's Ley Staff", ilvl: 26, spi: 12, twohand: true, q: "rare" },
        { id: 6631, title: "Living Root", ilvl: 25, spi: 12, twohand: true, q: "rare" },
        { id: 890, title: "Twisted Chanter's Staff", ilvl: 24, int: 10, twohand: true, q: "rare" },
        { id: 6505, title: "Crescent Staff", ilvl: 24, int: 7, spi: 7, twohand: true, q: "rare" },
        { id: 3415, title: "Staff of the Friar", ilvl: 24, int: 3, spi: 11, twohand: true, q: "rare" },
        { id: 5201, title: "Emberstone Staff", ilvl: 23, int: 8, spi: 5, twohand: true, q: "rare" },
        { id: 1484, title: "Witching Stave", ilvl: 22, int: 8, twohand: true, q: "rare" },
        { id: 2280, title: "Kam's Walking Stick", ilvl: 27, int: 8, twohand: true, q: "uncommon" },
        { id: 2013, title: "Cryptbone Staff", ilvl: 26, int: 6, spi: 4, twohand: true, q: "uncommon" },
        { id: 16889, title: "Polished Walking Staff", ilvl: 24, int: 7, twohand: true, q: "uncommon" },
        { id: 16894, title: "Clear Crystal Rod", ilvl: 21, int: 5, twohand: true, q: "uncommon" },
        { id: 1300, title: "Lesser Staff of the Spire", ilvl: 20, int: 6, twohand: true, q: "uncommon" },
        { id: 4437, title: "Channeler's Staff", ilvl: 20, int: 6, twohand: true, q: "uncommon" },
        { id: 2567, title: "Evocator's Blade", ilvl: 23, int: 4, spi: 2, q: "rare", unique: true },
    ],
    off_hand: [
        { id: 211458, title: "Tome of Shadow Warding", ilvl: 33, int: 3, q: "rare" },
        { id: 209570, title: "Tome of Cavern Lore", ilvl: 30, sp: 7, q: "rare" },
        { id: 209590, title: "Cracked Water Globe", ilvl: 30, int: 4, spi: 5, q: "rare" },
        { id: "15973:1804", title: "Watcher's Star of Arcane Wrath", ilvl: 30, sp_arcane: 9, q: "uncommon" },
        { id: "15973:1880", title: "Watcher's Star of Fiery Wrath", ilvl: 30, sp_fire: 9, q: "uncommon" },
        { id: "15973:1956", title: "Watcher's Star of Frozen Wrath", ilvl: 30, sp_frost: 9, q: "uncommon" },
        { id: "15973:763", title: "Watcher's Star of the Owl", ilvl: 30, spi: 4, int: 4, q: "uncommon" },
        { id: "15973:133", title: "Watcher's Star of Intellect", ilvl: 30, int: 6, q: "uncommon" },
        { id: 2879, title: "Antipodean Rod", ilvl: 22, "sp_frost": 6, "sp_fire": 6, q: "rare", unique: true },
        { id: 5183, title: "Pulsating Hydra Heart", ilvl: 20, "sp_fire": 7, q: "rare" },
        { id: 16887, title: "Witch's Finger", ilvl: 30, int: 7, q: "rare" },
        { id: 15927, title: "Bright Sphere", ilvl: 27, int: 4, spi: 2, q: "uncommon" },
        { id: 4838, title: "Orb of Power", ilvl: 26, int: 4, q: "uncommon" },
        { id: 7608, title: "Seer's Fine Stein", ilvl: 21, int: 3, spi: 1, q: "uncommon" },
        { id: 15926, title: "Spellbinder Orb", ilvl: 17, int: 3, q: "uncommon" },
    ],
    ranged: [
        { id: 211461, title: "Inscribed Gravestone Scepter", ilvl: 33, spi: 1, q: "rare" },
        { id: 209674, title: "Phoenix Ignition", ilvl: 30, int: 5, q: "rare" },
        { id: 209571, title: "Deadlight", ilvl: 30, q: "rare" },
        { id: 6806, title: "Dancing Flame", ilvl: 40, q: "uncommon" },
        { id: 8184, title: "Firestarter", ilvl: 29, q: "uncommon" },
        { id: 5250, title: "Charred Wand", ilvl: 28, q: "uncommon" },
        { id: 13062, title: "Thunderwood", ilvl: 27, q: "rare" },
    ],
    head: [
        { id: 209683, itemset: ids.SET_TWILIGHT_INVOKER, title: "Twilight Invoker's Shawl", ilvl: 30, spi: 9, sp: 7, q: "rare" },
        { id: 211842, title: "Rakkamar's Tattered Thinking Cap", ilvl: 30, int: 8, spi: 6, sp: 14, q: "rare" },
        { id: 10502, title: "Spellpower Goggles Xtreme", ilvl: 43, sp: 21, q: "uncommon" },
        { id: 4373, title: "Shadow Goggles", ilvl: 24, int: 5, spi: 6, q: "uncommon" },
        { id: 10499, title: "Bright-Eye Goggles", ilvl: 35, spi: 9, q: "uncommon" },
        { id: 4385, title: "Green Tinted Goggles", ilvl: 30, spi: 7, q: "uncommon" },
        { id: 4368, title: "Flying Tiger Goggles", ilvl: 20, spi: 4, q: "uncommon" },
    ],
    neck: [
        { id: 209686, title: "Jagged Bone Necklace", ilvl: 33, int: 2, spi: 7, sp: 6, q: "rare" },
        { id: 209825, title: "Droplet Choker", ilvl: 30, spi: 6, q: "rare" },
        { id: "12047:763", title: "Spectral Necklace of the Owl", ilvl: 30, int: 4, spi: 4, q: "uncommon" },
        { id: "12047:133", title: "Spectral Necklace of Intellect", ilvl: 30, int: 6, q: "uncommon" },
        { id: 5002, title: "Glowing Green Talisman", ilvl: 30, spi: 6, q: "uncommon" },
    ],
    shoulder: [
        { id: 209679, title: "Azshari Novice's Shoulderpads", ilvl: 30, int: 10, spi: 5, q: "rare" },
        { id: 209828, title: "Sub-Zero Pauldrons", ilvl: 30, int: 6, "sp_frost": 13, q: "rare" },
        { id: 211468, title: "Frayed Chestnut Mantle", ilvl: 30, int: 6, sp: 5, q: "rare" },
        { id: 12998, title: "Magician's Mantle", ilvl: 25, int: 9, sp: 5, q: "rare" },
        { id: 3748, title: "Feline Mantle", ilvl: 28, int: 10, spi: 3, q: "rare" },
        { id: 3560, title: "Mantle of Honor", ilvl: 35, int: 7, spi: 7, q: "uncommon" },
        { id: 9536, title: "Fairywing Mantle", ilvl: 30, int: 8, q: "uncommon" },
        { id: 7750, title: "Mantle of Woe", ilvl: 33, int: 7, spi: 4, q: "uncommon" },
        { id: 6685, title: "Death Speaker Mantle", ilvl: 30, int: 7, spi: 3, q: "uncommon" },
        { id: 6395, title: "Silver-thread Amice", ilvl: 29, int: 7, q: "uncommon" },
        { id: 14378, title: "Sanguine Mantle", ilvl: 27, int: 6, q: "uncommon" },
        { id: 17695, title: "Chestnut Mantle", ilvl: 26, int: 6, q: "uncommon" },
        { id: 10657, title: "Talbar Mantle", ilvl: 26, int: 6, q: "uncommon" },
        { id: 3324, title: "Ghostly Mantle", ilvl: 28, int: 5, spi: 5, q: "uncommon" },
        { id: 4661, title: "Bright Mantle", ilvl: 26, int: 5, spi: 4, q: "uncommon" },
        { id: 6461, title: "Slime-encrusted Pads", ilvl: 27, q: "rare" },
        { id: 6664, title: "Voodoo Mantle", ilvl: 26, spi: 5, q: "uncommon" },
    ],
    back: [
        { id: 209423, title: "Flowing Scarf", ilvl: 30, int: 6, spi: 6, q: "rare" },
        { id: 209523, title: "Shimmering Thresher Cape", ilvl: 30, spi: 6, q: "rare" },
        { id: 209680, title: "Waterproof Scarf", ilvl: 30, spi: 6, q: "rare" },
        { id: 211464, title: "Worn Prelacy Cape", ilvl: 30, spi: 9, q: "rare" },
        { id: "7356:1804", title: "Elder's Cloak of Arcane Wrath", ilvl: 30, sp_arcane: 9, q: "uncommon" },
        { id: "7356:1880", title: "Elder's Cloak of Fiery Wrath", ilvl: 30, sp_fire: 9, q: "uncommon" },
        { id: "7356:1956", title: "Elder's Cloak of Frozen Wrath", ilvl: 30, sp_frost: 9, q: "uncommon" },
        { id: "7356:763", title: "Elder's Cloak of the Owl", ilvl: 30, spi: 4, int: 4, q: "uncommon" },
        { id: "7356:133", title: "Elder's Cloak of Intellect", ilvl: 30, int: 6, q: "uncommon" },
        { id: 14400, title: "Resilient Cape", ilvl: 29, int: 6, q: "uncommon" },
        { id: 13005, title: "Amy's Blanket", ilvl: 28, spi: 7, q: "rare" },
        { id: 16661, title: "Soft Willow Cape", ilvl: 27, int: 5, q: "uncommon" },
        { id: 15468, title: "Windsong Drape", ilvl: 29, int: 4, spi: 4, q: "uncommon" },
        { id: 4713, title: "Silver-thread Cloak", ilvl: 26, int: 4, spi: 2, q: "uncommon" },
        { id: 14376, title: "Sanguine Cape", ilvl: 22, int: 4, q: "uncommon" },
        { id: 211444, title: "Summoner's Cloak", ilvl: 18, int: 3, spi: 1, q: "uncommon" },
        { id: 5542, title: "Pearl-clasped Cloak", ilvl: 19, int: 3, q: "uncommon" },
        { id: 6667, title: "Engineer's Cloak", ilvl: 27, int: 2, q: "uncommon" },
    ],
    chest: [
        { id: 209671, itemset: ids.SET_TWILIGHT_INVOKER, title: "Twilight Invoker's Robes", ilvl: 30, int: 11, sp: 8, q: "rare" },
        { id: 209675, title: "Clamweave Tunic", ilvl: 30, int: 5, q: "rare" },
        { id: "14184:1809", title: "Watcher's Robes of Arcane Wrath", ilvl: 30, sp_arcane: 16, q: "uncommon" },
        { id: "14184:1885", title: "Watcher's Robes of Fiery Wrath", ilvl: 30, sp_fire: 16, q: "uncommon" },
        { id: "14184:1961", title: "Watcher's Robes of Frozen Wrath", ilvl: 30, sp_frost: 16, q: "uncommon" },
        { id: "14184:772", title: "Watcher's Robes of the Owl", ilvl: 30, int: 7, spi: 7, q: "uncommon" },
        { id: "14184:177", title: "Watcher's Robes of Intellect", ilvl: 30, int: 11, q: "uncommon" },
        { id: 4324, title: "Azure Silk Vest", ilvl: 30, int: 9, "sp_frost": 7, q: "uncommon" },
        { id: 2800, title: "Black Velvet Robes", ilvl: 26, int: 12, q: "rare" },
        { id: 2292, title: "Necrology Robes", ilvl: 25, int: 12, q: "rare" },
        { id: 1486, title: "Tree Bark Jacket", ilvl: 24, int: 10, q: "rare" },
        { id: 6324, title: "Robes of Arugal", ilvl: 29, int: 9, spi: 10, q: "rare" },
        { id: 5770, title: "Robes of Arcana", ilvl: 30, int: 8, spi: 7, q: "uncommon" },
        { id: 6608, title: "Bright Armor", ilvl: 27, int: 8, spi: 4, q: "uncommon" },
        { id: 5766, title: "Lesser Wizard's Robe", ilvl: 27, int: 8, spi: 5, q: "uncommon" },
        { id: 7510, title: "Lesser Spellfire Robes", ilvl: 20, int: 6, q: "uncommon" },
    ],
    wrist: [
        { id: "9821:1804", title: "Durable Bracers of Arcane Wrath", ilvl: 30, sp_arcane: 9, q: "uncommon" },
        { id: "9821:1880", title: "Durable Bracers of Fiery Wrath", ilvl: 30, sp_fire: 9, q: "uncommon" },
        { id: "9821:1956", title: "Durable Bracers of Frozen Wrath", ilvl: 30, sp_frost: 9, q: "uncommon" },
        { id: "9821:763", title: "Durable Bracers of the Owl", ilvl: 30, spi: 4, int: 4, q: "uncommon" },
        { id: 1974, title: "Mindthrust Bracers", ilvl: 22, int: 9, q: "rare" },
        { id: 1351, title: "Fingerbone Bracers", ilvl: 28, int: 5, q: "uncommon" },
        { id: 4036, title: "Silver-thread Cuffs", ilvl: 27, int: 5, q: "uncommon" },
        { id: 3647, title: "Bright Bracers", ilvl: 23, int: 4, q: "uncommon" },
        { id: 15452, title: "Featherbead Bracers", ilvl: 18, int: 3, q: "uncommon" },
    ],
    hands: [
        { id: 209672, title: "Black Fingerless Gloves", ilvl: 33, int: 9, spi: 4, sp: 6, q: "rare" },
        { id: 211852, title: "Handwraps of Befouled Water", ilvl: 30, int: 4, q: "rare" },
        { id: "14191:1882", title: "Raincaller Mitts of Fiery Wrath", ilvl: 30, sp_fire: 11, q: "uncommon" },
        { id: "14191:769", title: "Raincaller Mitts of the Owl", ilvl: 30, int: 6, spi: 6, q: "uncommon" },
        { id: 4319, title: "Azure Silk Gloves", ilvl: 29, spi: 3, "sp_frost": 10, q: "uncommon" },
        { id: 4331, title: "Phoenix Gloves", ilvl: 25, "sp_fire": 9, q: "uncommon" },
        { id: 892, title: "Gnoll Casting Gloves", ilvl: 22, sp: 6, q: "uncommon" },
        { id: 10654, title: "Jutebraid Gloves", ilvl: 31, int: 6, q: "uncommon" },
        { id: 16740, title: "Shredder Operating Gloves", ilvl: 31, int: 6, q: "uncommon" },
        { id: 6393, title: "Silver-thread Gloves", ilvl: 28, int: 5, spi: 5, q: "uncommon" },
        { id: 12977, title: "Magefist Gloves", ilvl: 20, int: 5, spi: 4, q: "rare" },
        { id: 14377, title: "Sanguine Handwraps", ilvl: 26, int: 4, q: "uncommon" },
        { id: 3066, title: "Bright Gloves", ilvl: 24, int: 4, spi: 4, q: "uncommon" },
        { id: 5970, title: "Serpent Gloves", ilvl: 23, int: 4, q: "uncommon" },
        { id: 10919, title: "Apothecary Gloves", ilvl: 18, int: 3, spi: 2, q: "uncommon" },
        { id: 4310, title: "Heavy Woolen Gloves", ilvl: 17, int: 2, spi: 2, q: "uncommon" },
    ],
    waist: [
        { id: 209432, title: "Ghamoo-ra's Cinch", ilvl: 30, int: 8, q: "rare" },
        { id: "10404:1882", title: "Durable Belt of Fiery Wrath", ilvl: 30, sp_fire: 11, q: "uncommon" },
        { id: "10404:769", title: "Durable Belt of the Owl", ilvl: 30, int: 6, spi: 6, q: "uncommon" },
        { id: 6392, title: "Belt of Arugal", ilvl: 29, int: 10, spi: 3, q: "rare" },
        { id: 16975, title: "Warsong Sash", ilvl: 27, int: 9, q: "rare" },
        { id: 2911, title: "Keller's Girdle", ilvl: 23, int: 8, q: "rare" },
        { id: 14406, title: "Resilient Cord", ilvl: 30, int: 6, spi: 4, q: "uncommon" },
        { id: 4829, title: "Dreamer's Belt", ilvl: 29, int: 7, spi: 2, q: "uncommon" },
        { id: 4827, title: "Wizard's Belt", ilvl: 28, int: 8, q: "uncommon" },
        { id: 4828, title: "Nightwind Belt", ilvl: 27, int: 6, q: "uncommon" },
        { id: 4714, title: "Silver-thread Sash", ilvl: 27, int: 5, q: "uncommon" },
        { id: 4785, title: "Brimstone Belt", ilvl: 24, int: 5, q: "uncommon" },
        { id: 4786, title: "Wise Man's Belt", ilvl: 20, int: 5, q: "uncommon" },
    ],
    legs: [
        { id: 209667, title: "Gaze Dreamer Leggings", ilvl: 33, int: 9, spi: 9, sp: 7, q: "rare" },
        { id: 209684, title: "Soul Leech Pants", ilvl: 33, int: 8, spi: 6, sp: 11, q: "rare" },
        { id: "14183:1885", title: "Watcher's Leggings of Fiery Wrath", ilvl: 30, sp_fire: 16, q: "uncommon" },
        { id: "14183:772", title: "Watcher's Leggings of the Owl", ilvl: 30, int: 7, spi: 7, q: "uncommon" },
        { id: 7046, title: "Azure Silk Pants", ilvl: 28, int: 6, "sp_frost": 10, q: "uncommon" },
        { id: 3073, title: "Smoldering Pants", ilvl: 26, "sp_fire": 13, q: "uncommon" },
        { id: 4317, title: "Phoenix Pants", ilvl: 25, int: 4, "sp_fire": 10, q: "uncommon" },
        { id: 6282, title: "Sacred Burial Trousers", ilvl: 32, int: 5, spi: 9, q: "uncommon" },
        { id: 12987, title: "Darkweave Breeches", ilvl: 22, int: 7, spi: 6, q: "rare" },
        { id: 4037, title: "Silver-thread Pants", ilvl: 30, int: 8, spi: 4, q: "uncommon" },
        { id: 14379, title: "Sanguine Trousers", ilvl: 29, spi: 9, q: "uncommon" },
        { id: 4316, title: "Heavy Woolen Pants", ilvl: 22, int: 3, spi: 6, q: "uncommon" },
        { id: 4723, title: "Humbert's Pants", ilvl: 29, spi: 7, q: "uncommon" },
        { id: 3067, title: "Bright Pants", ilvl: 26, spi: 6, q: "uncommon" },
    ],
    feet: [
        { id: 209669, itemset: ids.SET_TWILIGHT_INVOKER, title: "Twilight Invoker's Shoes", ilvl: 30, int: 8, spi: 8, q: "rare" },
        { id: 209670, title: "Skinwalkers", ilvl: 30, spi: 7, q: "rare" },
        { id: 211465, title: "Nimbus Boots of Insight", ilvl: 30, int: 5, spi: 4, q: "rare" },
        { id: 210795, title: "Extraplanar Spidersilk Boots", ilvl: 30, int: 5, spi: 6, sp: 7, hit: 1, unique: true },
        { id: "14195:1806", title: "Raincaller Boots of Arcane Wrath", ilvl: 29, sp_arcane: 11, q: "uncommon" },
        { id: "14195:1882", title: "Raincaller Boots of Fiery Wrath", ilvl: 29, sp_fire: 11, q: "uncommon" },
        { id: "14195:1958", title: "Raincaller Boots of Frozen Wrath", ilvl: 29, sp_frost: 11, q: "uncommon" },
        { id: "14195:769", title: "Raincaller Boots of The Owl", ilvl: 29, spi: 6, int: 6, q: "uncommon" },
        { id: 4320, title: "Spidersilk Boots", ilvl: 25, int: 4, spi: 7, q: "rare" },
        { id: 13099, title: "Moccasins of the White Hare", ilvl: 29, spi: 8, q: "rare" },
        { id: 15461, title: "Lightheel Boots", ilvl: 25, int: 4, spi: 4, q: "uncommon" },
        { id: 4321, title: "Spider Silk Slippers", ilvl: 28, spi: 7, q: "uncommon" },
        { id: 16985, title: "Windseeker Boots", ilvl: 25, spi: 6, q: "uncommon" },
        { id: 6482, title: "Firewalker Boots", ilvl: 24, int: 1, spi: 5, q: "uncommon" },
        { id: 3076, title: "Smoldering Boots", ilvl: 23, spi: 6, q: "uncommon" },
        { id: 1560, title: "Bluegill Sandals", ilvl: 21, int: 2, spi: 4, q: "uncommon" },
        { id: 6394, title: "Silver-thread Boots", ilvl: 27, int: 5, q: "uncommon" },
        { id: 4313, title: "Red Woolen Boots", ilvl: 20, spi: 5, q: "uncommon" },
        { id: 2583, title: "Woolen Boots", ilvl: 19, int: 2, spi: 2, q: "uncommon" },
    ],
    finger: [
        { id: 209668, title: "Signet of the Twilight Lord", ilvl: 30, spi: 6, sp: 7, q: "rare", unique: true },
        { id: 211462, title: "Ever-Sustaining Ring", ilvl: 30, spi: 8, q: "rare" },
        { id: 20426, title: "Advisor's Ring", ilvl: 23, sp: 5, mp5: 2, q: "rare", unique: true, pvp: true },
        { id: 13094, title: "The Queen's Jewel", ilvl: 30, int: 2, spi: 8, q: "rare", unique: true },
        { id: 6463, title: "Deep Fathom Ring", ilvl: 26, int: 3, spi: 6, q: "rare", unique: true },
        { id: 6332, title: "Black Pearl Ring", ilvl: 22, int: 1, spi: 6, q: "rare", unique: true },
        { id: "11984:763", title: "Cobalt Ring of the Owl", ilvl: 29, spi: 4, int: 4, q: "uncommon" },
        { id: "11984:133", title: "Cobalt Ring of Intellect", ilvl: 29, int: 6, q: "uncommon" },
        { id: "11984:134", title: "Cobalt Ring of Spirit", ilvl: 29, spi: 6, q: "uncommon" },
        { id: "11984:2070", title: "Cobalt Ring of Concentration", ilvl: 29, mp5: 2, q: "uncommon" },
        { id: 1077, title: "Defias Mage Ring", ilvl: 26, int: 4, spi: 3, q: "uncommon", unique: true },
        { id: 6199, title: "Black Widow Band", ilvl: 24, int: 7, q: "uncommon", unique: true },
        { id: 6750, title: "Snake Hoop", ilvl: 31, int: 6, q: "uncommon" },
        { id: 6669, title: "Sacred Band", ilvl: 25, int: 2, spi: 4, q: "uncommon" },
        { id: 5313, title: "Totemic Clan Ring", ilvl: 25, int: 3, spi: 3, q: "uncommon" },
        { id: 12996, title: "Band of Purification", ilvl: 23, spi: 6, q: "rare", unique: true },
        { id: 1156, title: "Lavishly Jeweled Ring", ilvl: 22, int: 6, q: "rare", unique: true },
        { id: 3760, title: "Band of the Undercity", ilvl: 32, spi: 5, q: "uncommon" },
        { id: 4999, title: "Azora's Will", ilvl: 27, int: 5, q: "uncommon" },
        { id: 15467, title: "Inventor's League Ring", ilvl: 31, spi: 4, q: "uncommon" },
        { id: 2039, title: "Plains Ring", ilvl: 29, int: 3, q: "rare", unique: true },
        { id: 1491, title: "Ring of Precision", ilvl: 25, spi: 3, q: "rare", unique: true },
        { id: 3739, title: "Skull Ring", ilvl: 25, int: 3, q: "uncommon" },
        { id: 8350, title: "The 1 Ring", ilvl: 15, int: 1, spi: 1, q: "uncommon" },
    ],
    trinket: [
        { id: ids.TRINKET_MQG, title: "Mind Quickening Gem", ilvl: 76, use: true, unique: true, phase: 4 },
        { id: ids.TRINKET_ALCHEMIST_STONE, title: "Alchemists' Stone", ilvl: 60, spi: 8, unique: true, phase: 4 },
        { id: 211450, title: "Invoker's Void Pearl", ilvl: 33, sp: 11, q: "rare", unique: true },
        { id: ids.TRINKET_RECOMBO, title: "Minor Recombobulator", ilvl: 28, use: true, q: "uncommon" },
    ],
};

var enchants = {
    weapon: [
        // { id: 22749, enchantmentId: 2504, title: "Spell Power", sp: 30 },
        { id: 21931, enchantmentId: 2443, title: "Winter's Might", sp_frost: 7 },
        // { id: 23804, enchantmentId: 2568, title: "Mighty Intellect", int: 22 },
        // { id: 20036, enchantmentId: 1904, title: "Major Intellect", int: 9, twohand: true },
        { id: 7793, enchantmentId: 723, title: "Lesser Intellect", int: 3, twohand: true },
        // { id: 23803, enchantmentId: 2567, title: "Mighty Spirit", spi: 20 },
        // { id: 20035, enchantmentId: 1903, title: "Major Spirit", spi: 9, twohand: true },
        { id: 13380, enchantmentId: 255, title: "Lesser Spirit", spi: 3, twohand: true },
    ],
    // head: [
    // ],
    // shoulder: [
    // ],
    chest: [
        // { id: 20025, enchantmentId: 1891, title: "Greater Stats", int: 4, spi: 4 },
        // { id: 13941, enchantmentId: 928, title: "Stats", int: 3, spi: 3 },
        { id: 13700, enchantmentId: 866, title: "Lesser Stats", int: 2, spi: 2 },
        { id: 13626, enchantmentId: 847, title: "Minor Stats", int: 1, spi: 1 },
        // { id: 20028, enchantmentId: 1893, title: "Major Mana", mana: 100 },
        // { id: 13917, enchantmentId: 913, title: "Superior Mana", mana: 65 },
        { id: 13663, enchantmentId: 857, title: "Greater Mana", mana: 50 },
        { id: 13607, enchantmentId: 843, title: "Mana", mana: 30 },
    ],
    wrist: [
        // { id: 20008, enchantmentId: 1883, title: "Greater Intellect", int: 7 },
        { id: 13822, enchantmentId: 905, title: "Intellect", int: 5 },
        { id: 13622, enchantmentId: 723, title: "Lesser Intellect", int: 3 },
        // { id: 23801, enchantmentId: 2565, title: "Mana Regeneration", mp5: 4 },
        // { id: 20009, enchantmentId: 1884, title: "Superior Spirit", spi: 9 },
        { id: 13846, enchantmentId: 807, title: "Greater Spirit", spi: 7 },
        { id: 13642, enchantmentId: 851, title: "Spirit", spi: 5 },
        { id: 7859, enchantmentId: 255, title: "Lesser Spirit", spi: 3 },
        { id: 7766, enchantmentId: 243, title: "Minor Spirit", spi: 1 },
    ],
    // hands: [
    //     { id: 25078, enchantmentId: 2616, title: "Fire Power", sp_fire: 20 },
    //     { id: 25074, enchantmentId: 2615, title: "Frost Power", sp_frost: 20 },
    // ],
    // legs: [
    // ],
    feet: [
        { id: 13890, enchantmentId: 911, title: "Minor speed" },
        // { id: 20024, enchantmentId: 851, title: "Spirit", spi: 5 },
        { id: 13687, enchantmentId: 255, title: "Lesser spirit", spi: 3 },
    ],
};

var itemsets = [

];

export default { ids, equip, enchants, itemsets };
