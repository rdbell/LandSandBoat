-----------------------------------
-- Song Utilities
-- Dual-wired pure inject forms (slice 6729 / 0875 / 6115):
--   resolveSingingLevelFromParams, calculateSongPowerFromParams,
--   calculateSongDurationFromParams, resolveSubEffect, paramFourFor,
--   marchPower, useEnhancingSongFromParams
-- Parity: internal/enhancingsong
-----------------------------------
require('scripts/globals/jobpoints')
-----------------------------------
xi = xi or {}
xi.spells = xi.spells or {}
xi.spells.enhancing = xi.spells.enhancing or {}
-----------------------------------
-- File structure:
-- 2 Basic Functions called by the main function.

local column =
{
    EFFECT_TIER       =  1,
    EFFECT_MAIN       =  2,
    EFFECT_SUB        =  3,
    MODIFIER          =  4,
    MERIT_ID          =  5,
    JOB_POINT_ID      =  6,
    POWER_BASE        =  7,
    SKILL_REQUIREMENT =  8,
    POWER_CAP         =  9,
    MULTIPLIER        = 10,
    DIVISOR           = 11,
    SOUL_VOICE        = 12,
}

-- Table variables.
local pTable =
{
--                                          1     2                 3                         4                       5                         6                     7     8    9    10   11  12
-- Structure:                 [spellId] = { Tier, Main Effect,      subEffect,                Main Modifier,          Merit Effect,             Job-Point Effect,     power Sreq Pcap Mult Div SVP },
    -- Ballad
    [xi.magic.spell.MAGES_BALLAD      ] = { 1, xi.effect.BALLAD,    xi.mod.AUGMENT_SONG_STAT, xi.mod.BALLAD_EFFECT,   0,                        0,                    1,   0,   1,   1,  0, true  },
    [xi.magic.spell.MAGES_BALLAD_II   ] = { 2, xi.effect.BALLAD,    xi.mod.AUGMENT_SONG_STAT, xi.mod.BALLAD_EFFECT,   0,                        0,                    2,   0,   2,   1,  0, true  },
    [xi.magic.spell.MAGES_BALLAD_III  ] = { 3, xi.effect.BALLAD,    xi.mod.AUGMENT_SONG_STAT, xi.mod.BALLAD_EFFECT,   0,                        0,                    3,   0,   3,   1,  0, true  },
    -- Carol - NOTE: CAROL II Gives a fixed elemental evasion. However, it also gives a Elemental Nullification effect, that follows regular song rules concerning power.
    [xi.magic.spell.FIRE_CAROL        ] = { 1, xi.effect.CAROL,     xi.element.FIRE,          xi.mod.CAROL_EFFECT,    0,                        0,                   20, 200,  80,   8, 10, true  },
    [xi.magic.spell.ICE_CAROL         ] = { 1, xi.effect.CAROL,     xi.element.ICE,           xi.mod.CAROL_EFFECT,    0,                        0,                   20, 200,  80,   8, 10, true  },
    [xi.magic.spell.WIND_CAROL        ] = { 1, xi.effect.CAROL,     xi.element.WIND,          xi.mod.CAROL_EFFECT,    0,                        0,                   20, 200,  80,   8, 10, true  },
    [xi.magic.spell.EARTH_CAROL       ] = { 1, xi.effect.CAROL,     xi.element.EARTH,         xi.mod.CAROL_EFFECT,    0,                        0,                   20, 200,  80,   8, 10, true  },
    [xi.magic.spell.LIGHTNING_CAROL   ] = { 1, xi.effect.CAROL,     xi.element.THUNDER,       xi.mod.CAROL_EFFECT,    0,                        0,                   20, 200,  80,   8, 10, true  },
    [xi.magic.spell.WATER_CAROL       ] = { 1, xi.effect.CAROL,     xi.element.WATER,         xi.mod.CAROL_EFFECT,    0,                        0,                   20, 200,  80,   8, 10, true  },
    [xi.magic.spell.LIGHT_CAROL       ] = { 1, xi.effect.CAROL,     xi.element.LIGHT,         xi.mod.CAROL_EFFECT,    0,                        0,                   20, 200,  80,   8, 10, true  },
    [xi.magic.spell.DARK_CAROL        ] = { 1, xi.effect.CAROL,     xi.element.DARK,          xi.mod.CAROL_EFFECT,    0,                        0,                   20, 200,  80,   8, 10, true  },
    -- [xi.magic.spell.FIRE_CAROL_II     ] = { 2, xi.effect.CAROL_II,  xi.element.FIRE,        xi.mod.ETUDE_EFFECT,    0,                        0,                   10, 400,  15, 1.5, 10, true  },
    -- [xi.magic.spell.ICE_CAROL_II      ] = { 2, xi.effect.CAROL_II,  xi.element.ICE,         xi.mod.ETUDE_EFFECT,    0,                        0,                   10, 400,  15, 1.5, 10, true  },
    -- [xi.magic.spell.WIND_CAROL_II     ] = { 2, xi.effect.CAROL_II,  xi.element.WIND,        xi.mod.ETUDE_EFFECT,    0,                        0,                   10, 400,  15, 1.5, 10, true  },
    -- [xi.magic.spell.EARTH_CAROL_II    ] = { 2, xi.effect.CAROL_II,  xi.element.EARTH,       xi.mod.ETUDE_EFFECT,    0,                        0,                   10, 400,  15, 1.5, 10, true  },
    -- [xi.magic.spell.LIGHTNING_CAROL_II] = { 2, xi.effect.CAROL_II,  xi.element.THUNDER,     xi.mod.ETUDE_EFFECT,    0,                        0,                   10, 400,  15, 1.5, 10, true  },
    -- [xi.magic.spell.WATER_CAROL_II    ] = { 2, xi.effect.CAROL_II,  xi.element.WATER,       xi.mod.ETUDE_EFFECT,    0,                        0,                   10, 400,  15, 1.5, 10, true  },
    -- [xi.magic.spell.LIGHT_CAROL_II    ] = { 2, xi.effect.CAROL_II,  xi.element.LIGHT,       xi.mod.ETUDE_EFFECT,    0,                        0,                   10, 400,  15, 1.5, 10, true  },
    -- [xi.magic.spell.DARK_CAROL_II     ] = { 2, xi.effect.CAROL_II,  xi.element.DARK,        xi.mod.ETUDE_EFFECT,    0,                        0,                   10, 400,  15, 1.5, 10, true  },
    -- Etude
    [xi.magic.spell.SINEWY_ETUDE      ] = { 1, xi.effect.ETUDE,     xi.mod.STR,               xi.mod.ETUDE_EFFECT,    0,                        0,                    3,   0,   9,   1,  0, true  },
    [xi.magic.spell.DEXTROUS_ETUDE    ] = { 1, xi.effect.ETUDE,     xi.mod.DEX,               xi.mod.ETUDE_EFFECT,    0,                        0,                    3,   0,   9,   1,  0, true  },
    [xi.magic.spell.VIVACIOUS_ETUDE   ] = { 1, xi.effect.ETUDE,     xi.mod.VIT,               xi.mod.ETUDE_EFFECT,    0,                        0,                    3,   0,   9,   1,  0, true  },
    [xi.magic.spell.QUICK_ETUDE       ] = { 1, xi.effect.ETUDE,     xi.mod.AGI,               xi.mod.ETUDE_EFFECT,    0,                        0,                    3,   0,   9,   1,  0, true  },
    [xi.magic.spell.LEARNED_ETUDE     ] = { 1, xi.effect.ETUDE,     xi.mod.INT,               xi.mod.ETUDE_EFFECT,    0,                        0,                    3,   0,   9,   1,  0, true  },
    [xi.magic.spell.SPIRITED_ETUDE    ] = { 1, xi.effect.ETUDE,     xi.mod.MND,               xi.mod.ETUDE_EFFECT,    0,                        0,                    3,   0,   9,   1,  0, true  },
    [xi.magic.spell.ENCHANTING_ETUDE  ] = { 1, xi.effect.ETUDE,     xi.mod.CHR,               xi.mod.ETUDE_EFFECT,    0,                        0,                    3,   0,   9,   1,  0, true  },
    [xi.magic.spell.HERCULEAN_ETUDE   ] = { 2, xi.effect.ETUDE,     xi.mod.STR,               xi.mod.ETUDE_EFFECT,    0,                        0,                   12,   0,  15,   1,  0, true  },
    [xi.magic.spell.UNCANNY_ETUDE     ] = { 2, xi.effect.ETUDE,     xi.mod.DEX,               xi.mod.ETUDE_EFFECT,    0,                        0,                   12,   0,  15,   1,  0, true  },
    [xi.magic.spell.VITAL_ETUDE       ] = { 2, xi.effect.ETUDE,     xi.mod.VIT,               xi.mod.ETUDE_EFFECT,    0,                        0,                   12,   0,  15,   1,  0, true  },
    [xi.magic.spell.SWIFT_ETUDE       ] = { 2, xi.effect.ETUDE,     xi.mod.AGI,               xi.mod.ETUDE_EFFECT,    0,                        0,                   12,   0,  15,   1,  0, true  },
    [xi.magic.spell.SAGE_ETUDE        ] = { 2, xi.effect.ETUDE,     xi.mod.INT,               xi.mod.ETUDE_EFFECT,    0,                        0,                   12,   0,  15,   1,  0, true  },
    [xi.magic.spell.LOGICAL_ETUDE     ] = { 2, xi.effect.ETUDE,     xi.mod.MND,               xi.mod.ETUDE_EFFECT,    0,                        0,                   12,   0,  15,   1,  0, true  },
    [xi.magic.spell.BEWITCHING_ETUDE  ] = { 2, xi.effect.ETUDE,     xi.mod.CHR,               xi.mod.ETUDE_EFFECT,    0,                        0,                   12,   0,  15,   1,  0, true  },
    -- Madrigal: ADMITEDLY MADE UP IN ORIGINAL SCRIPT
    [xi.magic.spell.SWORD_MADRIGAL    ] = { 1, xi.effect.MADRIGAL,  xi.mod.AUGMENT_SONG_STAT, xi.mod.MADRIGAL_EFFECT, xi.merit.MADRIGAL_EFFECT, 0,                    5,  85,  45, 4.5, 18, true  },
    [xi.magic.spell.BLADE_MADRIGAL    ] = { 2, xi.effect.MADRIGAL,  xi.mod.AUGMENT_SONG_STAT, xi.mod.MADRIGAL_EFFECT, xi.merit.MADRIGAL_EFFECT, 0,                    9, 130,  60,   6, 18, true  },
    -- Mambo: ADMITEDLY MADE UP IN ORIGINAL SCRIPT
    [xi.magic.spell.SHEEPFOE_MAMBO    ] = { 1, xi.effect.MAMBO,     xi.mod.AUGMENT_SONG_STAT, xi.mod.MAMBO_EFFECT,    0,                        0,                    5,  85,  48,   5, 18, true  },
    [xi.magic.spell.DRAGONFOE_MAMBO   ] = { 2, xi.effect.MAMBO,     xi.mod.AUGMENT_SONG_STAT, xi.mod.MAMBO_EFFECT,    0,                        0,                    9, 130,  48,   7, 18, true  },
    -- March
    [xi.magic.spell.ADVANCING_MARCH   ] = { 1, xi.effect.MARCH,     xi.mod.AUGMENT_SONG_STAT, xi.mod.MARCH_EFFECT,    0,                        0,                   35, 200, 108,  11,  7, true  },
    [xi.magic.spell.VICTORY_MARCH     ] = { 2, xi.effect.MARCH,     xi.mod.AUGMENT_SONG_STAT, xi.mod.MARCH_EFFECT,    0,                        0,                   43, 300, 163,  16,  7, true  },
    [xi.magic.spell.HONOR_MARCH       ] = { 3, xi.effect.MARCH,     xi.mod.AUGMENT_SONG_STAT, xi.mod.MARCH_EFFECT,    0,                        0,                   24, 400, 126,  12,  7, true  }, -- Not an error. It is weaker.
    -- Minne: Skill Caps unknown?
    [xi.magic.spell.KNIGHTS_MINNE     ] = { 1, xi.effect.MINNE,     xi.mod.AUGMENT_SONG_STAT, xi.mod.MINNE_EFFECT,    xi.merit.MINNE_EFFECT,    xi.jp.MINNE_EFFECT,   8,   0,  30,   3, 10, true  },
    [xi.magic.spell.KNIGHTS_MINNE_II  ] = { 2, xi.effect.MINNE,     xi.mod.AUGMENT_SONG_STAT, xi.mod.MINNE_EFFECT,    xi.merit.MINNE_EFFECT,    xi.jp.MINNE_EFFECT,  12,   0,  69,   7, 10, true  },
    [xi.magic.spell.KNIGHTS_MINNE_III ] = { 3, xi.effect.MINNE,     xi.mod.AUGMENT_SONG_STAT, xi.mod.MINNE_EFFECT,    xi.merit.MINNE_EFFECT,    xi.jp.MINNE_EFFECT,  18,   0, 108,  11, 10, true  },
    [xi.magic.spell.KNIGHTS_MINNE_IV  ] = { 4, xi.effect.MINNE,     xi.mod.AUGMENT_SONG_STAT, xi.mod.MINNE_EFFECT,    xi.merit.MINNE_EFFECT,    xi.jp.MINNE_EFFECT,  30,   0, 164,  16, 10, true  },
    [xi.magic.spell.KNIGHTS_MINNE_V   ] = { 5, xi.effect.MINNE,     xi.mod.AUGMENT_SONG_STAT, xi.mod.MINNE_EFFECT,    xi.merit.MINNE_EFFECT,    xi.jp.MINNE_EFFECT,  50,   0, 204,  20, 10, true  },
    -- Minuet
    [xi.magic.spell.VALOR_MINUET      ] = { 1, xi.effect.MINUET,    xi.mod.AUGMENT_SONG_STAT, xi.mod.MINUET_EFFECT,   xi.merit.MINUET_EFFECT,   xi.jp.MINUET_EFFECT,  5,  50,  32,  3, 4.3, true  }, -- skill cap 163: (163 - 50)/4.3 + 5 ~31
    [xi.magic.spell.VALOR_MINUET_II   ] = { 2, xi.effect.MINUET,    xi.mod.AUGMENT_SONG_STAT, xi.mod.MINUET_EFFECT,   xi.merit.MINUET_EFFECT,   xi.jp.MINUET_EFFECT, 10, 100,  64,  6, 3.9, true  }, -- skill cap 310: (310 - 100)/3.9 + 10 ~64
    [xi.magic.spell.VALOR_MINUET_III  ] = { 3, xi.effect.MINUET,    xi.mod.AUGMENT_SONG_STAT, xi.mod.MINUET_EFFECT,   xi.merit.MINUET_EFFECT,   xi.jp.MINUET_EFFECT, 24, 200,  96,  9, 3.5, true  }, -- skill cap 455: (455 - 200)/3.5 + 24 ~96
    [xi.magic.spell.VALOR_MINUET_IV   ] = { 4, xi.effect.MINUET,    xi.mod.AUGMENT_SONG_STAT, xi.mod.MINUET_EFFECT,   xi.merit.MINUET_EFFECT,   xi.jp.MINUET_EFFECT, 31, 300, 112, 11, 3.3, true  }, -- skill cap 570: (570 - 300)/3.3 + 31 ~112
    [xi.magic.spell.VALOR_MINUET_V    ] = { 5, xi.effect.MINUET,    xi.mod.AUGMENT_SONG_STAT, xi.mod.MINUET_EFFECT,   xi.merit.MINUET_EFFECT,   xi.jp.MINUET_EFFECT, 32, 500, 124, 12,   4, true  }, -- skill cap 874: (874 - 500)/4 + 32 ~124
    -- Paeon
    [xi.magic.spell.ARMYS_PAEON       ] = { 1, xi.effect.PAEON,     xi.mod.AUGMENT_SONG_STAT, xi.mod.PAEON_EFFECT,    0,                        0,                    1, 100,   2,   1,  0, true  },
    [xi.magic.spell.ARMYS_PAEON_II    ] = { 2, xi.effect.PAEON,     xi.mod.AUGMENT_SONG_STAT, xi.mod.PAEON_EFFECT,    0,                        0,                    2, 150,   3,   1,  0, true  },
    [xi.magic.spell.ARMYS_PAEON_III   ] = { 3, xi.effect.PAEON,     xi.mod.AUGMENT_SONG_STAT, xi.mod.PAEON_EFFECT,    0,                        0,                    3, 200,   4,   1,  0, true  },
    [xi.magic.spell.ARMYS_PAEON_IV    ] = { 4, xi.effect.PAEON,     xi.mod.AUGMENT_SONG_STAT, xi.mod.PAEON_EFFECT,    0,                        0,                    4, 250,   5,   1,  0, true  },
    [xi.magic.spell.ARMYS_PAEON_V     ] = { 5, xi.effect.PAEON,     xi.mod.AUGMENT_SONG_STAT, xi.mod.PAEON_EFFECT,    0,                        0,                    5, 350,   7,   1,  0, true  },
    [xi.magic.spell.ARMYS_PAEON_VI    ] = { 6, xi.effect.PAEON,     xi.mod.AUGMENT_SONG_STAT, xi.mod.PAEON_EFFECT,    0,                        0,                    6, 450,   8,   1,  0, true  },
    -- Prelude
    [xi.magic.spell.HUNTERS_PRELUDE   ] = { 1, xi.effect.PRELUDE,   xi.mod.AUGMENT_SONG_STAT, xi.mod.PRELUDE_EFFECT,  0,                        0,                   10,  85,  45, 4.5, 18, true  },
    [xi.magic.spell.ARCHERS_PRELUDE   ] = { 2, xi.effect.PRELUDE,   xi.mod.AUGMENT_SONG_STAT, xi.mod.PRELUDE_EFFECT,  0,                        0,                   20, 130,  60,   6, 18, true  },
    -- Status effect resistance: Aubade, Capriccio, Gavotte, Operetta, Pastoral,
    [xi.magic.spell.FOWL_AUBADE       ] = { 1, xi.effect.AUBADE,    xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                   20, 200,  80,   8, 10, true  },
    [xi.magic.spell.GOLD_CAPRICCIO    ] = { 1, xi.effect.CAPRICCIO, xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                   20, 200,  80,   8, 10, true  },
    [xi.magic.spell.GOBLIN_GAVOTTE    ] = { 1, xi.effect.GAVOTTE,   xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                   20, 200,  80,   8, 10, true  },
    [xi.magic.spell.SCOPS_OPERETTA    ] = { 1, xi.effect.OPERETTA,  xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                   20, 200,  80,   8, 10, true  },
    [xi.magic.spell.PUPPETS_OPERETTA  ] = { 2, xi.effect.OPERETTA,  xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                   40, 200, 120,   8, 10, true  },
    [xi.magic.spell.HERB_PASTORAL     ] = { 1, xi.effect.PASTORAL,  xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                   20, 200,  80,   8, 10, true  },
    [xi.magic.spell.SHINING_FANTASIA  ] = { 1, xi.effect.FANTASIA,  xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                   20, 200,  80,   8, 10, true  },
    [xi.magic.spell.WARDING_ROUND     ] = { 1, xi.effect.ROUND,     xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                   20, 200,  80,   8, 10, true  },
    -- Misc.
    [xi.magic.spell.GODDESSS_HYMNUS   ] = { 1, xi.effect.HYMNUS,    xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                    1,   0,   1,   0,  0, false },
    [xi.magic.spell.SENTINELS_SCHERZO ] = { 1, xi.effect.SCHERZO,   xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                    1, 350,  45,   1, 10, false },
    [xi.magic.spell.RAPTOR_MAZURKA    ] = { 1, xi.effect.MAZURKA,   xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                    5,   0,  12,   0,  0, false },
    [xi.magic.spell.CHOCOBO_MAZURKA   ] = { 1, xi.effect.MAZURKA,   xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                   10,   0,  24,   0,  0, false },

    -- Emnity Songs
    [xi.magic.spell.FOE_SIRVENTE      ] = { 1, xi.effect.SIRVENTE,  xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                   35,   0,  35,   1,  0, true  },
    [xi.magic.spell.ADVENTURERS_DIRGE ] = { 1, xi.effect.DIRGE,     xi.mod.AUGMENT_SONG_STAT, 0,                      0,                        0,                   32,   0,  32,   0,  0, true  },
}

-----------------------------------
-- Pure inject forms (slice 6729)
-- Parity: internal/enhancingsong
-----------------------------------
xi.spells.enhancing.baseSongDurationSeconds = 120
xi.spells.enhancing.skillSinging            = 40
xi.spells.enhancing.skillStringInstrument   = 41
xi.spells.enhancing.skillWindInstrument     = 42

-- Pure ResolveSingingLevel (internal/enhancingsong.ResolveSingingLevel).
-- params: isPC, singingSkill, rangedSkillType, rangedSkillLevel
xi.spells.enhancing.resolveSingingLevelFromParams = function(params)
    params = params or {}
    local singing = params.singingSkill or 0
    if not params.isPC then
        return singing * 2
    end

    local rangeType = params.rangedSkillType or 0
    if rangeType == xi.skill.WIND_INSTRUMENT then
        return singing + (params.rangedSkillLevel or 0)
    elseif rangeType == xi.skill.STRING_INSTRUMENT then
        return singing + math.floor((params.rangedSkillLevel or 0) / 2)
    end

    return singing
end

-- Pure calculateSongPower (internal/enhancingsong.SongPower).
-- params: powerBase, tier, songEffect, skillNeeded, potencyCap, multiplier, divisor,
--         soulVoiceAffectsPower, singingLvl, instrumentBoost, meritBonus, jobPointBonus,
--         hasSoulVoice, hasMarcato, marcatoPower, augmentSongStat
xi.spells.enhancing.calculateSongPowerFromParams = function(params)
    params = params or {}
    local power      = params.powerBase or 0
    local tier       = params.tier or 0
    local songEffect = params.songEffect or 0
    local singingLvl = params.singingLvl or 0
    local skillNeeded = params.skillNeeded or 0
    local potencyCap = params.potencyCap or 0
    local multiplier = params.multiplier or 0
    local divisor    = params.divisor or 0

    -- Skill ladders.
    if songEffect == xi.effect.ETUDE and tier == 1 then
        if singingLvl >= 450 then
            power = power + 6
        elseif singingLvl >= 397 then
            power = power + 5
        elseif singingLvl >= 343 then
            power = power + 4
        elseif singingLvl >= 289 then
            power = power + 3
        elseif singingLvl >= 236 then
            power = power + 2
        elseif singingLvl >= 182 then
            power = power + 1
        end
    elseif songEffect == xi.effect.ETUDE and tier == 2 then
        if singingLvl >= 475 then
            power = power + 3
        elseif singingLvl >= 446 then
            power = power + 2
        elseif singingLvl >= 417 then
            power = power + 1
        end
    else
        if singingLvl > skillNeeded then
            if divisor == 0 then
                if skillNeeded > 0 then
                    power = power + 1
                end
            else
                power = math.floor(power + (singingLvl - skillNeeded) / divisor)
            end
        end
    end

    if power > potencyCap then
        power = potencyCap
    end

    power = math.floor(power + (params.instrumentBoost or 0) * multiplier)

    if (params.meritBonus or 0) ~= 0 then
        power = math.floor(power + params.meritBonus)
    end

    if (params.jobPointBonus or 0) ~= 0 then
        power = math.floor(power + params.jobPointBonus)
    end

    if params.soulVoiceAffectsPower then
        if params.hasSoulVoice then
            power = math.floor(power * 2)
        elseif params.hasMarcato then
            power = math.floor(power * (1 + (params.marcatoPower or 0) / 100))
        end
    end

    if songEffect == xi.effect.ETUDE then
        power = power + (params.augmentSongStat or 0)
    end

    return power
end

-- Pure calculateSongDuration (internal/enhancingsong.SongDuration).
-- returns: { duration, consumeMarcato }
-- Note: after Marcato JP add, LSB deletes Marcato so SV duration arm only sees SV.
xi.spells.enhancing.calculateSongDurationFromParams = function(params)
    params = params or {}
    local duration = xi.spells.enhancing.baseSongDurationSeconds
    local consume  = false

    duration = math.floor(duration * ((params.instrumentBoost or 0) * 0.1 + (params.songDurationBonus or 0) / 100 + 1))

    if params.hasClarionCall then
        duration = math.floor(duration + (params.clarionCallJP or 0) * 2)
    end

    if params.hasMarcato then
        duration = math.floor(duration + (params.marcatoJP or 0))
        consume  = true
    end

    if params.hasTenuto then
        duration = math.floor(duration + (params.tenutoJP or 0) * 2)
    end

    -- When power is NOT affected by Soul Voice, SV scales duration ×2.
    -- Marcato is consumed above, so no Marcato duration product after consume.
    if not params.soulVoiceAffectsPower and params.hasSoulVoice then
        duration = math.floor(duration * 2)
    end

    if params.hasTroubadour then
        duration = math.floor(duration * 2)
    end

    return { duration = duration, consumeMarcato = consume }
end

-- Pure subEffect resolve (internal/enhancingsong.ResolveSubEffect).
xi.spells.enhancing.resolveSubEffect = function(songEffect, tableSub, augmentSongStat, subModValue)
    if songEffect == xi.effect.CAROL then
        return (tableSub or 0) + (augmentSongStat or 0) * 100
    elseif songEffect == xi.effect.ETUDE then
        return tableSub or 0
    end

    return subModValue or 0
end

-- Pure Etude tier-2 paramFour (internal/enhancingsong.ParamFourFor).
xi.spells.enhancing.paramFourFor = function(songEffect, tier)
    if songEffect == xi.effect.ETUDE and tier == 2 then
        return 10
    end

    return 0
end

-- Pure March conversion (internal/enhancingsong.MarchPower).
xi.spells.enhancing.marchPower = function(power)
    return math.floor(((power or 0) / 1024) * 10000)
end

-- Pure useEnhancingSong plan (internal/enhancingsong.Use).
-- params: songEffect, tier, power, duration, tableSub, augmentSongStat, subModValue,
--         hasMarcato, addBardSongOK
xi.spells.enhancing.useEnhancingSongFromParams = function(params)
    params = params or {}
    local songEffect = params.songEffect or 0
    local power      = params.power or 0

    if songEffect == xi.effect.MARCH then
        power = xi.spells.enhancing.marchPower(power)
    end

    local res =
    {
        returnEffect   = songEffect,
        setMsg         = false,
        msg            = 0,
        applySong      = false,
        power          = power,
        duration       = params.duration or 0,
        paramFour      = xi.spells.enhancing.paramFourFor(songEffect, params.tier or 0),
        subEffect      = xi.spells.enhancing.resolveSubEffect(
            songEffect,
            params.tableSub or 0,
            params.augmentSongStat or 0,
            params.subModValue or 0
        ),
        tier           = params.tier or 0,
        consumeMarcato = params.hasMarcato and true or false,
    }

    if not params.addBardSongOK then
        res.setMsg = true
        res.msg    = 75 -- MAGIC_NO_EFFECT
        return res
    end

    res.applySong = true
    return res
end

-- Enhancing Song Potency function. (1/2) host → pure
xi.spells.enhancing.calculateSongPower = function(caster, target, spell, spellId, tier, songEffect, instrumentBoost, soulVoicePower)
    local meritEffect = pTable[spellId][column.MERIT_ID]
    local jpEffect    = pTable[spellId][column.JOB_POINT_ID]
    local rangeType   = 0
    local rangeSkill  = 0
    if caster:isPC() then
        rangeType  = caster:getWeaponSkillType(xi.slot.RANGED)
        rangeSkill = caster:getSkillLevel(rangeType)
    end

    local singingLvl = xi.spells.enhancing.resolveSingingLevelFromParams({
        isPC             = caster:isPC(),
        singingSkill     = caster:getSkillLevel(xi.skill.SINGING),
        rangedSkillType  = rangeType,
        rangedSkillLevel = rangeSkill,
    })

    local meritBonus = 0
    if meritEffect ~= 0 then
        meritBonus = caster:getMerit(meritEffect)
    end

    local jobPointBonus = 0
    if jpEffect ~= 0 then
        jobPointBonus = caster:getJobPointLevel(jpEffect)
    end

    local marcatoEffect = caster:getStatusEffect(xi.effect.MARCATO)
    local hasMarcato    = marcatoEffect ~= nil
    local marcatoPower  = hasMarcato and marcatoEffect:getPower() or 0

    return xi.spells.enhancing.calculateSongPowerFromParams({
        powerBase             = pTable[spellId][column.POWER_BASE],
        tier                  = tier,
        songEffect            = songEffect,
        skillNeeded           = pTable[spellId][column.SKILL_REQUIREMENT],
        potencyCap            = pTable[spellId][column.POWER_CAP],
        multiplier            = pTable[spellId][column.MULTIPLIER],
        divisor               = pTable[spellId][column.DIVISOR],
        soulVoiceAffectsPower = soulVoicePower,
        singingLvl            = singingLvl,
        instrumentBoost       = instrumentBoost,
        meritBonus            = meritBonus,
        jobPointBonus         = jobPointBonus,
        hasSoulVoice          = caster:hasStatusEffect(xi.effect.SOUL_VOICE),
        hasMarcato            = hasMarcato,
        marcatoPower          = marcatoPower,
        augmentSongStat       = caster:getMod(xi.mod.AUGMENT_SONG_STAT),
    })
end

-- Enhancing Song Duration function. (2/2) host → pure
xi.spells.enhancing.calculateSongDuration = function(caster, target, spell, instrumentBoost, soulVoicePower)
    local res = xi.spells.enhancing.calculateSongDurationFromParams({
        instrumentBoost       = instrumentBoost,
        songDurationBonus     = caster:getMod(xi.mod.SONG_DURATION_BONUS),
        hasClarionCall        = caster:hasStatusEffect(xi.effect.CLARION_CALL),
        clarionCallJP         = caster:getJobPointLevel(xi.jp.CLARION_CALL_EFFECT),
        hasMarcato            = caster:hasStatusEffect(xi.effect.MARCATO),
        marcatoJP             = caster:getJobPointLevel(xi.jp.MARCATO_EFFECT),
        hasTenuto             = caster:hasStatusEffect(xi.effect.TENUTO),
        tenutoJP              = caster:getJobPointLevel(xi.jp.TENUTO_EFFECT),
        soulVoiceAffectsPower = soulVoicePower,
        hasSoulVoice          = caster:hasStatusEffect(xi.effect.SOUL_VOICE),
        hasTroubadour         = caster:hasStatusEffect(xi.effect.TROUBADOUR),
    })

    if res.consumeMarcato then
        caster:delStatusEffect(xi.effect.MARCATO)
    end

    return res.duration
end

-- Main function for Enhancing Songs (host → pure plan).
xi.spells.enhancing.useEnhancingSong = function(caster, target, spell)
    local spellId = spell:getID()

    local tier            = pTable[spellId][column.EFFECT_TIER]
    local songEffect      = pTable[spellId][column.EFFECT_MAIN]
    local instrumentBoost = caster:getMod(pTable[spellId][column.MODIFIER]) + caster:getMod(xi.mod.ALL_SONGS_EFFECT)
    local soulVoicePower  = pTable[spellId][column.SOUL_VOICE]
    local tableSub        = pTable[spellId][column.EFFECT_SUB]

    local power    = xi.spells.enhancing.calculateSongPower(caster, target, spell, spellId, tier, songEffect, instrumentBoost, soulVoicePower)
    local duration = xi.spells.enhancing.calculateSongDuration(caster, target, spell, instrumentBoost, soulVoicePower)

    local subModValue = 0
    if songEffect ~= xi.effect.CAROL and songEffect ~= xi.effect.ETUDE then
        subModValue = caster:getMod(tableSub)
    end

    local plan = xi.spells.enhancing.useEnhancingSongFromParams({
        songEffect      = songEffect,
        tier            = tier,
        power           = power,
        duration        = duration,
        tableSub        = tableSub,
        augmentSongStat = caster:getMod(xi.mod.AUGMENT_SONG_STAT),
        subModValue     = subModValue,
        hasMarcato      = caster:hasStatusEffect(xi.effect.MARCATO),
        addBardSongOK   = true,
    })

    if plan.consumeMarcato then
        caster:delStatusEffect(xi.effect.MARCATO)
    end

    -- Change message when higher effect already in place.
    if not target:addBardSong(caster, songEffect, plan.power, plan.paramFour, plan.duration, caster:getID(), plan.subEffect, plan.tier) then
        spell:setMsg(xi.msg.basic.MAGIC_NO_EFFECT)
    end

    return songEffect
end
