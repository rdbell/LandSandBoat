-----------------------------------
-- Can use spell override functionality
-- Used to allow cast of spells granted by job points
--
-- Dual-wired pure inject forms (slice 6721 / 0883):
--   getSpellJobPointCostForJob, canUseSpellOverrideFromParams,
--   spentJobPointsForOverride, giftCatalogSize
-- Parity: internal/canusespell + can_use_spell_capacity.h
-----------------------------------
xi = xi or {}
xi.spells = xi.spells or {}

-- Spent-JP thresholds (internal/canusespell GiftJP*).
xi.spells.giftJP100  = 100
xi.spells.giftJP550  = 550
xi.spells.giftJP1200 = 1200
xi.spells.unknownGiftCost = -1
xi.spells.minLevelForSpentJP = 99

local jobPointSpellGiftMap =
{
    [xi.job.WHM] =
    {
        [xi.magic.spell.RERAISE_IV] = 100,
        [xi.magic.spell.FULL_CURE]  = 1200,
    },

    [xi.job.BLM] =
    {
        [xi.magic.spell.FIRE_VI]     = 100,
        [xi.magic.spell.BLIZZARD_VI] = 100,
        [xi.magic.spell.AERO_VI]     = 100,
        [xi.magic.spell.STONE_VI]    = 100,
        [xi.magic.spell.THUNDER_VI]  = 100,
        [xi.magic.spell.WATER_VI]    = 100,

        [xi.magic.spell.ASPIR_III]   = 550,

        [xi.magic.spell.DEATH]       = 1200,
    },

    [xi.job.RDM] =
    {
        [xi.magic.spell.FIRE_V]       = 100,
        [xi.magic.spell.BLIZZARD_V]   = 100,
        [xi.magic.spell.AERO_V]       = 100,
        [xi.magic.spell.STONE_V]      = 100,
        [xi.magic.spell.THUNDER_V]    = 100,
        [xi.magic.spell.WATER_V]      = 100,

        [xi.magic.spell.ADDLE_II]     = 550,
        [xi.magic.spell.DISTRACT_III] = 550,
        [xi.magic.spell.FRAZZLE_III]  = 550,

        [xi.magic.spell.REFRESH_III]  = 1200,
        [xi.magic.spell.TEMPER_II]    = 1200,
    },

    [xi.job.PLD] =
    {
        [xi.magic.spell.ENLIGHT_II] = 100,
    },

    [xi.job.DRK] =
    {
        [xi.magic.spell.ENDARK_II] = 100,
        [xi.magic.spell.DRAIN_III] = 100,
    },

    [xi.job.BRD] =
    {
        [xi.magic.spell.FIRE_THRENODY_II]      = 100,
        [xi.magic.spell.ICE_THRENODY_II]       = 100,
        [xi.magic.spell.WIND_THRENODY_II]      = 100,
        [xi.magic.spell.EARTH_THRENODY_II]     = 100,
        [xi.magic.spell.LIGHTNING_THRENODY_II] = 100,
        [xi.magic.spell.WATER_THRENODY_II]     = 100,
        [xi.magic.spell.LIGHT_THRENODY_II]     = 100,
        [xi.magic.spell.DARK_THRENODY_II]      = 100,
    },

    [xi.job.NIN] =
    {
        [xi.magic.spell.UTSUSEMI_SAN] = 100,
    },

    [xi.job.SCH] =
    {
        [xi.magic.spell.FIRESTORM_II]    = 100,
        [xi.magic.spell.HAILSTORM_II]    = 100,
        [xi.magic.spell.WINDSTORM_II]    = 100,
        [xi.magic.spell.SANDSTORM_II]    = 100,
        [xi.magic.spell.THUNDERSTORM_II] = 100,
        [xi.magic.spell.RAINSTORM_II]    = 100,
        [xi.magic.spell.AURORASTORM_II]  = 100,
        [xi.magic.spell.VOIDSTORM_II]    = 100,

        [xi.magic.spell.PYROHELIX_II]    = 1200,
        [xi.magic.spell.CRYOHELIX_II]    = 1200,
        [xi.magic.spell.ANEMOHELIX_II]   = 1200,
        [xi.magic.spell.GEOHELIX_II]     = 1200,
        [xi.magic.spell.LUMINOHELIX_II]  = 1200,
        [xi.magic.spell.NOCTOHELIX_II]   = 1200,
    },

    [xi.job.GEO] =
    {
        [xi.magic.spell.FIRE_V]       = 100,
        [xi.magic.spell.BLIZZARD_V]   = 100,
        [xi.magic.spell.AERO_V]       = 100,
        [xi.magic.spell.STONE_V]      = 100,
        [xi.magic.spell.THUNDER_V]    = 100,
        [xi.magic.spell.WATER_V]      = 100,

        [xi.magic.spell.FIRA_III]     = 1200,
        [xi.magic.spell.BLIZZARA_III] = 1200,
        [xi.magic.spell.AERA_III]     = 1200,
        [xi.magic.spell.STONERA_III]  = 1200,
        [xi.magic.spell.THUNDARA_III] = 1200,
        [xi.magic.spell.WATERA_III]   = 1200,
    },

    [xi.job.RUN] =
    {
        [xi.magic.spell.TEMPER] = 550,
    }
}

-- Pure: getSpellJobPointCostForJob (missing → -1).
-- Dual-wired to internal/canusespell.GiftCost / canusespellhelpers::GiftCost.
xi.spells.getSpellJobPointCostForJob = function(job, spellID)
    local jobGiftMap = jobPointSpellGiftMap[job]
    if jobGiftMap then
        local jobPointCost = jobGiftMap[spellID]

        if jobPointCost then
            return jobPointCost
        end
    end

    return xi.spells.unknownGiftCost
end

-- Pure spent-JP inject after Level Sync gate (main level < 99 → 0).
-- Dual-wired to internal/canusespell.SpentJobPointsForOverride.
xi.spells.spentJobPointsForOverride = function(isPC, mainLevel, rawSpentJP)
    if not isPC or (mainLevel or 0) < xi.spells.minLevelForSpentJP then
        return 0
    end

    return rawSpentJP or 0
end

-- Pure canUseSpellOverride after injects.
-- params: job, spellID, spentJobPoints
-- Dual-wired to internal/canusespell.CanUseOverride.
xi.spells.canUseSpellOverrideFromParams = function(params)
    params = params or {}
    local cost = xi.spells.getSpellJobPointCostForJob(params.job or 0, params.spellID or 0)

    if cost == xi.spells.unknownGiftCost then -- that job can't cast that spell no matter how many JP they have
        return false
    end

    return (params.spentJobPoints or 0) >= cost
end

-- Catalog completeness (60 gift rows).
xi.spells.giftCatalogSize = function()
    local n = 0
    for _, jobMap in pairs(jobPointSpellGiftMap) do
        for _ in pairs(jobMap) do
            n = n + 1
        end
    end

    return n
end

-- return true to indicate that the spell can indeed be cast
-- return false falls back to default behavior of checking main/sub job levels for cast availability
-- note: this only affects whether or not you are able to cast a spell in general, MP costs (if any) are still required.
-- Host: injects main job / spell ID / spent JP into pure canUseSpellOverrideFromParams.
xi.spells.canUseSpellOverride = function(player, spell)
    return xi.spells.canUseSpellOverrideFromParams({
        job            = player:getMainJob(),
        spellID        = spell:getID(),
        spentJobPoints = player:getSpentJobPoints(),
    })
end
