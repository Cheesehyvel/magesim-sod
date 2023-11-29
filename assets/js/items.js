var ids = {
    // Trinkets
    TRINKET_MQG: 19339,
};

var equip = {
    weapon: [
        { id: 2567, title: "Evocator's Blade", ilvl: 23, int: 4, spi: 2, q: "rare", unique: true },
    ],
    off_hand: [
        { id: 15927, title: "Bright Sphere", ilvl: 27, int: 4, spi: 2, q: "uncommon" },
    ],
    ranged: [
        { id: 8184, title: "Firestarter", ilvl: 29, q: "uncommon" },
    ],
    head: [
        { id: 4373, title: "Shadow Goggles", ilvl: 24, int: 5, spi: 6, q: "uncommon" },
    ],
    neck: [
        { id: "12047:763", title: "Spectral Necklace of the Owl", ilvl: 30, int: 4, spi: 4, q: "uncommon" },
    ],
    shoulder: [
        { id: 3560, title: "Mantle of Honor", ilvl: 35, int: 7, spi: 7, q: "uncommon" },
    ],
    back: [
        { id: "15135:763", title: "Cutthroat's Cape of the Owl", ilvl: 30, int: 4, spi: 4, q: "uncommon" },
    ],
    chest: [
        { id: 6324, title: "Robes of Arugal", ilvl: 29, int: 9, spi: 10, q: "rare" }
    ],
    wrist: [
        { id: 1974, title: "Mindthrust Bracers", ilvl: 22, int: 9, q: "rare" },
    ],
    hands: [
        { id: "14191:769", title: "Raincaller Mitts of the Owl", ilvl: 30, int: 6, spi: 6, q: "uncommon" },
    ],
    waist: [
        { id: 6392, title: "Belt of Arugal", ilvl: 29, int: 10, spi: 3, q: "rare" },
    ],
    legs: [
        { id: 4037, title: "Silver-thread Pants", ilvl: 30, int: 8, spi: 4, q: "uncommon" },
    ],
    feet: [
        { id: 4320, title: "Spidersilk Boots", ilvl: 25, int: 4, spi: 7, q: "rare" },
    ],
    finger: [
        { id: 6463, title: "Deep Fathom Ring", ilvl: 26, int: 3, spi: 6, q: "rare", unique: true },
        { id: "11984:763", title: "Cobalt Ring", ilvl: 29, q: "uncommon", int: 4, spi: 4 },
    ],
    trinket: [
        // vanilla
        { id: ids.TRINKET_MQG, title: "Mind Quickening Gem", ilvl: 76, use: true, unique: true, phase: 5 },
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
