-----------------------------------
-- Damage Spell Utilities
-- Used for spells that deal direct damage. (Black, White, Dark and Ninjutsu)
-----------------------------------
require('scripts/globals/combat/magic_hit_rate')
require('scripts/globals/jobpoints')
require('scripts/globals/magicburst')
-----------------------------------
xi = xi or {}
xi.spells = xi.spells or {}
xi.spells.damage = xi.spells.damage or {}
-----------------------------------
-- File structure:
-- 17 INDEPENDENT functions. Close them for better readability.
-- 1 FINAL function. Uses all 17 previous functions in succession and order.

-----------------------------------
-- Tables
-----------------------------------
local column =
{
    STAT_USED         =  1,
    BONUS_MACC        =  2,
    FORCE_DAY_WEATHER =  3,
    NPC_POWER         =  4,
    NPC_MULTIPLIER    =  5,
    PC_POWER          =  6,
    INFLEXION_POINT   =  7,
    MULTIPLIER_0      =  8,
    MULTIPLIER_50     =  9,
    MULTIPLIER_100    = 10,
    MULTIPLIER_200    = 11,
    MULTIPLIER_300    = 12,
    MULTIPLIER_400    = 13,
    MULTIPLIER_500    = 14,
}

local pTable =
{
-- Single target black magic spells:
--                                       1          2     3      4     5      6      7    8    9    10    11    12    13    14
-- Structure:             [spellId] = {  Stat used, mAcc, d/w,   vNPC, mNPC,  vPC,   I,   M0,  M50, M100, M200, M300, M400, M500 },
    [xi.magic.spell.AERO          ] = { xi.mod.INT,    0, false,   25,    1,   40,  35,  1.6,    1,    0,    0,    0,    0,    0 },
    [xi.magic.spell.AERO_II       ] = { xi.mod.INT,   10, false,  113,    1,  140, 133,  2.6,  1.8,    1,    0,    0,    0,    0 },
    [xi.magic.spell.AERO_III      ] = { xi.mod.INT,   20, false,  265,  1.5,  260, 295,  3.4,  2.8,  1.9,    1,    0,    0,    0 },
    [xi.magic.spell.AERO_IV       ] = { xi.mod.INT,   20, false,  440,    2,  480, 472,  4.4,  3.8,  2.9, 1.98,    1,    0,    0 },
    [xi.magic.spell.AERO_V        ] = { xi.mod.INT,   25, false,  738,  2.3,  750, 550,  5.2,  4.5,  3.9, 2.98, 1.98,    1,    0 }, -- I value unknown. Guesstimate used.
    [xi.magic.spell.AERO_VI       ] = { xi.mod.INT,    0, false, 1070,  2.5, 1070, 600,    6,  5.8,  4.8,  3.8,  2.9, 1.98,    1 }, -- I value unknown. Guesstimate used.
    [xi.magic.spell.TORNADO       ] = { xi.mod.INT,    0, false,  552,    2,  700, 577,    2,    2,    2,    2,    2,    2,    2 },
    [xi.magic.spell.TORNADO_II    ] = { xi.mod.INT,   10, false,  710,    2,  800, 780,    2,    2,    2,    2,    2,    2,    2 },
    [xi.magic.spell.BLIZZARD      ] = { xi.mod.INT,    0, false,   46,    1,   70,  60,  1.2,    1,    0,    0,    0,    0,    0 },
    [xi.magic.spell.BLIZZARD_II   ] = { xi.mod.INT,   10, false,  155,    1,  180, 178,  2.2,  1.6,    1,    0,    0,    0,    0 },
    [xi.magic.spell.BLIZZARD_III  ] = { xi.mod.INT,   20, false,  320,  1.5,  320, 345,  2.8,  2.6,  1.8,    1,    0,    0,    0 },
    [xi.magic.spell.BLIZZARD_IV   ] = { xi.mod.INT,   20, false,  506,    2,  560, 541,  3.9,  3.6,  2.8, 1.96,    1,    0,    0 },
    [xi.magic.spell.BLIZZARD_V    ] = { xi.mod.INT,   25, false,  829,  2.3,  850, 600,  4.4,    4,  3.8, 2.96, 1.96,    1,    0 }, -- I value unknown. Guesstimate used.
    [xi.magic.spell.BLIZZARD_VI   ] = { xi.mod.INT,    0, false, 1190,  2.5, 1190, 650,    5,  5.6,  4.6,  3.6,  2.8, 1.96,    1 }, -- I value unknown. Guesstimate used.
    [xi.magic.spell.FREEZE        ] = { xi.mod.INT,    0, false,  552,    2,  700, 552,    2,    2,    2,    2,    2,    2,    2 },
    [xi.magic.spell.FREEZE_II     ] = { xi.mod.INT,   10, false,  710,    2,  800, 780,    2,    2,    2,    2,    2,    2,    2 },
    [xi.magic.spell.FIRE          ] = { xi.mod.INT,    0, false,   35,    1,   55,  46,  1.4,    1,    0,    0,    0,    0,    0 },
    [xi.magic.spell.FIRE_II       ] = { xi.mod.INT,   10, false,  133,    1,  160, 155,  2.4,  1.7,    1,    0,    0,    0,    0 },
    [xi.magic.spell.FIRE_III      ] = { xi.mod.INT,   20, false,  295,  1.5,  290, 320,  3.1,  2.7, 1.85,    1,    0,    0,    0 },
    [xi.magic.spell.FIRE_IV       ] = { xi.mod.INT,   20, false,  472,    2,  520, 506,  4.2,  3.7, 2.85, 1.97,    1,    0,    0 },
    [xi.magic.spell.FIRE_V        ] = { xi.mod.INT,   25, false,  785,  2.3,  800, 550,  4.8, 4.24, 3.85, 2.97, 1.97,    1,    0 }, -- I value Unknown. Guesstimate used.
    [xi.magic.spell.FIRE_VI       ] = { xi.mod.INT,    0, false, 1130,  2.5, 1130, 600,  5.5,  5.7,  4.7,  3.7, 2.85, 1.97,    1 }, -- I value Unknown. Guesstimate used.
    [xi.magic.spell.FLARE         ] = { xi.mod.INT,    0, false,  552,    2,  700, 684,    2,    2,    2,    2,    2,    2,    2 },
    [xi.magic.spell.FLARE_II      ] = { xi.mod.INT,   10, false,  710,    2,  800, 780,    2,    2,    2,    2,    2,    2,    2 },
    [xi.magic.spell.STONE         ] = { xi.mod.INT,    0, false,   10,    1,   10,  16,    2,    1,    0,    0,    0,    0,    0 },
    [xi.magic.spell.STONE_II      ] = { xi.mod.INT,   10, false,   78,    1,  100,  95,    3,    2,    1,    0,    0,    0,    0 },
    [xi.magic.spell.STONE_III     ] = { xi.mod.INT,   20, false,  210,  1.5,  200, 236,    4,    3,    2,    1,    0,    0,    0 },
    [xi.magic.spell.STONE_IV      ] = { xi.mod.INT,   20, false,  381,    2,  400, 410,    5,    4,    3,    2,    1,    0,    0 },
    [xi.magic.spell.STONE_V       ] = { xi.mod.INT,   25, false,  626,  2.3,  650, 500,    6,    5,    4,    3,    2,    1,    0 }, -- I value Unknown. Guesstimate used.
    [xi.magic.spell.STONE_VI      ] = { xi.mod.INT,    0, false,  950,  2.5,  950, 550,    7,    6,    5,    4,    3,    2,    1 }, -- I value Unknown. Guesstimate used.
    [xi.magic.spell.QUAKE         ] = { xi.mod.INT,    0, false,  552,    2,  700, 603,    2,    2,    2,    2,    2,    2,    2 },
    [xi.magic.spell.QUAKE_II      ] = { xi.mod.INT,   10, false,  710,    2,  800, 780,    2,    2,    2,    2,    2,    2,    2 },
    [xi.magic.spell.THUNDER       ] = { xi.mod.INT,    0, false,   60,    1,   85,  78,    1,    1,    0,    0,    0,    0,    0 },
    [xi.magic.spell.THUNDER_II    ] = { xi.mod.INT,   10, false,  178,    1,  200, 210,    2,  1.5,    1,    0,    0,    0,    0 },
    [xi.magic.spell.THUNDER_III   ] = { xi.mod.INT,   20, false,  345,  1.5,  350, 381,  2.5,  2.5, 1.75,    1,    0,    0,    0 },
    [xi.magic.spell.THUNDER_IV    ] = { xi.mod.INT,   20, false,  541,    2,  600, 626,  3.6,  3.5, 2.75, 1.95,    1,    0,    0 },
    [xi.magic.spell.THUNDER_V     ] = { xi.mod.INT,   25, false,  874,  2.3,  900, 700,    4, 3.74, 3.75, 2.95, 1.95,    1,    0 }, -- I value Unknown. Guesstimate used.
    [xi.magic.spell.THUNDER_VI    ] = { xi.mod.INT,    0, false, 1250,  2.5, 1250, 750,  4.5,  5.5,  4.5,  3.5, 2.75, 1.95,    1 }, -- I value Unknown. Guesstimate used.
    [xi.magic.spell.BURST         ] = { xi.mod.INT,    0, false,  552,    2,  700, 630,    2,    2,    2,    2,    2,    2,    2 },
    [xi.magic.spell.BURST_II      ] = { xi.mod.INT,   10, false,  710,    2,  800, 780,    2,    2,    2,    2,    2,    2,    2 },
    [xi.magic.spell.WATER         ] = { xi.mod.INT,    0, false,   16,    1,   25,  25,  1.8,    1,    0,    0,    0,    0,    0 },
    [xi.magic.spell.WATER_II      ] = { xi.mod.INT,   10, false,   95,    1,  120, 113,  2.8,  1.9,    1,    0,    0,    0,    0 },
    [xi.magic.spell.WATER_III     ] = { xi.mod.INT,   20, false,  236,  1.5,  230, 265,  3.7,  2.9, 1.95,    1,    0,    0,    0 },
    [xi.magic.spell.WATER_IV      ] = { xi.mod.INT,   20, false,  410,    2,  440, 440,  4.7,  3.9, 2.95, 1.99,    1,    0,    0 },
    [xi.magic.spell.WATER_V       ] = { xi.mod.INT,   25, false,  680,  2.3,  700, 500,  5.6, 4.74, 3.95, 2.99, 1.99,    1,    0 }, -- I value Unknown. Guesstimate used.
    [xi.magic.spell.WATER_VI      ] = { xi.mod.INT,    0, false, 1010,  1.5, 1010, 550,  6.5,  5.9,  4.9,  3.9, 2.95, 1.99,    1 }, -- I value Unknown. Guesstimate used.
    [xi.magic.spell.FLOOD         ] = { xi.mod.INT,    0, false,  552,    2,  700, 657,    2,    2,    2,    2,    2,    2,    2 },
    [xi.magic.spell.FLOOD_II      ] = { xi.mod.INT,   10, false,  710,    2,  800, 780,    2,    2,    2,    2,    2,    2,    2 },
    [xi.magic.spell.IMPACT        ] = { xi.mod.INT,    0, false,  932,  2.3,  932, 525,    0,    0,    0,    0,    0,    0,    0 }, -- I value unknown. Guesstimate used.
    [xi.magic.spell.COMET         ] = { xi.mod.INT,    0, false,  552,    2,  700, 700,    2,    2,    2,    2,    2,    2,    2 }, -- I value unknown. Guesstimate used.
    [xi.magic.spell.DEATH         ] = {          0,    0, false,   32,    0,   32,   0,    0,    0,    0,    0,    0,    0,    0 },

    -- Dia as nuke.
    [xi.magic.spell.DIA           ] = { xi.mod.MND,    0, false,    1,    1,    1,   1,    0,    0,    0,    0,    0,    0,    0 },
    [xi.magic.spell.DIA_II        ] = { xi.mod.MND,    0, false,    4,    1,    4,   2,    0,    0,    0,    0,    0,    0,    0 },
    [xi.magic.spell.DIA_III       ] = { xi.mod.MND,    0, false,   16,    1,   16,   4,    0,    0,    0,    0,    0,    0,    0 },
    [xi.magic.spell.DIA_IV        ] = { xi.mod.MND,    0, false,   64,    1,   64,   8,    0,    0,    0,    0,    0,    0,    0 },
    [xi.magic.spell.DIA_V         ] = { xi.mod.MND,    0, false,  256,    1,  256,  16,    0,    0,    0,    0,    0,    0,    0 },

    [xi.magic.spell.DIAGA         ] = { xi.mod.MND,    0, false,    2,    1,    2,   2,    0,    0,    0,    0,    0,    0,    0 },
    [xi.magic.spell.DIAGA_II      ] = { xi.mod.MND,    0, false,    8,    1,    8,   4,    0,    0,    0,    0,    0,    0,    0 },
    [xi.magic.spell.DIAGA_III     ] = { xi.mod.MND,    0, false,   32,    1,   32,   8,    0,    0,    0,    0,    0,    0,    0 },
    [xi.magic.spell.DIAGA_IV      ] = { xi.mod.MND,    0, false,  128,    1,  128,  16,    0,    0,    0,    0,    0,    0,    0 },
    [xi.magic.spell.DIAGA_V       ] = { xi.mod.MND,    0, false,  512,    1,  512,  32,    0,    0,    0,    0,    0,    0,    0 },

    -- Bio as nuke.
    [xi.magic.spell.BIO           ] = { xi.mod.INT,    0, false,   10,    1,   10,   5,    0,    0,    0,    0,    0,    0,    0 },
    [xi.magic.spell.BIO_II        ] = { xi.mod.INT,    0, false,   50,    1,   50,  10,    0,    0,    0,    0,    0,    0,    0 },
    [xi.magic.spell.BIO_III       ] = { xi.mod.INT,    0, false,  100,  1.5,  100,  21,    0,    0,    0,    0,    0,    0,    0 },
    [xi.magic.spell.BIO_IV        ] = { xi.mod.INT,    0, false,  125,  1.5,  125,  27,    0,    0,    0,    0,    0,    0,    0 },
    [xi.magic.spell.BIO_V         ] = { xi.mod.INT,    0, false,  150,    2,  150,  32,    0,    0,    0,    0,    0,    0,    0 },

    -- Helixes (Initial damage) https://www.bluegartr.com/threads/108196-Random-Facts-Thread-Magic?p=6817880&viewfull=1#post6817880
    [xi.magic.spell.GEOHELIX      ] = { xi.mod.INT,    0, true,    35,    1,   31, 100,    1,    1,  0.5,    0,    0,    0,    0 },
    [xi.magic.spell.GEOHELIX_II   ] = { xi.mod.INT,    0, true,    75,    2,   75, 100,    2,    1,    0,    0,    0,    0,    0 },
    [xi.magic.spell.HYDROHELIX    ] = { xi.mod.INT,    0, true,    35,    1,   31, 100,    1,    1,  0.5,    0,    0,    0,    0 },
    [xi.magic.spell.HYDROHELIX_II ] = { xi.mod.INT,    0, true,    75,    2,   75, 100,    2,    1,    0,    0,    0,    0,    0 },
    [xi.magic.spell.ANEMOHELIX    ] = { xi.mod.INT,    0, true,    35,    1,   31, 100,    1,    1,  0.5,    0,    0,    0,    0 },
    [xi.magic.spell.ANEMOHELIX_II ] = { xi.mod.INT,    0, true,    75,    2,   75, 100,    2,    1,    0,    0,    0,    0,    0 },
    [xi.magic.spell.PYROHELIX     ] = { xi.mod.INT,    0, true,    35,    1,   31, 100,    1,    1,  0.5,    0,    0,    0,    0 },
    [xi.magic.spell.PYROHELIX_II  ] = { xi.mod.INT,    0, true,    75,    2,   75, 100,    2,    1,    0,    0,    0,    0,    0 },
    [xi.magic.spell.CRYOHELIX     ] = { xi.mod.INT,    0, true,    35,    1,   31, 100,    1,    1,  0.5,    0,    0,    0,    0 },
    [xi.magic.spell.CRYOHELIX_II  ] = { xi.mod.INT,    0, true,    75,    2,   75, 100,    2,    1,    0,    0,    0,    0,    0 },
    [xi.magic.spell.IONOHELIX     ] = { xi.mod.INT,    0, true,    35,    1,   31, 100,    1,    1,  0.5,    0,    0,    0,    0 },
    [xi.magic.spell.IONOHELIX_II  ] = { xi.mod.INT,    0, true,    75,    2,   75, 100,    2,    1,    0,    0,    0,    0,    0 },
    [xi.magic.spell.NOCTOHELIX    ] = { xi.mod.INT,    0, true,    35,    1,   31, 100,    1,    1,  0.5,    0,    0,    0,    0 },
    [xi.magic.spell.NOCTOHELIX_II ] = { xi.mod.INT,    0, true,    75,    2,   75, 100,    2,    1,    0,    0,    0,    0,    0 },
    [xi.magic.spell.LUMINOHELIX   ] = { xi.mod.INT,    0, true,    35,    1,   31, 100,    1,    1,  0.5,    0,    0,    0,    0 },
    [xi.magic.spell.LUMINOHELIX_II] = { xi.mod.INT,    0, true,    75,    2,   75, 100,    2,    1,    0,    0,    0,    0,    0 },

-- Multiple target spells:
--                                       1          2     3      4     5      6      7    8    9    10    11    12    13    14
-- Structure:             [spellId] = {  Stat used, mAcc, d/w,   vNPC, mNPC,  vPC,   I,   M0,  M50, M100, M200, M300, M400, M500 },
    [xi.magic.spell.AEROGA        ] = { xi.mod.INT,    0, false,   93,    1,  100, 120,  2.6,  1.8,    1,    0,    0,    0,    0 },
    [xi.magic.spell.AEROGA_II     ] = { xi.mod.INT,    0, false,  266,    1,  310, 312,  3.4,  2.8,  1.9,    1,    0,    0,    0 },
    [xi.magic.spell.AEROGA_III    ] = { xi.mod.INT,    0, false,  527,  1.5,  580, 642,  4.4,  3.8,  2.9, 1.98,    1,    0,    0 },
    [xi.magic.spell.AEROGA_IV     ] = { xi.mod.INT,    0, false,  738,    2,    0, 700,    1,    1,    1,    1,    1,    1,    0 }, -- Enemy only. No data found. Values taken from Aero V.
    [xi.magic.spell.AEROGA_V      ] = { xi.mod.INT,    0, false, 1070,  2.3,    0, 750,    1,    1,    1,    1,    1,    1,    1 }, -- Enemy only. No data found. Values taken from Aero VI.
    [xi.magic.spell.AERA          ] = { xi.mod.INT,    0, false,  210,    1,  210, 250,  2.6,  1.8,    1,    0,    0,    0,    0 },
    [xi.magic.spell.AERA_II       ] = { xi.mod.INT,    0, false,  430,    1,  430, 600,  3.4,  2.8,  1.9,    1,    0,    0,    0 },
    [xi.magic.spell.AERA_III      ] = { xi.mod.INT,    0, false,  710,  1.5,  710, 700,  4.4,  3.8,  2.9, 1.98,    1,    0,    0 }, -- No info found. Since Aera I and II N Values coincided with Aeroga 1 and II, used Values of Aeroga III.
    [xi.magic.spell.AEROJA        ] = { xi.mod.INT,    0, false,  844,  2.3,  850, 800,  5.2,  4.5,  3.9,    3,    2,    1,    0 }, -- Some values not found. Used guesstimates for M200 and M300.
    [xi.magic.spell.BLIZZAGA      ] = { xi.mod.INT,    0, false,  145,    1,  160, 172,  2.2,  1.6,    1,    0,    0,    0,    0 },
    [xi.magic.spell.BLIZZAGA_II   ] = { xi.mod.INT,    0, false,  350,    1,  370, 392,  2.8,  2.6,  1.8,    1,    0,    0,    0 },
    [xi.magic.spell.BLIZZAGA_III  ] = { xi.mod.INT,    0, false,  642,  1.5,  660, 697,  3.9,  3.6,  2.8, 1.96,    1,    0,    0 },
    [xi.magic.spell.BLIZZAGA_IV   ] = { xi.mod.INT,    0, false,  829,    2,    0, 800,    1,    1,    1,    1,    1,    1,    0 }, -- Enemy only. No data found. Values taken from Blizzard V.
    [xi.magic.spell.BLIZZAGA_V    ] = { xi.mod.INT,    0, false, 1190,  2.3,    0, 950,    1,    1,    1,    1,    1,    1,    1 }, -- Enemy only. No data found. Values taken from Blizzard VI.
    [xi.magic.spell.BLIZZARA      ] = { xi.mod.INT,    0, false,  270,    1,  270, 300,  2.2,  1.6,    1,    0,    0,    0,    0 },
    [xi.magic.spell.BLIZZARA_II   ] = { xi.mod.INT,    0, false,  510,    1,  510, 550,  2.8,  2.6,  1.8,    1,    0,    0,    0 },
    [xi.magic.spell.BLIZZARA_III  ] = { xi.mod.INT,    0, false,  830,  1.5,  830, 850,  3.9,  3.6,  2.8, 1.96,    1,    0,    0 }, -- No info found. Since Blizzara I and II N Values coincided with Blizzaga 1 and II, used Values of Blizzaga III.
    [xi.magic.spell.BLIZZAJA      ] = { xi.mod.INT,    0, false,  953,  2.3,  950, 950,  4.4,    4,  3.8,    3,    2,    1,    0 }, -- Some values not found. Used guesstimates for M200 and M300.
    [xi.magic.spell.FIRAGA        ] = { xi.mod.INT,    0, false,  120,    1,  120, 145,  2.4,  1.7,    1,    0,    0,    0,    0 },
    [xi.magic.spell.FIRAGA_II     ] = { xi.mod.INT,    0, false,  312,    1,  340, 350,  3.1,  2.7, 1.85,    1,    0,    0,    0 },
    [xi.magic.spell.FIRAGA_III    ] = { xi.mod.INT,    0, false,  589,  1.5,  620, 642,  4.2,  3.7, 2.85, 1.97,    1,    0,    0 },
    [xi.magic.spell.FIRAGA_IV     ] = { xi.mod.INT,    0, false,  785,    2,    0, 700,    1,    1,    1,    1,    1,    1,    0 }, -- Enemy only. No data found. Values taken from Fire V.
    [xi.magic.spell.FIRAGA_V      ] = { xi.mod.INT,    0, false, 1130,  2.3,    0, 800,    1,    1,    1,    1,    1,    1,    1 }, -- Enemy only. No data found. Values taken from Fire VI.
    [xi.magic.spell.FIRA          ] = { xi.mod.INT,    0, false,  240,    1,  240, 250,  2.4,  1.7,    1,    0,    0,    0,    0 },
    [xi.magic.spell.FIRA_II       ] = { xi.mod.INT,    0, false,  470,    1,  470, 500,  3.1,  2.7, 1.85,    1,    0,    0,    0 },
    [xi.magic.spell.FIRA_III      ] = { xi.mod.INT,    0, false,  760,  1.5,  760, 800,  4.2,  3.7, 2.85, 1.97,    1,    0,    0 }, -- No info found. Since Fira I and II N Values coincided with Firaga 1 and II, used Values of Firaga III.
    [xi.magic.spell.FIRAJA        ] = { xi.mod.INT,    0, false,  902,  2.3,  900, 950,  4.8, 4.25, 3.85,    3,    2,    1,    0 }, -- Some values not found. Used guesstimates for M200 and M300.
    [xi.magic.spell.STONEGA       ] = { xi.mod.INT,    0, false,   56,    1,   60,  74,    3,    2,    1,    0,    0,    0,    0 },
    [xi.magic.spell.STONEGA_II    ] = { xi.mod.INT,    0, false,  201,    1,  250, 232,    4,    3,    2,    1,    0,    0,    0 },
    [xi.magic.spell.STONEGA_III   ] = { xi.mod.INT,    0, false,  434,  1.5,  500, 480,    5,    4,    3,    2,    1,    0,    0 },
    [xi.magic.spell.STONEGA_IV    ] = { xi.mod.INT,    0, false,  626,    2,    0, 650,    1,    1,    1,    1,    1,    1,    0 }, -- Enemy only. No data found. Values taken from Stone V.
    [xi.magic.spell.STONEGA_V     ] = { xi.mod.INT,    0, false,  950,  2.3,    0, 950,    1,    1,    1,    1,    1,    1,    1 }, -- Enemy only. No data found. Values taken from Stone VI.
    [xi.magic.spell.STONERA       ] = { xi.mod.INT,    0, false,  150,    1,  150, 150,    3,    2,    1,    0,    0,    0,    0 },
    [xi.magic.spell.STONERA_II    ] = { xi.mod.INT,    0, false,  350,    1,  350, 350,    4,    3,    2,    1,    0,    0,    0 },
    [xi.magic.spell.STONERA_III   ] = { xi.mod.INT,    0, false,  650,  1.5,  650, 650,    5,    4,    3,    2,    1,    0,    0 }, -- No info found. Since Stonera I and II N Values coincided with Stonega 1 and II, used Values of Stonega III.
    [xi.magic.spell.STONEJA       ] = { xi.mod.INT,    0, false,  719,  2.3,  750, 750,    6,    5,    4,    3,    2,    1,    0 }, -- Some values not found. Used guesstimates for M200 and M300.
    [xi.magic.spell.THUNDAGA      ] = { xi.mod.INT,    0, false,  172,    1,  200, 201,    2,  1.5,    1,    0,    0,    0,    0 },
    [xi.magic.spell.THUNDAGA_II   ] = { xi.mod.INT,    0, false,  392,    1,  400, 434,  2.5,  2.5, 1.75,    1,    0,    0,    0 },
    [xi.magic.spell.THUNDAGA_III  ] = { xi.mod.INT,    0, false,  697,  1.5,  700, 719,  3.6,  3.5, 2.75, 1.95,    1,    0,    0 },
    [xi.magic.spell.THUNDAGA_IV   ] = { xi.mod.INT,    0, false,  874,    2,    0, 900,    1,    1,    1,    1,    1,    1,    0 }, -- Enemy only. No data found. Values taken from Thunder V.
    [xi.magic.spell.THUNDAGA_V    ] = { xi.mod.INT,    0, false, 1250,  2.3,    0, 999,    1,    1,    1,    1,    1,    1,    1 }, -- Enemy only. No data found. Values taken from Thunder VI.
    [xi.magic.spell.THUNDARA      ] = { xi.mod.INT,    0, false,  300,    1,  300, 300,    2,  1.5,    1,    0,    0,    0,    0 },
    [xi.magic.spell.THUNDARA_II   ] = { xi.mod.INT,    0, false,  550,    1,  550, 550,  2.5,  2.5, 1.75,    1,    0,    0,    0 },
    [xi.magic.spell.THUNDARA_III  ] = { xi.mod.INT,    0, false,  900,  1.5,  900, 900,  3.6,  3.5, 2.75, 1.95,    1,    0,    0 }, -- No info found. Since Thundara I and II N Values coincided with Thundaga 1 and II, used Values of Thundaga III.
    [xi.magic.spell.THUNDAJA      ] = { xi.mod.INT,    0, false, 1005,  2.3, 1000, 999,    4, 3.75, 3.75,    3,    2,    1,    0 }, -- Some values not found. Used guesstimates for M200 and M300.
    [xi.magic.spell.WATERGA       ] = { xi.mod.INT,    0, false,   74,    1,   80,  96,  2.8,  1.9,    1,    0,    0,    0,    0 },
    [xi.magic.spell.WATERGA_II    ] = { xi.mod.INT,    0, false,  232,    1,  280, 266,  3.7,  2.9, 1.95,    1,    0,    0,    0 },
    [xi.magic.spell.WATERGA_III   ] = { xi.mod.INT,    0, false,  480,  1.5,  540, 527,  4.7,  3.9, 2.95, 1.99,    1,    0,    0 },
    [xi.magic.spell.WATERGA_IV    ] = { xi.mod.INT,    0, false,  680,    2,    0, 700,    1,    1,    1,    1,    1,    1,    0 }, -- Enemy only. No data found. Values taken from Water V.
    [xi.magic.spell.WATERGA_V     ] = { xi.mod.INT,    0, false, 1010,  2.3,    0, 900,    1,    1,    1,    1,    1,    1,    1 }, -- Enemy only. No data found. Values taken from Water VI.
    [xi.magic.spell.WATERA        ] = { xi.mod.INT,    0, false,  180,    1,  180, 200,  2.8,  1.9,    1,    0,    0,    0,    0 },
    [xi.magic.spell.WATERA_II     ] = { xi.mod.INT,    0, false,  390,    1,  390, 400,  3.7,  2.9, 1.95,    1,    0,    0,    0 },
    [xi.magic.spell.WATERA_III    ] = { xi.mod.INT,    0, false,  660,  1.5,  660, 700,  4.7,  3.9, 2.95, 1.99,    1,    0,    0 }, -- No info found. Since Watera I and II N Values coincided with Waterga 1 and II, used Values of Waterga III.
    [xi.magic.spell.WATERJA       ] = { xi.mod.INT,    0, false,  782,  2.3,  800, 900,  5.6, 4.75, 3.95,    3,    2,    1,    0 }, -- Some values not found. Used guesstimates for M200 and M300.

-- Dark spells.
--                                       1          2     3      4     5      6      7    8    9    10    11    12    13    14
-- Structure:             [spellId] = {  Stat used, mAcc, d/w,   vNPC, mNPC,  vPC,   I,   M0,  M50, M100, M200, M300, M400, M500 },
    [xi.magic.spell.KAUSTRA       ] = { xi.mod.INT,    0, false,    0, 0.67,    0, 300, 0.67, 0.67, 0.67, 0.67,    0,    0,    0 },

-- Ninjutsu spells: https://www.ffxiah.com/forum/topic/56749/updated-ninjutsu-damage-formulae/
--                                       1          2     3      4        5   6      7  8
-- Structure:             [spellId] = {  Stat used, mAcc, d/w,   vNPC,    M,  vPC,   I, M0 },
    [xi.magic.spell.DOTON_ICHI    ] = { xi.mod.INT,    0, false,   16,    1,   16,  25, 0 },
    [xi.magic.spell.DOTON_NI      ] = { xi.mod.INT,    0, false,   69,    1,   69, 113, 0 },
    [xi.magic.spell.DOTON_SAN     ] = { xi.mod.INT,    0, false,  134,    2,  134, 118, 0 },
    [xi.magic.spell.HUTON_ICHI    ] = { xi.mod.INT,    0, false,   16,    1,   16,  25, 0 },
    [xi.magic.spell.HUTON_NI      ] = { xi.mod.INT,    0, false,   69,    1,   69, 113, 0 },
    [xi.magic.spell.HUTON_SAN     ] = { xi.mod.INT,    0, false,  134,    2,  134, 118, 0 },
    [xi.magic.spell.HYOTON_ICHI   ] = { xi.mod.INT,    0, false,   16,    1,   16,  25, 0 },
    [xi.magic.spell.HYOTON_NI     ] = { xi.mod.INT,    0, false,   69,    1,   69, 113, 0 },
    [xi.magic.spell.HYOTON_SAN    ] = { xi.mod.INT,    0, false,  134,    2,  134, 118, 0 },
    [xi.magic.spell.KATON_ICHI    ] = { xi.mod.INT,    0, false,   16,    1,   16,  25, 0 },
    [xi.magic.spell.KATON_NI      ] = { xi.mod.INT,    0, false,   69,    1,   69, 113, 0 },
    [xi.magic.spell.KATON_SAN     ] = { xi.mod.INT,    0, false,  134,    2,  134, 118, 0 },
    [xi.magic.spell.RAITON_ICHI   ] = { xi.mod.INT,    0, false,   16,    1,   16,  25, 0 },
    [xi.magic.spell.RAITON_NI     ] = { xi.mod.INT,    0, false,   69,    1,   69, 113, 0 },
    [xi.magic.spell.RAITON_SAN    ] = { xi.mod.INT,    0, false,  134,    2,  134, 118, 0 },
    [xi.magic.spell.SUITON_ICHI   ] = { xi.mod.INT,    0, false,   16,    1,   16,  25, 0 },
    [xi.magic.spell.SUITON_NI     ] = { xi.mod.INT,    0, false,   69,    1,   69, 113, 0 },
    [xi.magic.spell.SUITON_SAN    ] = { xi.mod.INT,    0, false,  134,    2,  134, 118, 0 },

-- Divine spells: https://nw6yx36onohv5j6wmzoba3nllq-ac4c6men2g7xr2a-wiki-ffo-jp.translate.goog/html/1963.html
--                                       1          2     3      4        5   6      7  8
-- Structure:             [spellId] = {  Stat used, mAcc, d/w,   vNPC,    M,  vPC,   I, M0 },
    [xi.magic.spell.BANISH        ] = { xi.mod.MND,    0, false,   14,    1,   14,  25, 0 },
    [xi.magic.spell.BANISH_II     ] = { xi.mod.MND,    0, false,   85,    1,   85, 113, 0 },
    [xi.magic.spell.BANISH_III    ] = { xi.mod.MND,    0, false,  198,  1.5,  198, 250, 0 },
    [xi.magic.spell.BANISH_IV     ] = { xi.mod.MND,    0, false,  420,  1.5,  420, 400, 0 }, -- Enemy only. Stats unknown/unchecked.
    [xi.magic.spell.BANISHGA      ] = { xi.mod.MND,    0, false,   50,    1,   50,  46, 0 },
    [xi.magic.spell.BANISHGA_II   ] = { xi.mod.MND,    0, false,  180,    1,  180, 133, 0 },
    [xi.magic.spell.BANISHGA_III  ] = { xi.mod.MND,    0, false,  480,  1.5,  480, 450, 0 }, -- Enemy only. Stats unknown.
    [xi.magic.spell.BANISHGA_IV   ] = { xi.mod.MND,    0, false,  600,  1.5,  600, 600, 0 }, -- Enemy only. Stats unknown.
    [xi.magic.spell.HOLY          ] = { xi.mod.MND,    0, false,  125,    1,  125, 150, 0 },
    [xi.magic.spell.HOLY_II       ] = { xi.mod.MND,    0, false,  250,    2,  250, 300, 0 },

-- Healing Spells when used against undead/zombie: https://wiki.ffo.jp/html/336.html
--                                       1          2     3      4        5   6      7  8
-- Structure:             [spellId] = {  Stat used, mAcc, d/w,   vNPC,    M,  vPC,   I, M0 },
    [xi.magic.spell.CURE          ] = { xi.mod.MND,    0, false,    7,    1,    7,  16, 0 },
    [xi.magic.spell.CURE_II       ] = { xi.mod.MND,    0, false,   35,    1,   35,  60, 0 },
    [xi.magic.spell.CURE_III      ] = { xi.mod.MND,    0, false,   70,    1,   70, 133, 0 },
    [xi.magic.spell.CURE_IV       ] = { xi.mod.MND,    0, false,  140,  1.5,  140, 104, 0 }, -- Not a typo. Both Inflexion point and upper cap by extension are lower than Cure III.
    [xi.magic.spell.CURE_V        ] = { xi.mod.MND,    0, false,  210,  1.5,  210, 159, 0 },
    [xi.magic.spell.CURE_VI       ] = { xi.mod.MND,    0, false,  295,    2,  295, 212, 0 },
}

local function cardinalChantBonus(actor, target, direction, spellId, skillType)
    -- https://www.bg-wiki.com/ffxi/Cardinal_Chant
    local chantBonus = 0

    -- Early return
    if spellId == 0 or skillType ~= xi.skill.ELEMENTAL_MAGIC then
        return chantBonus
    end

    -- Calculate base bonus.
    local raSpellTable =
    set{
        xi.magic.spell.STONERA,  xi.magic.spell.STONERA_II,  xi.magic.spell.STONERA_III,
        xi.magic.spell.WATERA,   xi.magic.spell.WATERA_II,   xi.magic.spell.WATERA_III,
        xi.magic.spell.AERA,     xi.magic.spell.AERA_II,     xi.magic.spell.AERA_III,
        xi.magic.spell.FIRA,     xi.magic.spell.FIRA_II,     xi.magic.spell.FIRA_III,
        xi.magic.spell.BLIZZARA, xi.magic.spell.BLIZZARA_II, xi.magic.spell.BLIZZARA_III,
        xi.magic.spell.THUNDARA, xi.magic.spell.THUNDARA_II, xi.magic.spell.THUNDARA_III,
    }

    local chantTable =
    {
        [0] = { [xi.direction.EAST] = {  0,  0 }, [xi.direction.SOUTH] = {  0,  0 }, [xi.direction.WEST] = {  0,  0 }, [xi.direction.NORTH] = {  0,  0 } },
        [1] = { [xi.direction.EAST] = {  5,  8 }, [xi.direction.SOUTH] = {  5,  8 }, [xi.direction.WEST] = { 10, 15 }, [xi.direction.NORTH] = {  5,  8 } },
        [2] = { [xi.direction.EAST] = {  7, 10 }, [xi.direction.SOUTH] = {  7, 10 }, [xi.direction.WEST] = { 14, 19 }, [xi.direction.NORTH] = {  7, 10 } },
        [3] = { [xi.direction.EAST] = { 10, 14 }, [xi.direction.SOUTH] = { 10, 14 }, [xi.direction.WEST] = { 18, 24 }, [xi.direction.NORTH] = { 10, 14 } },
        [4] = { [xi.direction.EAST] = { 13, 17 }, [xi.direction.SOUTH] = { 13, 17 }, [xi.direction.WEST] = { 22, 28 }, [xi.direction.NORTH] = { 13, 17 } },
    }

    local isRaSpell = raSpellTable[spellId] and 2 or 1
    local baseBonus = chantTable[actor:getMod(xi.mod.CARDINAL_CHANT)][direction][isRaSpell]

    -- Calculate fervor %
    local fervorFactor = actor:hasStatusEffect(xi.effect.COLLIMATED_FERVOR) and 1.5 or 1

    -- Calculate gear %
    local gearFactor = 1 + actor:getMod(xi.mod.CARDINAL_CHANT_BONUS) / 100

    -- Calculate angle %
    local angle       = utils.getWorldRotation(actor:getPos(), target:getPos())
    local angleFactor = 0

    switch (direction) : caseof
    {
        [xi.direction.EAST] = function() -- MAB -> Optimal angle = 0
            if angle > 192 and angle < 256 then
                angleFactor = 1 - (256 - angle) / 64
            elseif angle >= 0 and angle < 64 then
                angleFactor = 1 - angle / 64
            end
        end,

        [xi.direction.SOUTH] = function() -- MACC -> Optimal angle = 64
            if angle > 0 and angle < 64 then
                angleFactor = 1 - (64 - angle) / 64
            elseif angle >= 64 and angle < 128 then
                angleFactor = 1 - (angle - 64) / 64
            end
        end,

        [xi.direction.WEST] = function() -- MBB -> Optimal angle = 128
            if angle > 64 and angle < 128 then
                angleFactor = 1 - (128 - angle) / 64
            elseif angle >= 128 and angle < 192 then
                angleFactor = 1 - (angle - 128) / 64
            end
        end,

        [xi.direction.NORTH] = function() -- M.Crit -> Optimal angle = 192
            if angle > 128 and angle < 192 then
                angleFactor = 1 - (192 - angle) / 64
            elseif angle >= 192 and angle < 256 then
                angleFactor = 1 - (angle - 192) / 64
            end
        end,
    }

    chantBonus = math.floor(baseBonus * fervorFactor * gearFactor * angleFactor)

    return chantBonus
end

-----------------------------------
-- Base spell damage pure helpers
-- Dual-wired to OmegaXI internal/spellbasedmg (slice 6713 / 0857).
-----------------------------------

xi.spells.damage.baseDamageMin = 0
xi.spells.damage.baseDamageMax = 99999

-- New-system soft-cap ladder thresholds/widths (mTable).
xi.spells.damage.newSystemStatLadder =
{
    [1] = {   0,  50 },
    [2] = {  50,  50 },
    [3] = { 100, 100 },
    [4] = { 200, 100 },
    [5] = { 300, 100 },
    [6] = { 400, 100 },
    [7] = { 500, 100 },
}

-- Gate: MULTIPLIER_0 > 0 and isPC and not USE_OLD_MAGIC_DAMAGE.
xi.spells.damage.useNewMagicDamageSystem = function(params)
    return (params.multiplier0 or 0) > 0 and
        params.isPC and
        not params.useOldMagicDamage
end

-- Old system statDiff * M ladder with 3*I cap.
xi.spells.damage.oldSystemStatDiffBonus = function(statDiff, spellMultiplier, inflexionPoint)
    spellMultiplier = spellMultiplier or 0
    inflexionPoint = inflexionPoint or 0
    local statCap = 3 * inflexionPoint
    if statDiff > statCap then
        statDiff = statCap
    end

    if statDiff <= 0 then
        return statDiff
    elseif statDiff <= inflexionPoint then
        return math.floor(statDiff * spellMultiplier)
    end

    return math.floor(inflexionPoint * spellMultiplier) +
        math.floor((statDiff - inflexionPoint) * spellMultiplier / 2)
end

-- New system multi-segment PC ladder. multipliers is 7-entry M0..M500 array.
xi.spells.damage.newSystemStatDiffBonus = function(statDiff, multipliers)
    local bonus = 0
    local ladder = xi.spells.damage.newSystemStatLadder
    multipliers = multipliers or {}
    for i = 1, 7 do
        local thr = ladder[i][1]
        local width = ladder[i][2]
        local m = multipliers[i] or 0
        bonus = bonus + math.floor(utils.clamp(statDiff - thr, 0, width) * m)
    end

    return bonus
end

-- mDMG assembly from JP/status/gear/cascade injects.
xi.spells.damage.baseSpellDamageBonusFromParams = function(params)
    local bonus = 0
    if params.isPC then
        if params.hasManafont then
            bonus = bonus + (params.manafontJP or 0) * 3
        end

        if params.hasManawell then
            bonus = bonus + (params.manawellJP or 0)
        end

        if params.isBLMMain then
            bonus = bonus + (params.magicDmgBonusJP or 0)
        end

        if params.skillType == xi.skill.NINJUTSU then
            bonus = bonus + (params.elemNinjutsuJP or 0) * 2
        end

        if
            (params.spellGroup == xi.magic.spellGroup.WHITE and params.hasRapture) or
            (params.spellGroup == xi.magic.spellGroup.BLACK and params.hasEbullience)
        then
            bonus = bonus + (params.strategemEffectIIIJP or 0) * 2
        end
    end

    bonus = bonus + (params.magicDamageMod or 0)

    if
        params.skillType == xi.skill.ELEMENTAL_MAGIC and
        params.hasCascade
    then
        bonus = bonus + math.floor((params.currentTP or 0) / 10)
    end

    return bonus
end

-- Pure calculateBaseDamage once table columns, statDiff, and mDMG injects known.
-- params fields mirror Go spellbasedmg.Params + table columns as scalars.
xi.spells.damage.calculateBaseDamageFromParams = function(params)
    local useNew = xi.spells.damage.useNewMagicDamageSystem(params)
    local baseSpellDamage = params.npcPower or 0
    if useNew then
        baseSpellDamage = params.pcPower or 0
    end

    local statDiff = params.statDiff or 0
    local statDiffBonus = 0
    if useNew then
        local multipliers =
        {
            params.multiplier0 or 0,
            params.multiplier50 or 0,
            params.multiplier100 or 0,
            params.multiplier200 or 0,
            params.multiplier300 or 0,
            params.multiplier400 or 0,
            params.multiplier500 or 0,
        }
        -- Prefer packed multipliers array when provided (1-indexed M0..M500).
        if params.multipliers then
            multipliers = params.multipliers
        end

        statDiffBonus = xi.spells.damage.newSystemStatDiffBonus(statDiff, multipliers)
    else
        statDiffBonus = xi.spells.damage.oldSystemStatDiffBonus(
            statDiff, params.npcMultiplier, params.inflexionPoint)
    end

    local mdmg = xi.spells.damage.baseSpellDamageBonusFromParams(params)
    local spellDamage = baseSpellDamage + mdmg + statDiffBonus
    local spellId = params.spellId or 0

    if spellId == xi.magic.spell.DEATH then
        spellDamage = baseSpellDamage + (params.casterMP or 0) * 3
    elseif xi.spells.damage.isHelixSpell(spellId) then
        spellDamage = spellDamage + (params.helixEffectMod or 0)
    elseif spellId == xi.magic.spell.KAUSTRA then
        baseSpellDamage = math.floor((params.mainLvl or 0) * 0.67) / 10
        statDiffBonus = math.floor(statDiffBonus)
        spellDamage = math.floor(baseSpellDamage * (mdmg + statDiffBonus))
    end

    return utils.clamp(spellDamage, xi.spells.damage.baseDamageMin, xi.spells.damage.baseDamageMax)
end

-----------------------------------
-- Basic Functions (entity hosts)
-----------------------------------
xi.spells.damage.calculateBaseDamage = function(caster, target, spellId, spellGroup, skillType, statUsed)
    local row = pTable[spellId]
    if not row then
        return 0
    end

    local isPC = caster:isPC()
    local inject =
    {
        spellId            = spellId,
        spellGroup         = spellGroup,
        skillType          = skillType,
        statDiff           = caster:getStat(statUsed) - target:getStat(statUsed),
        isPC               = isPC,
        useOldMagicDamage  = xi.settings.main.USE_OLD_MAGIC_DAMAGE,
        npcPower           = row[column.NPC_POWER],
        npcMultiplier      = row[column.NPC_MULTIPLIER],
        pcPower            = row[column.PC_POWER],
        inflexionPoint     = row[column.INFLEXION_POINT],
        multiplier0        = row[column.MULTIPLIER_0],
        multiplier50       = row[column.MULTIPLIER_50],
        multiplier100      = row[column.MULTIPLIER_100],
        multiplier200      = row[column.MULTIPLIER_200],
        multiplier300      = row[column.MULTIPLIER_300],
        multiplier400      = row[column.MULTIPLIER_400],
        multiplier500      = row[column.MULTIPLIER_500],
        magicDamageMod     = caster:getMod(xi.mod.MAGIC_DAMAGE),
        hasCascade         = caster:hasStatusEffect(xi.effect.CASCADE),
        currentTP          = caster:getTP(),
        helixEffectMod     = caster:getMod(xi.mod.HELIX_EFFECT),
        casterMP           = caster:getMP(),
        mainLvl            = caster:getMainLvl(),
    }

    if isPC then
        inject.hasManafont          = caster:hasStatusEffect(xi.effect.MANAFONT)
        inject.manafontJP           = caster:getJobPointLevel(xi.jp.MANAFONT_EFFECT)
        inject.hasManawell          = caster:hasStatusEffect(xi.effect.MANAWELL)
        inject.manawellJP           = caster:getJobPointLevel(xi.jp.MANAWELL_EFFECT)
        inject.isBLMMain            = caster:getMainJob() == xi.job.BLM
        inject.magicDmgBonusJP      = caster:getJobPointLevel(xi.jp.MAGIC_DMG_BONUS)
        inject.elemNinjutsuJP       = caster:getJobPointLevel(xi.jp.ELEM_NINJITSU_EFFECT)
        inject.hasRapture           = caster:hasStatusEffect(xi.effect.RAPTURE)
        inject.hasEbullience        = caster:hasStatusEffect(xi.effect.EBULLIENCE)
        inject.strategemEffectIIIJP = caster:getJobPointLevel(xi.jp.STRATEGEM_EFFECT_III)
    end

    return xi.spells.damage.calculateBaseDamageFromParams(inject)
end

-----------------------------------
-- Spell damage product mult pure helpers
-- Dual-wired to OmegaXI internal/spelldmgmult (slice 6711 / 0862 / 6090).
-----------------------------------

xi.spells.damage.spellDmgDefaultMult        = 1
xi.spells.damage.divineSealMult             = 2
xi.spells.damage.ebullienceBase             = 1.2
xi.spells.damage.undeadDivinePenalty        = 1.5
xi.spells.damage.mtdrMin                    = 0.4
xi.spells.damage.mtdrMax                    = 1.0
xi.spells.damage.aoeResistMin               = 0
xi.spells.damage.aoeResistMax               = 2
xi.spells.damage.additionalResistTierRank   = 4
xi.spells.damage.additionalResistTierHalf   = 0.5

-- Pure MTDR once PC gate and target count inject.
xi.spells.damage.calculateMTDRFromParams = function(params)
    if not params.isPC then
        return 1
    end

    local targetAmount = params.targetAmount or 1
    if targetAmount == 1 then
        return 1
    end

    return utils.clamp(0.9 - 0.05 * targetAmount, xi.spells.damage.mtdrMin, xi.spells.damage.mtdrMax)
end

xi.spells.damage.calculateElementalStaffBonusFromParams = function(params)
    if (params.spellElement or 0) == xi.element.NONE then
        return 1
    end

    return 1 + (params.staffMod or 0) * 5 / 100
end

xi.spells.damage.calculateElementalAffinityBonusFromParams = function(params)
    if (params.spellElement or 0) == xi.element.NONE then
        return 1
    end

    return 1 + (params.elementalMABMod or 0) / 100
end

xi.spells.damage.calculateAdditionalResistTierFromParams = function(params)
    if params.hasSubtleSorcery then
        return 1
    end

    if (params.resistanceRank or 0) < xi.spells.damage.additionalResistTierRank then
        return 1
    end

    return xi.spells.damage.additionalResistTierHalf
end

-- Calculate: Multiple Target Damage Reduction (MTDR)
xi.spells.damage.calculateMTDR = function(caster, spell)
    return xi.spells.damage.calculateMTDRFromParams({
        isPC          = caster:isPC(),
        targetAmount  = spell:getTotalTargets(),
    })
end

-- Bonus elemental damage from Elemental Staves.
xi.spells.damage.calculateElementalStaffBonus = function(caster, spellElement)
    local staffMod = 0
    if spellElement ~= xi.element.NONE then
        staffMod = caster:getMod(xi.data.element.getElementalStaffModifier(spellElement))
    end

    return xi.spells.damage.calculateElementalStaffBonusFromParams({
        spellElement = spellElement,
        staffMod     = staffMod,
    })
end

-- Elemental "Magic Attack Bonus" from Magian trials staves, Atmas, etc...
xi.spells.damage.calculateElementalAffinityBonus = function(caster, spellElement)
    local mabMod = 0
    if spellElement ~= xi.element.NONE then
        mabMod = caster:getMod(xi.data.element.getElementalMABModifier(spellElement))
    end

    return xi.spells.damage.calculateElementalAffinityBonusFromParams({
        spellElement    = spellElement,
        elementalMABMod = mabMod,
    })
end

xi.spells.damage.calculateAdditionalResistTier = function(caster, target, spellElement)
    local rank = 0
    if spellElement and spellElement > xi.element.NONE then
        rank = target:getMod(xi.data.element.getElementalResistanceRankModifier(spellElement))
    end

    return xi.spells.damage.calculateAdditionalResistTierFromParams({
        hasSubtleSorcery = caster:hasStatusEffect(xi.effect.SUBTLE_SORCERY),
        resistanceRank   = rank,
    })
end

-----------------------------------
-- Day/weather damage mult pure helpers
-- Dual-wired to OmegaXI internal/dayweather (slice 6710 / 0855).
-----------------------------------

xi.spells.damage.dayWeatherMultMin          = 0
xi.spells.damage.dayWeatherMultMax          = 1.4
xi.spells.damage.dayWeatherSingleStep       = 0.10
xi.spells.damage.dayWeatherDoubleStep       = 0.25
xi.spells.damage.dayWeatherDayStep          = 0.10
xi.spells.damage.dayWeatherIridescenceStep  = 0.05
xi.spells.damage.dayWeatherProcChance       = 33

-- Pure calculateDayAndWeather once weather/day/proc/mod injects are known.
-- params: spellElement, weather, dayElement, alwaysApply, randomProc,
--   forceDWBonusPenalty, forceElementBonus, iridescence, dayWeatherProcBonus, dayNukeBonus
xi.spells.damage.calculateDayAndWeatherFromParams = function(params)
    local dayAndWeather = 1
    local spellElement = params.spellElement or 0

    if spellElement <= xi.element.NONE then
        return dayAndWeather
    end

    local applyBonuses = false
    local applyPenalties = false

    if params.alwaysApply or params.randomProc or params.forceDWBonusPenalty then
        applyBonuses = true
        applyPenalties = true
    elseif params.forceElementBonus then
        applyBonuses = true
    end

    local weather = params.weather or 0
    local dayElement = params.dayElement or 0
    local iri = (params.iridescence or 0) * xi.spells.damage.dayWeatherIridescenceStep
    local singleWeather = params.associatedSingleWeather
    local doubleWeather = params.associatedDoubleWeather
    local oppSingle = params.oppositeSingleWeather
    local oppDouble = params.oppositeDoubleWeather
    local weakness = params.elementWeakness

    if singleWeather == nil then
        singleWeather = xi.data.element.getAssociatedSingleWeather(spellElement)
    end

    if doubleWeather == nil then
        doubleWeather = xi.data.element.getAssociatedDoubleWeather(spellElement)
    end

    if oppSingle == nil then
        oppSingle = xi.data.element.getOppositeSingleWeather(spellElement)
    end

    if oppDouble == nil then
        oppDouble = xi.data.element.getOppositeDoubleWeather(spellElement)
    end

    if weakness == nil then
        weakness = xi.data.element.getElementWeakness(spellElement)
    end

    if applyBonuses then
        if weather == singleWeather then
            dayAndWeather = dayAndWeather + xi.spells.damage.dayWeatherSingleStep + iri
        elseif weather == doubleWeather then
            dayAndWeather = dayAndWeather + xi.spells.damage.dayWeatherDoubleStep + iri
        end

        if dayElement == spellElement then
            dayAndWeather = dayAndWeather + xi.spells.damage.dayWeatherDayStep
        end

        if
            weather == singleWeather or
            weather == doubleWeather or
            dayElement == spellElement
        then
            dayAndWeather = dayAndWeather + (params.dayWeatherProcBonus or 0) / 100
        end
    end

    if applyPenalties then
        if weather == oppSingle then
            dayAndWeather = dayAndWeather - xi.spells.damage.dayWeatherSingleStep - iri
        elseif weather == oppDouble then
            dayAndWeather = dayAndWeather - xi.spells.damage.dayWeatherDoubleStep - iri
        end

        if dayElement == weakness then
            dayAndWeather = dayAndWeather - xi.spells.damage.dayWeatherDayStep
        end
    end

    if
        spellElement <= xi.element.WATER and
        spellElement == dayElement
    then
        dayAndWeather = dayAndWeather + (params.dayNukeBonus or 0) / 100
    end

    return utils.clamp(dayAndWeather, xi.spells.damage.dayWeatherMultMin, xi.spells.damage.dayWeatherMultMax)
end

-- Entity host for day/weather damage multiplier.
xi.spells.damage.calculateDayAndWeather = function(caster, spellElement, alwaysApply)
    local forceElementBonus = false
    if (spellElement or 0) > xi.element.NONE then
        forceElementBonus = caster:getMod(xi.data.element.getForcedDayOrWeatherBonusModifier(spellElement)) >= 1
    end

    return xi.spells.damage.calculateDayAndWeatherFromParams({
        spellElement              = spellElement,
        weather                   = caster:getWeather(),
        dayElement                = VanadielDayElement(),
        alwaysApply               = alwaysApply,
        randomProc                = math.random(1, 100) <= xi.spells.damage.dayWeatherProcChance,
        forceDWBonusPenalty       = caster:getMod(xi.mod.FORCE_DW_BONUS_PENALTY) >= 1,
        forceElementBonus         = forceElementBonus,
        iridescence               = caster:getMod(xi.mod.IRIDESCENCE),
        dayWeatherProcBonus       = caster:getMod(xi.mod.DAY_WEATHER_PROC_BONUS),
        dayNukeBonus              = caster:getMod(xi.mod.DAY_NUKE_BONUS),
        associatedSingleWeather   = (spellElement or 0) > 0 and xi.data.element.getAssociatedSingleWeather(spellElement) or 0,
        associatedDoubleWeather   = (spellElement or 0) > 0 and xi.data.element.getAssociatedDoubleWeather(spellElement) or 0,
        oppositeSingleWeather     = (spellElement or 0) > 0 and xi.data.element.getOppositeSingleWeather(spellElement) or 0,
        oppositeDoubleWeather     = (spellElement or 0) > 0 and xi.data.element.getOppositeDoubleWeather(spellElement) or 0,
        elementWeakness           = (spellElement or 0) > 0 and xi.data.element.getElementWeakness(spellElement) or 0,
    })
end

-----------------------------------
-- MAB/MDB ratio pure helpers
-- Dual-wired to OmegaXI internal/mabdiff (slice 6707 / 0859).
-----------------------------------

xi.spells.damage.mabCritAddMin         = 10
xi.spells.damage.mabCritAddMax         = 40
xi.spells.damage.theurgicFocusBase     = 50
xi.spells.damage.theurgicFocusJPScale  = 3
xi.spells.damage.mabRatioMin           = 0
xi.spells.damage.mabRatioMax           = 10

xi.spells.damage.isAncientMagic = function(spellId)
    return spellId >= xi.magic.spell.FLARE and spellId <= xi.magic.spell.FLOOD_II
end

xi.spells.damage.isTheurgicRa = function(spellId)
    return (spellId >= xi.magic.spell.FIRA and spellId <= xi.magic.spell.WATERA_II) or
        (spellId >= xi.magic.spell.FIRA_III and spellId <= xi.magic.spell.WATERA_III)
end

-- clamp(10 + MAGIC_CRIT_DMG_INCREASE, 10, 40)
xi.spells.damage.mabCritAdd = function(magicCritDmgIncrease)
    return utils.clamp(10 + (magicCritDmgIncrease or 0), xi.spells.damage.mabCritAddMin, xi.spells.damage.mabCritAddMax)
end

-- Category 1 elemental ninjutsu merit amount once spell-band merit injects known.
xi.spells.damage.elementalNinMeritFromParams = function(params)
    local spellId = params.spellId or 0
    if spellId >= xi.magic.spell.KATON_ICHI and spellId <= xi.magic.spell.KATON_SAN then
        return params.katonEffectMerit or 0
    elseif spellId >= xi.magic.spell.HYOTON_ICHI and spellId <= xi.magic.spell.HYOTON_SAN then
        return params.hyotonEffectMerit or 0
    elseif spellId >= xi.magic.spell.HUTON_ICHI and spellId <= xi.magic.spell.HUTON_SAN then
        return params.hutonEffectMerit or 0
    elseif spellId >= xi.magic.spell.DOTON_ICHI and spellId <= xi.magic.spell.DOTON_SAN then
        return params.dotonEffectMerit or 0
    elseif spellId >= xi.magic.spell.RAITON_ICHI and spellId <= xi.magic.spell.RAITON_SAN then
        return params.raitonEffectMerit or 0
    elseif spellId >= xi.magic.spell.SUITON_ICHI and spellId <= xi.magic.spell.SUITON_SAN then
        return params.suitonEffectMerit or 0
    end

    return 0
end

-- Pure calculateMagicBonusDiff once entity halves and crit roll are injected.
-- params: spellId, skillType, spellElement, baseMAB, mabCritProc, magicCritDmgIncrease,
--   ninMagicBonusMerit, katon..suitonEffectMerit, ninNukeBonusGear,
--   elementalPotencyMerit, barspellSubPower, mainJob, rdmJobPointMAB, geoJobPointMAB,
--   ancientMagicAtkBonusMerit, hasTheurgicFocus, theurgicFocusJP,
--   autoMABCoefficient, targetMDEF
xi.spells.damage.calculateMagicBonusDiffFromParams = function(params)
    local mab = params.baseMAB or 0
    local skillType = params.skillType or 0
    local spellId = params.spellId or 0
    local spellElement = params.spellElement or 0

    if skillType == xi.skill.NINJUTSU then
        mab = mab + (params.ninMagicBonusMerit or 0)
        mab = mab + xi.spells.damage.elementalNinMeritFromParams(params)
        mab = mab + (params.ninNukeBonusGear or 0)
    end

    if params.mabCritProc then
        mab = mab + xi.spells.damage.mabCritAdd(params.magicCritDmgIncrease)
    end

    local mDefBarBonus = 0
    if spellElement >= xi.element.FIRE and spellElement <= xi.element.WATER then
        mab = mab + (params.elementalPotencyMerit or 0)
        mDefBarBonus = params.barspellSubPower or 0
    end

    local mainJob = params.mainJob or 0
    if mainJob == xi.job.RDM then
        mab = mab + (params.rdmJobPointMAB or 0)
    elseif mainJob == xi.job.GEO then
        mab = mab + (params.geoJobPointMAB or 0)
    end

    if xi.spells.damage.isAncientMagic(spellId) then
        mab = mab + (params.ancientMagicAtkBonusMerit or 0)
    end

    if params.hasTheurgicFocus and xi.spells.damage.isTheurgicRa(spellId) then
        mab = mab + xi.spells.damage.theurgicFocusBase +
            (params.theurgicFocusJP or 0) * xi.spells.damage.theurgicFocusJPScale
    end

    local finalCasterMAB = (100 + mab) * (1 + (params.autoMABCoefficient or 0) / 100)
    local finalTargetMDB = 100 + (params.targetMDEF or 0) + mDefBarBonus

    -- Match Go zero-MDB guard (retail MDB is always positive).
    if finalTargetMDB == 0 then
        if finalCasterMAB > 0 then
            return xi.spells.damage.mabRatioMax
        elseif finalCasterMAB < 0 then
            return xi.spells.damage.mabRatioMin
        end

        return 0
    end

    return utils.clamp(finalCasterMAB / finalTargetMDB, xi.spells.damage.mabRatioMin, xi.spells.damage.mabRatioMax)
end

-- Magic Attack Bonus VS Magic Defense Bonus (entity host)
xi.spells.damage.calculateMagicBonusDiff = function(caster, target, spellId, skillType, spellElement, bonusMATT)
    local baseMAB = caster:getMod(xi.mod.MATT) +
        cardinalChantBonus(caster, target, xi.direction.EAST, spellId, skillType) +
        (bonusMATT or 0)
    local mabCritChance = caster:getMod(xi.mod.MAGIC_CRITHITRATE) +
        cardinalChantBonus(caster, target, xi.direction.NORTH, spellId, skillType)

    local elementalPotency = 0
    local barspellSubPower = 0
    if
        spellElement >= xi.element.FIRE and
        spellElement <= xi.element.WATER
    then
        elementalPotency = caster:getMerit(xi.data.element.getElementalPotencyMerit(spellElement))
        local barEffectId = xi.data.element.getAssociatedBarspellEffect(spellElement)
        if target:hasStatusEffect(barEffectId) then
            barspellSubPower = target:getStatusEffect(barEffectId):getSubPower()
        end
    end

    return xi.spells.damage.calculateMagicBonusDiffFromParams({
        spellId                   = spellId,
        skillType                 = skillType,
        spellElement              = spellElement,
        baseMAB                   = baseMAB,
        mabCritProc               = math.random(1, 100) <= mabCritChance,
        magicCritDmgIncrease      = caster:getMod(xi.mod.MAGIC_CRIT_DMG_INCREASE),
        ninMagicBonusMerit        = caster:getMerit(xi.merit.NIN_MAGIC_BONUS),
        katonEffectMerit          = caster:getMerit(xi.merit.KATON_EFFECT),
        hyotonEffectMerit         = caster:getMerit(xi.merit.HYOTON_EFFECT),
        hutonEffectMerit          = caster:getMerit(xi.merit.HUTON_EFFECT),
        dotonEffectMerit          = caster:getMerit(xi.merit.DOTON_EFFECT),
        raitonEffectMerit         = caster:getMerit(xi.merit.RAITON_EFFECT),
        suitonEffectMerit         = caster:getMerit(xi.merit.SUITON_EFFECT),
        ninNukeBonusGear          = caster:getMod(xi.mod.NIN_NUKE_BONUS_GEAR),
        elementalPotencyMerit     = elementalPotency,
        barspellSubPower          = barspellSubPower,
        mainJob                   = caster:getMainJob(),
        rdmJobPointMAB            = caster:getJobPointLevel(xi.jp.RDM_MAGIC_ATK_BONUS),
        geoJobPointMAB            = caster:getJobPointLevel(xi.jp.GEO_MAGIC_ATK_BONUS),
        ancientMagicAtkBonusMerit = caster:getMerit(xi.merit.ANCIENT_MAGIC_ATK_BONUS),
        hasTheurgicFocus          = caster:hasStatusEffect(xi.effect.THEURGIC_FOCUS),
        theurgicFocusJP           = caster:getJobPointLevel(xi.jp.THEURGIC_FOCUS_EFFECT),
        autoMABCoefficient        = caster:getMod(xi.mod.AUTO_MAB_COEFFICIENT),
        targetMDEF                = target:getMod(xi.mod.MDEF),
    })
end

-----------------------------------
-- Magic Critical Hit II pure helpers
-- Dual-wired to OmegaXI internal/magicbonus (slice 6714 / 0858).
-- https://www.bg-wiki.com/ffxi/Magic_Critical_Hit
-- https://www.bg-wiki.com/ffxi/Sroda_Tathlum
-----------------------------------

xi.spells.damage.magicCriticalIIMultiplier = 1.25

-- Pure Magic Crit II: roll in 1..100 and roll <= chance → 1.25, else 1.
-- params: critChanceII, critIIRoll
xi.spells.damage.calculateMagicCriticalMultiplierFromParams = function(params)
    local chance = params.critChanceII or 0
    local roll = params.critIIRoll or 0
    if roll >= 1 and roll <= 100 and roll <= chance then
        return xi.spells.damage.magicCriticalIIMultiplier
    end

    return 1
end

xi.spells.damage.calculateMagicCriticalMultiplier = function(caster)
    return xi.spells.damage.calculateMagicCriticalMultiplierFromParams({
        critChanceII = caster:getMod(xi.mod.MAGIC_CRITHITRATE_II),
        critIIRoll   = math.random(1, 100),
    })
end

-- Pure mid/late product multipliers (consume flags for status del hosts).
-- Returns multiplier, consume (when true host should del the matching status).

xi.spells.damage.calculateDivineSealMultiplierFromParams = function(params)
    if not params.hasDivineSeal then
        return 1, false
    end

    if not params.targetIsUndead then
        return 1, false
    end

    if params.skillType ~= xi.skill.HEALING_MAGIC then
        return 1, false
    end

    return xi.spells.damage.divineSealMult, true
end

xi.spells.damage.calculateDivineEmblemMultiplierFromParams = function(params)
    if not params.hasDivineEmblem then
        return 1, false
    end

    if params.skillType ~= xi.skill.DIVINE_MAGIC then
        return 1, false
    end

    return 1 + (params.divineSkill or 0) / 100, true
end

xi.spells.damage.calculateEnhancedElementalSealMultiplierFromParams = function(params)
    if not params.hasElementalSeal then
        return 1
    end

    if params.skillType ~= xi.skill.ELEMENTAL_MAGIC then
        return 1
    end

    if (params.spellElement or 0) <= xi.element.NONE then
        return 1
    end

    return 1 + (params.enhancesElementalSealMod or 0) / 100
end

xi.spells.damage.calculateEbullienceMultiplierFromParams = function(params)
    if not params.hasEbullience then
        return 1, false
    end

    if params.spellGroup ~= xi.magic.spellGroup.BLACK then
        return 1, false
    end

    return xi.spells.damage.ebullienceBase + (params.ebullienceAmountMod or 0) / 100, true
end

xi.spells.damage.calculateSkillTypeMultiplierFromParams = function(params)
    local skillType = params.skillType or 0
    if skillType == xi.skill.ELEMENTAL_MAGIC then
        return params.elementalPower or 1
    elseif skillType == xi.skill.DARK_MAGIC then
        return params.darkPower or 1
    elseif skillType == xi.skill.NINJUTSU then
        return params.ninjutsuPower or 1
    elseif skillType == xi.skill.DIVINE_MAGIC then
        return params.divinePower or 1
    end

    return 1
end

-- Divine seal applies its own multiplier to healing spells when used against undead.
xi.spells.damage.calculateDivineSealMultiplier = function(caster, target, skillType)
    local mult, consume = xi.spells.damage.calculateDivineSealMultiplierFromParams({
        hasDivineSeal  = caster:hasStatusEffect(xi.effect.DIVINE_SEAL),
        targetIsUndead = target:isUndead(),
        skillType      = skillType,
    })
    if consume then
        caster:delStatusEffect(xi.effect.DIVINE_SEAL)
    end

    return mult
end

-- Divine Emblem applies its own damage multiplier to divine spells.
xi.spells.damage.calculateDivineEmblemMultiplier = function(caster, skillType)
    local mult, consume = xi.spells.damage.calculateDivineEmblemMultiplierFromParams({
        hasDivineEmblem = caster:hasStatusEffect(xi.effect.DIVINE_EMBLEM),
        skillType       = skillType,
        divineSkill     = caster:getSkillLevel(xi.skill.DIVINE_MAGIC),
    })
    if consume then
        caster:delStatusEffect(xi.effect.DIVINE_EMBLEM)
    end

    return mult
end

-- Elemental seal enhance mult (no consume in LSB).
xi.spells.damage.calculateEnhancedElementalSealMultiplier = function(caster, skillType, spellElement)
    return xi.spells.damage.calculateEnhancedElementalSealMultiplierFromParams({
        hasElementalSeal         = caster:hasStatusEffect(xi.effect.ELEMENTAL_SEAL),
        skillType                = skillType,
        spellElement             = spellElement,
        enhancesElementalSealMod = caster:getMod(xi.mod.ENHANCES_ELEMENTAL_SEAL),
    })
end

-- Ebullience applies an entirely separate multiplier to Black Magic.
xi.spells.damage.calculateEbullienceMultiplier = function(caster, spellGroup)
    local mult, consume = xi.spells.damage.calculateEbullienceMultiplierFromParams({
        hasEbullience       = caster:hasStatusEffect(xi.effect.EBULLIENCE),
        spellGroup          = spellGroup,
        ebullienceAmountMod = caster:getMod(xi.mod.EBULLIENCE_AMOUNT),
    })
    if consume then
        caster:delStatusEffectSilent(xi.effect.EBULLIENCE)
    end

    return mult
end

-- CUSTOM function supported in settings.
xi.spells.damage.calculateSkillTypeMultiplier = function(skillType)
    return xi.spells.damage.calculateSkillTypeMultiplierFromParams({
        skillType      = skillType,
        elementalPower = xi.settings.main.ELEMENTAL_POWER,
        darkPower      = xi.settings.main.DARK_POWER,
        ninjutsuPower  = xi.settings.main.NINJUTSU_POWER,
        divinePower    = xi.settings.main.DIVINE_POWER,
    })
end

-----------------------------------
-- Ninjutsu damage mult pure helpers
-- Dual-wired to OmegaXI internal/ninjutsudmg (slice 6712 / 0856).
-----------------------------------

xi.spells.damage.ninSkillCaps =
{
    -- Tier = { Min skill, Max skill }
    [1] = {  50, 250 }, -- Ichi
    [2] = { 125, 350 }, -- Ni
    [3] = { 275, 500 }, -- San
}

xi.spells.damage.ninFutaeBase = 1.5

-- Ichi/Ni/San tier from elemental nuke spell ID (LSB modulo rule).
xi.spells.damage.ninSpellTier = function(spellId)
    spellId = spellId or 0
    if spellId % 3 == 2 then
        return 1 -- Ichi (e.g. 320, 323, …)
    elseif spellId % 3 == 0 then
        return 2 -- Ni
    end

    return 3 -- San
end

-- Pure calculateNinSkillBonus once job/skill/spell/skill-level inject.
xi.spells.damage.calculateNinSkillBonusFromParams = function(params)
    if (params.mainJob or 0) ~= xi.job.NIN then
        return 1
    end

    if (params.skillType or 0) ~= xi.skill.NINJUTSU then
        return 1
    end

    local tier = xi.spells.damage.ninSpellTier(params.spellId)
    local caps = xi.spells.damage.ninSkillCaps[tier]
    local skillLevel = utils.clamp(params.ninjutsuSkill or 0, caps[1], caps[2])

    return 1 + (skillLevel - caps[1]) / 200
end

-- Pure calculateNinFutaeBonus. Returns multiplier, consume.
xi.spells.damage.calculateNinFutaeBonusFromParams = function(params)
    if not params.hasFutae then
        return 1, false
    end

    if (params.skillType or 0) ~= xi.skill.NINJUTSU then
        return 1, false
    end

    local mult = xi.spells.damage.ninFutaeBase +
        (params.enhancesFutaeMod or 0) / 100 +
        (params.futaeJP or 0) / 20

    return mult, true
end

-- Pure calculateNinjutsuMultiplier (Innin behind nuke bonus).
xi.spells.damage.calculateNinjutsuMultiplierFromParams = function(params)
    if not params.hasInnin then
        return 1
    end

    if not params.isBehind then
        return 1
    end

    if (params.skillType or 0) ~= xi.skill.NINJUTSU then
        return 1
    end

    return 1 + (params.ninNukeBonusInnin or 0) / 100
end

xi.spells.damage.calculateNinSkillBonus = function(caster, spellId, skillType)
    return xi.spells.damage.calculateNinSkillBonusFromParams({
        mainJob       = caster:getMainJob(),
        skillType     = skillType,
        spellId       = spellId,
        ninjutsuSkill = caster:getSkillLevel(xi.skill.NINJUTSU),
    })
end

xi.spells.damage.calculateNinFutaeBonus = function(caster, skillType)
    local mult, consume = xi.spells.damage.calculateNinFutaeBonusFromParams({
        hasFutae         = caster:hasStatusEffect(xi.effect.FUTAE),
        skillType        = skillType,
        enhancesFutaeMod = caster:getMod(xi.mod.ENHANCES_FUTAE),
        futaeJP          = caster:getJobPointLevel(xi.jp.FUTAE_EFFECT),
    })
    if consume then
        caster:delStatusEffect(xi.effect.FUTAE)
    end

    return mult
end

xi.spells.damage.calculateNinjutsuMultiplier = function(caster, target, skillType)
    return xi.spells.damage.calculateNinjutsuMultiplierFromParams({
        hasInnin          = caster:hasStatusEffect(xi.effect.INNIN),
        isBehind          = caster:isBehind(target),
        skillType         = skillType,
        ninNukeBonusInnin = caster:getMod(xi.mod.NIN_NUKE_BONUS_INNIN),
    })
end

xi.spells.damage.isHelixSpell = function(spellId)
    spellId = spellId or 0
    return (spellId >= xi.magic.spell.GEOHELIX and spellId <= xi.magic.spell.LUMINOHELIX) or
        (spellId >= xi.magic.spell.GEOHELIX_II and spellId <= xi.magic.spell.LUMINOHELIX_II)
end

xi.spells.damage.calculateUndeadDivinePenaltyFromParams = function(params)
    if not params.targetIsUndead then
        return 1
    end

    if params.skillType ~= xi.skill.DIVINE_MAGIC then
        return 1
    end

    return xi.spells.damage.undeadDivinePenalty
end

xi.spells.damage.calculateHelixMeritMultiplierFromParams = function(params)
    if not xi.spells.damage.isHelixSpell(params.spellId) then
        return 1
    end

    return 1 + 2 * (params.helixMagicAccAttMerit or 0) / 100
end

xi.spells.damage.calculateAreaOfEffectResistanceFromParams = function(params)
    if params.isPrimaryTarget then
        return 1
    end

    return utils.clamp(1 + (params.dmgAoEMod or 0) / 10000,
        xi.spells.damage.aoeResistMin, xi.spells.damage.aoeResistMax)
end

xi.spells.damage.calculateSpellActionTypeMultiplierFromParams = function(params)
    return 1 + (params.powerMultiplierSpellMod or 0) / 100
end

xi.spells.damage.calculateUndeadDivinePenalty = function(target, skillType)
    return xi.spells.damage.calculateUndeadDivinePenaltyFromParams({
        targetIsUndead = target:isUndead(),
        skillType      = skillType,
    })
end

xi.spells.damage.calculateHelixMeritMultiplier = function(caster, spellId)
    return xi.spells.damage.calculateHelixMeritMultiplierFromParams({
        spellId               = spellId,
        helixMagicAccAttMerit = caster:getMerit(xi.merit.HELIX_MAGIC_ACC_ATT),
    })
end

xi.spells.damage.calculateAreaOfEffectResistance = function(target, spell)
    return xi.spells.damage.calculateAreaOfEffectResistanceFromParams({
        isPrimaryTarget = target:getID() == spell:getPrimaryTargetID(),
        dmgAoEMod       = target:getMod(xi.mod.DMG_AOE),
    })
end

xi.spells.damage.calculateSpellActionTypeMultiplier = function(caster)
    return xi.spells.damage.calculateSpellActionTypeMultiplierFromParams({
        powerMultiplierSpellMod = caster:getMod(xi.mod.POWER_MULTIPLIER_SPELL),
    })
end

-----------------------------------
-- Absorb / nullification pure helpers
-- Dual-wired to OmegaXI internal/absorbnull (slice 6709 / 0864).
-----------------------------------

-- Lua chance compare: math.random(1, 100) <= chance
xi.spells.damage.absorbNullChanceLua = function(roll, chance)
    return (roll or 0) <= (chance or 0)
end

-- Pure calculateAbsorption once Liement factor and proc flags are injected.
-- params: liementFactor, element, isMagic, absorbAllProc, absorbMagicProc, absorbElementProc
xi.spells.damage.calculateAbsorptionFromParams = function(params)
    local liement = params.liementFactor
    if liement == nil then
        liement = 1
    end

    if liement < 0 then
        return liement
    end

    if params.absorbAllProc then
        return -1
    end

    if params.isMagic and params.absorbMagicProc then
        return -1
    end

    if (params.element or 0) > 0 and params.absorbElementProc then
        return -1
    end

    return 1
end

-- Pure calculateNullification once proc flags are injected.
-- params: element, isMagic, isBreath, nullAllProc, nullMagicProc, nullBreathProc, nullElementProc
xi.spells.damage.calculateNullificationFromParams = function(params)
    if params.nullAllProc then
        return 0
    end

    if params.isMagic and params.nullMagicProc then
        return 0
    end

    if params.isBreath and params.nullBreathProc then
        return 0
    end

    if (params.element or 0) > 0 and params.nullElementProc then
        return 0
    end

    return 1
end

-- Entity host: absorption by Liement then chance ladder (early-exit RNG order).
xi.spells.damage.calculateAbsorption = function(target, element, isMagic)
    local liementFactor = target:checkLiementAbsorb(xi.damageType.ELEMENTAL + element)
    if liementFactor < 0 then
        return xi.spells.damage.calculateAbsorptionFromParams({
            liementFactor = liementFactor,
        })
    end

    if math.random(1, 100) <= target:getMod(xi.mod.ABSORB_DMG_CHANCE) then
        return xi.spells.damage.calculateAbsorptionFromParams({
            liementFactor = 1,
            absorbAllProc = true,
        })
    end

    if isMagic and math.random(1, 100) <= target:getMod(xi.mod.MAGIC_ABSORB) then
        return xi.spells.damage.calculateAbsorptionFromParams({
            liementFactor   = 1,
            isMagic         = true,
            absorbMagicProc = true,
        })
    end

    if
        (element or 0) > 0 and
        math.random(1, 100) <= target:getMod(xi.data.element.getElementalAbsorptionModifier(element))
    then
        return xi.spells.damage.calculateAbsorptionFromParams({
            liementFactor     = 1,
            element           = element,
            absorbElementProc = true,
        })
    end

    return xi.spells.damage.calculateAbsorptionFromParams({
        liementFactor = 1,
        element       = element,
        isMagic       = isMagic,
    })
end

-- Entity host: nullification chance ladder (early-exit RNG order).
xi.spells.damage.calculateNullification = function(target, element, isMagic, isBreath)
    if math.random(1, 100) <= target:getMod(xi.mod.NULL_DAMAGE) then
        return xi.spells.damage.calculateNullificationFromParams({
            nullAllProc = true,
        })
    end

    if isMagic and math.random(1, 100) <= target:getMod(xi.mod.NULL_MAGICAL_DAMAGE) then
        return xi.spells.damage.calculateNullificationFromParams({
            isMagic       = true,
            nullMagicProc = true,
        })
    end

    if isBreath and math.random(1, 100) <= target:getMod(xi.mod.NULL_BREATH_DAMAGE) then
        return xi.spells.damage.calculateNullificationFromParams({
            isBreath       = true,
            nullBreathProc = true,
        })
    end

    if
        (element or 0) > 0 and
        math.random(1, 100) <= target:getMod(xi.data.element.getElementalNullificationModifier(element))
    then
        return xi.spells.damage.calculateNullificationFromParams({
            element         = element,
            nullElementProc = true,
        })
    end

    return xi.spells.damage.calculateNullificationFromParams({
        element  = element,
        isMagic  = isMagic,
        isBreath = isBreath,
    })
end

-----------------------------------
-- Magic burst product mult pure helpers
-- Dual-wired to OmegaXI internal/magicburst (slice 6715 / 0853).
-----------------------------------

xi.spells.damage.magicBurstBase           = 1.25
xi.spells.damage.magicBurstCappedBonusMax = 0.4
xi.spells.damage.magicBurstRankBonusLow   = 1.5  -- resistRank <= -3
xi.spells.damage.magicBurstRankBonusHigh  = 0.0  -- resistRank >= 5
-- Mid table for resistRank -2..4 (Lua 1-based rankTable[resistRank+3]).
xi.spells.damage.magicBurstRankTableMid   = { 1.15, 0.85, 0.6, 0.5, 0.4, 0.15, 0.05 }

xi.spells.damage.magicBurstRankBonus = function(resistRank)
    resistRank = resistRank or 0
    if resistRank <= -3 then
        return xi.spells.damage.magicBurstRankBonusLow
    end

    if resistRank >= 5 then
        return xi.spells.damage.magicBurstRankBonusHigh
    end

    -- resistRank in [-2, 4] → mid table index 1..7
    return xi.spells.damage.magicBurstRankTableMid[resistRank + 3]
end

-- Pure calculateIfMagicBurst. Returns multiplier, consumeSengikori.
-- params: spellElement, skillchainCount, resistRank, sengikoriMod
xi.spells.damage.calculateIfMagicBurstFromParams = function(params)
    local magicBurst = 1
    local spellElement = params.spellElement or 0
    local skillchainCount = params.skillchainCount or 0

    if spellElement > xi.element.NONE then
        magicBurst = xi.spells.damage.magicBurstBase +
            xi.spells.damage.magicBurstRankBonus(params.resistRank) +
            skillchainCount / 10
    end

    local consume = false
    local sengikori = params.sengikoriMod or 0
    if skillchainCount >= 1 and sengikori > 0 then
        magicBurst = magicBurst + sengikori / 100
        consume = true
    end

    return magicBurst, consume
end

-- Pure calculateIfMagicBurstBonus once capped/uncapped/merits/JP/chant inject.
-- params: spellId, cappedBonusMod, uncappedBonusMod, ancientMagicMerit,
--   inninMerit, magicBurstJP, cardinalChantWest (all percent units before /100)
xi.spells.damage.calculateIfMagicBurstBonusFromParams = function(params)
    local capped = (params.cappedBonusMod or 0) / 100
    local uncapped = (params.uncappedBonusMod or 0) / 100
    local spellId = params.spellId or 0

    if
        spellId >= xi.magic.spell.FLARE and
        spellId <= xi.magic.spell.FLOOD_II
    then
        capped = capped + (params.ancientMagicMerit or 0) / 100
    end

    capped = capped + (params.inninMerit or 0) / 100
    capped = utils.clamp(capped, 0, xi.spells.damage.magicBurstCappedBonusMax)

    uncapped = uncapped +
        (params.magicBurstJP or 0) / 100 +
        (params.cardinalChantWest or 0) / 100

    return 1 + capped + uncapped
end

xi.spells.damage.calculateIfMagicBurst = function(target, spellElement, skillchainCount)
    local resistRank = 0
    if (spellElement or 0) > xi.element.NONE then
        resistRank = target:getMod(xi.data.element.getElementalResistanceRankModifier(spellElement))
    end

    local mult, consume = xi.spells.damage.calculateIfMagicBurstFromParams({
        spellElement    = spellElement,
        skillchainCount = skillchainCount,
        resistRank      = resistRank,
        sengikoriMod    = target:getMod(xi.mod.SENGIKORI_MB_DMG_DEBUFF),
    })
    if consume then
        target:setMod(xi.mod.SENGIKORI_MB_DMG_DEBUFF, 0)
    end

    return mult
end

xi.spells.damage.calculateIfMagicBurstBonus = function(caster, target, spellId, skillType, spellElement)
    local inninMerit = 0
    if caster:isBehind(target) and caster:hasStatusEffect(xi.effect.INNIN) then
        inninMerit = caster:getMerit(xi.merit.INNIN_EFFECT)
    end

    local ancientMerit = 0
    if
        spellId >= xi.magic.spell.FLARE and
        spellId <= xi.magic.spell.FLOOD_II
    then
        ancientMerit = caster:getMerit(xi.merit.ANCIENT_MAGIC_BURST_DMG)
    end

    return xi.spells.damage.calculateIfMagicBurstBonusFromParams({
        spellId           = spellId,
        cappedBonusMod    = caster:getMod(xi.mod.MAGIC_BURST_BONUS_CAPPED),
        uncappedBonusMod  = caster:getMod(xi.mod.MAGIC_BURST_BONUS_UNCAPPED),
        ancientMagicMerit = ancientMerit,
        inninMerit        = inninMerit,
        magicBurstJP      = caster:getJobPointLevel(xi.jp.MAGIC_BURST_DMG_BONUS),
        cardinalChantWest = cardinalChantBonus(caster, target, xi.direction.WEST, spellId, skillType),
    })
end

-----------------------------------
-- Nuke Wall pure helpers
-- Dual-wired to OmegaXI internal/nukewall (slice 6708 / 0863).
-----------------------------------

xi.spells.damage.nukeWallPotencyMin       = 0
xi.spells.damage.nukeWallPotencyMax       = 4000
xi.spells.damage.nukeWallPotencyDecay     = 2000
xi.spells.damage.nukeWallDecayRemainingMs = 4000
xi.spells.damage.nukeWallDurationSec      = 5
xi.spells.damage.nukeWallLevelDamageScale = 21
xi.spells.damage.nukeWallLevelDamageBase  = 500

xi.spells.damage.nukeWallDamageCap = function(mainLvl)
    return (mainLvl or 0) * xi.spells.damage.nukeWallLevelDamageScale + xi.spells.damage.nukeWallLevelDamageBase
end

xi.spells.damage.nukeWallApplyTimeDecay = function(potency, timeRemainingMs)
    potency = potency or 0
    if (timeRemainingMs or 0) <= xi.spells.damage.nukeWallDecayRemainingMs then
        return utils.clamp(potency - xi.spells.damage.nukeWallPotencyDecay,
            xi.spells.damage.nukeWallPotencyMin, xi.spells.damage.nukeWallPotencyMax)
    end

    return potency
end

xi.spells.damage.nukeWallRaykeMatchesElement = function(spellElement, raykeSubPower)
    spellElement = spellElement or 0
    raykeSubPower = raykeSubPower or 0
    for i = 0, 16, 4 do
        if bit.band(bit.rshift(raykeSubPower, i), 0xF) == spellElement then
            return true
        end
    end

    return false
end

xi.spells.damage.nukeWallApplyRayke = function(potency, spellElement, hasRayke, raykeSubPower)
    potency = potency or 0
    if not hasRayke then
        return potency
    end

    if xi.spells.damage.nukeWallRaykeMatchesElement(spellElement, raykeSubPower) then
        return math.floor(potency / 2)
    end

    return potency
end

xi.spells.damage.nukeWallFactorFromPotency = function(potency)
    return 1 - (potency or 0) / 10000
end

xi.spells.damage.nukeWallNextPotency = function(prevPotency, finalDamage, mainLvl)
    local cap = xi.spells.damage.nukeWallDamageCap(mainLvl)
    local added = math.floor(xi.spells.damage.nukeWallPotencyMax * (finalDamage or 0) / cap)
    return utils.clamp(added + (prevPotency or 0),
        xi.spells.damage.nukeWallPotencyMin, xi.spells.damage.nukeWallPotencyMax)
end

-- Pure calculateNukeWallFactor inject form.
-- params: isNM, spellElement, finalDamage, hasNukeWall, wallPotency, timeRemainingMs,
--   hasRayke, raykeSubPower, mainLvl
-- returns: factor, nextPotency, applied, consumedWall
xi.spells.damage.calculateNukeWallFactorFromParams = function(params)
    if
        not params.isNM or
        (params.spellElement or 0) <= xi.element.NONE or
        (params.finalDamage or 0) < 0
    then
        return 1, 0, false, false
    end

    local potency = 0
    local consumed = false
    if params.hasNukeWall then
        potency = params.wallPotency or 0
        potency = xi.spells.damage.nukeWallApplyTimeDecay(potency, params.timeRemainingMs)
        potency = xi.spells.damage.nukeWallApplyRayke(
            potency, params.spellElement, params.hasRayke, params.raykeSubPower)
        consumed = true
    end

    local nextPot = xi.spells.damage.nukeWallNextPotency(potency, params.finalDamage, params.mainLvl)
    return xi.spells.damage.nukeWallFactorFromPotency(potency), nextPot, true, consumed
end

-- Consecutive Elemental Damage Penalty ("Nuke Wall") entity host.
local function calculateNukeWallFactor(target, spellElement, finalDamage)
    local hasWall = false
    local wallPotency = 0
    local timeRemainingMs = 0
    local effect = target:getStatusEffect(xi.effect.NUKE_WALL)
    if effect then
        hasWall = true
        wallPotency = effect:getPower()
        timeRemainingMs = effect:getTimeRemaining()
    end

    local hasRayke = target:hasStatusEffect(xi.effect.RAYKE)
    local raykeSubPower = 0
    if hasRayke then
        raykeSubPower = target:getStatusEffect(xi.effect.RAYKE):getSubPower()
    end

    local factor, nextPot, applied, consumed = xi.spells.damage.calculateNukeWallFactorFromParams({
        isNM            = target:isNM(),
        spellElement    = spellElement,
        finalDamage     = finalDamage,
        hasNukeWall     = hasWall,
        wallPotency     = wallPotency,
        timeRemainingMs = timeRemainingMs,
        hasRayke        = hasRayke,
        raykeSubPower   = raykeSubPower,
        mainLvl         = target:getMainLvl(),
    })

    if applied then
        if consumed then
            target:delStatusEffectSilent(xi.effect.NUKE_WALL)
        end

        target:addStatusEffect(xi.effect.NUKE_WALL, {
            power    = nextPot,
            duration = xi.spells.damage.nukeWallDurationSec,
            origin   = target,
            icon     = 0,
            subPower = spellElement,
        })
    end

    return factor
end

-----------------------------------
-- Spell Helper Function
-----------------------------------
xi.spells.damage.useDamageSpell = function(caster, target, spell)
    local finalDamage = 0 -- The variable we want to calculate

    -- Early return: Spell is nullified.
    local spellElement = spell:getElement()
    if xi.spells.damage.calculateNullification(target, spellElement, true, false) == 0 then
        spell:setMsg(xi.msg.basic.MAGIC_RESIST)

        return 0
    end

    -- Calculate absoprtion and magic burst.
    local absorbFactor       = xi.spells.damage.calculateAbsorption(target, spellElement, true)
    local _, skillchainCount = xi.magicburst.formMagicBurst(target, spellElement) -- External function.

    local notAbsorb = absorbFactor > 0
    local canMBurst = absorbFactor > 0 and skillchainCount > 0

    -- Fetch tabled data.
    local spellId         = spell:getID()
    local skillType       = spell:getSkillType()
    local spellGroup      = spell:getSpellGroup()
    local statUsed        = pTable[spellId][column.STAT_USED]
    local bonusMacc       = pTable[spellId][column.BONUS_MACC] + cardinalChantBonus(caster, target, xi.direction.SOUTH, spellId, skillType)
    local forceDayWeather = pTable[spellId][column.FORCE_DAY_WEATHER]

    -- Calculate base damage and the rest of damage multipliers.
    local spellDamage                 = xi.spells.damage.calculateBaseDamage(caster, target, spellId, spellGroup, skillType, statUsed)
    local multipleTargetReduction     = xi.spells.damage.calculateMTDR(caster, spell)
    local elementalStaffBonus         = xi.spells.damage.calculateElementalStaffBonus(caster, spellElement)
    local elementalAffinityBonus      = xi.spells.damage.calculateElementalAffinityBonus(caster, spellElement)
    local resistTier                  = notAbsorb and xi.combat.magicHitRate.calculateResistRate(caster, target, spellGroup, skillType, 0, spellElement, statUsed, 0, bonusMacc) or 1
    local additionalResistTier        = notAbsorb and xi.spells.damage.calculateAdditionalResistTier(caster, target, spellElement) or 1
    local magicBurst                  = canMBurst and xi.spells.damage.calculateIfMagicBurst(target, spellElement, skillchainCount) or 1
    local magicBurstBonus             = canMBurst and xi.spells.damage.calculateIfMagicBurstBonus(caster, target, spellId, skillType, spellElement) or 1
    local dayAndWeather               = xi.spells.damage.calculateDayAndWeather(caster, spellElement, forceDayWeather)
    local magicBonusDiff              = xi.spells.damage.calculateMagicBonusDiff(caster, target, spellId, skillType, spellElement, 0)
    local targetMagicDamageAdjustment = notAbsorb and xi.combat.damage.calculateDamageAdjustment(target, false, true, false, false) or 1
    local sdt                         = xi.combat.damage.magicalElementSDT(target, spellElement)
    local criticalDamageMultiplier    = xi.spells.damage.calculateMagicCriticalMultiplier(caster)
    local divineSealMultiplier        = xi.spells.damage.calculateDivineSealMultiplier(caster, target, skillType)
    local divineEmblemMultiplier      = xi.spells.damage.calculateDivineEmblemMultiplier(caster, skillType)
    local eleSealMultiplier           = xi.spells.damage.calculateEnhancedElementalSealMultiplier(caster, skillType, spellElement)
    local ebullienceMultiplier        = xi.spells.damage.calculateEbullienceMultiplier(caster, spellGroup)
    local skillTypeMultiplier         = xi.spells.damage.calculateSkillTypeMultiplier(skillType)
    local ninSkillBonus               = xi.spells.damage.calculateNinSkillBonus(caster, spellId, skillType)
    local ninFutaeBonus               = xi.spells.damage.calculateNinFutaeBonus(caster, skillType)
    local ninjutsuMultiplier          = xi.spells.damage.calculateNinjutsuMultiplier(caster, target, skillType)
    local undeadDivinePenalty         = xi.spells.damage.calculateUndeadDivinePenalty(target, skillType)
    local scarletDeliriumMultiplier   = xi.combat.damage.scarletDeliriumMultiplier(caster)
    local steamJacketMultiplier       = xi.combat.damage.steamJacketMultiplier(target, spellElement)
    local helixMeritMultiplier        = xi.spells.damage.calculateHelixMeritMultiplier(caster, spellId)
    local areaOfEffectResistance      = xi.spells.damage.calculateAreaOfEffectResistance(target, spell)
    local actionTypeMultiplier        = xi.spells.damage.calculateSpellActionTypeMultiplier(caster)

    -- Calculate finalDamage. It MUST be floored after EACH multiplication.
    finalDamage = math.floor(spellDamage * multipleTargetReduction)
    finalDamage = math.floor(finalDamage * elementalStaffBonus)
    finalDamage = math.floor(finalDamage * elementalAffinityBonus)
    finalDamage = math.floor(finalDamage * resistTier)
    finalDamage = math.floor(finalDamage * additionalResistTier)
    finalDamage = math.floor(finalDamage * magicBurst)
    finalDamage = math.floor(finalDamage * magicBurstBonus)
    finalDamage = math.floor(finalDamage * dayAndWeather)
    finalDamage = math.floor(finalDamage * magicBonusDiff)
    finalDamage = math.floor(finalDamage * targetMagicDamageAdjustment)
    finalDamage = math.floor(finalDamage * sdt)
    finalDamage = math.floor(finalDamage * criticalDamageMultiplier)
    finalDamage = math.floor(finalDamage * divineSealMultiplier)
    finalDamage = math.floor(finalDamage * divineEmblemMultiplier)
    finalDamage = math.floor(finalDamage * eleSealMultiplier)
    finalDamage = math.floor(finalDamage * ebullienceMultiplier)
    finalDamage = math.floor(finalDamage * skillTypeMultiplier)
    finalDamage = math.floor(finalDamage * ninSkillBonus)
    finalDamage = math.floor(finalDamage * ninFutaeBonus)
    finalDamage = math.floor(finalDamage * ninjutsuMultiplier)
    finalDamage = math.floor(finalDamage * undeadDivinePenalty)
    finalDamage = math.floor(finalDamage * scarletDeliriumMultiplier)
    finalDamage = math.floor(finalDamage * steamJacketMultiplier)
    finalDamage = math.floor(finalDamage * helixMeritMultiplier)
    finalDamage = math.floor(finalDamage * areaOfEffectResistance)
    finalDamage = math.floor(finalDamage * actionTypeMultiplier)
    finalDamage = math.floor(finalDamage * absorbFactor)

    -- Handle "Nuke Wall". It must be handled after all previous calculations, but before clamp.
    local nukeWallFactor = notAbsorb and calculateNukeWallFactor(target, spellElement, finalDamage) or 1
    finalDamage          = math.floor(finalDamage * nukeWallFactor)

    -- Handle Magic Absorb message and HP recovery.
    if finalDamage < 0 then
        finalDamage = target:addHP(-finalDamage)
        spell:setMsg(xi.msg.basic.MAGIC_RECOVERS_HP)

        return finalDamage
    end

    -- Handle Phalanx, One for All, Stoneskin.
    finalDamage = utils.clamp(utils.handlePhalanx(target, finalDamage), 0, 99999)
    finalDamage = utils.clamp(utils.handleOneForAll(target, finalDamage), 0, 99999)
    finalDamage = utils.clamp(utils.handleStoneskin(target, finalDamage), 0, 99999)

    -- Handle final adjustments. Most are located in core. TODO: Decide if we want core handling this.
    -- Check if the mob has a damage cap
    finalDamage = target:checkDamageCap(finalDamage)

    -- Handle Bind break and TP?
    target:takeSpellDamage(caster, spell, finalDamage, xi.attackType.MAGICAL, xi.damageType.ELEMENTAL + spellElement)

    -- Handle Afflatus Misery.
    target:handleAfflatusMiseryDamage(finalDamage)

    -- Handle Enmity.
    target:updateEnmityFromDamage(caster, finalDamage)

    -- Add "Magic Burst!" message
    if canMBurst then
        spell:setMsg(xi.msg.basic.MAGIC_BURST_DAMAGE)
        caster:triggerRoeEvent(xi.roeTrigger.MAGIC_BURST)
    end

    return finalDamage
end
