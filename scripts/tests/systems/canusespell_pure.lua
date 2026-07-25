-----------------------------------
-- Pure system tests for can-use-spell JP override dual-wire (slice 6721).
-- Calls production xi.spells pure exports.
-- Goldens match internal/canusespell (0883) and can_use_spell_capacity.h.
-----------------------------------

require('scripts/globals/canusespell_override')

local s = xi.spells

describe('Can-use-spell gift constants', function()
    it('pins JP thresholds and catalog size', function()
        assert(s.giftJP100 == 100)
        assert(s.giftJP550 == 550)
        assert(s.giftJP1200 == 1200)
        assert(s.unknownGiftCost == -1)
        assert(s.minLevelForSpentJP == 99)
        assert(s.giftCatalogSize() == 60)
    end)
end)

describe('getSpellJobPointCostForJob', function()
    it('returns known gift costs', function()
        assert(s.getSpellJobPointCostForJob(xi.job.WHM, xi.magic.spell.RERAISE_IV) == 100)
        assert(s.getSpellJobPointCostForJob(xi.job.WHM, xi.magic.spell.FULL_CURE) == 1200)
        assert(s.getSpellJobPointCostForJob(xi.job.BLM, xi.magic.spell.FIRE_VI) == 100)
        assert(s.getSpellJobPointCostForJob(xi.job.BLM, xi.magic.spell.ASPIR_III) == 550)
        assert(s.getSpellJobPointCostForJob(xi.job.BLM, xi.magic.spell.DEATH) == 1200)
        assert(s.getSpellJobPointCostForJob(xi.job.RDM, xi.magic.spell.FIRE_V) == 100)
        assert(s.getSpellJobPointCostForJob(xi.job.RDM, xi.magic.spell.ADDLE_II) == 550)
        assert(s.getSpellJobPointCostForJob(xi.job.RDM, xi.magic.spell.REFRESH_III) == 1200)
        assert(s.getSpellJobPointCostForJob(xi.job.PLD, xi.magic.spell.ENLIGHT_II) == 100)
        assert(s.getSpellJobPointCostForJob(xi.job.DRK, xi.magic.spell.ENDARK_II) == 100)
        assert(s.getSpellJobPointCostForJob(xi.job.DRK, xi.magic.spell.DRAIN_III) == 100)
        assert(s.getSpellJobPointCostForJob(xi.job.NIN, xi.magic.spell.UTSUSEMI_SAN) == 100)
        assert(s.getSpellJobPointCostForJob(xi.job.SCH, xi.magic.spell.FIRESTORM_II) == 100)
        assert(s.getSpellJobPointCostForJob(xi.job.SCH, xi.magic.spell.PYROHELIX_II) == 1200)
        assert(s.getSpellJobPointCostForJob(xi.job.GEO, xi.magic.spell.FIRA_III) == 1200)
        assert(s.getSpellJobPointCostForJob(xi.job.RUN, xi.magic.spell.TEMPER) == 550)
    end)

    it('returns -1 for missing job/spell gifts', function()
        assert(s.getSpellJobPointCostForJob(xi.job.WAR, xi.magic.spell.FIRE_V) == -1)
        assert(s.getSpellJobPointCostForJob(xi.job.WHM, xi.magic.spell.DEATH) == -1)
        assert(s.getSpellJobPointCostForJob(xi.job.BLM, xi.magic.spell.FIRE_V) == -1)
        assert(s.getSpellJobPointCostForJob(xi.job.RDM, xi.magic.spell.TEMPER) == -1)
        assert(s.getSpellJobPointCostForJob(xi.job.BLM, 0) == -1)
    end)
end)

describe('canUseSpellOverrideFromParams', function()
    it('threshold gates spent JP', function()
        assert(not s.canUseSpellOverrideFromParams({
            job = xi.job.WHM, spellID = xi.magic.spell.RERAISE_IV, spentJobPoints = 99,
        }))
        assert(s.canUseSpellOverrideFromParams({
            job = xi.job.WHM, spellID = xi.magic.spell.RERAISE_IV, spentJobPoints = 100,
        }))
        assert(s.canUseSpellOverrideFromParams({
            job = xi.job.WHM, spellID = xi.magic.spell.RERAISE_IV, spentJobPoints = 500,
        }))

        assert(not s.canUseSpellOverrideFromParams({
            job = xi.job.BLM, spellID = xi.magic.spell.DEATH, spentJobPoints = 1199,
        }))
        assert(s.canUseSpellOverrideFromParams({
            job = xi.job.BLM, spellID = xi.magic.spell.DEATH, spentJobPoints = 1200,
        }))

        assert(not s.canUseSpellOverrideFromParams({
            job = xi.job.RDM, spellID = xi.magic.spell.ADDLE_II, spentJobPoints = 549,
        }))
        assert(s.canUseSpellOverrideFromParams({
            job = xi.job.RDM, spellID = xi.magic.spell.ADDLE_II, spentJobPoints = 550,
        }))

        assert(not s.canUseSpellOverrideFromParams({
            job = xi.job.RUN, spellID = xi.magic.spell.TEMPER, spentJobPoints = 0,
        }))
        assert(s.canUseSpellOverrideFromParams({
            job = xi.job.RUN, spellID = xi.magic.spell.TEMPER, spentJobPoints = 550,
        }))
    end)

    it('missing gift always false', function()
        assert(not s.canUseSpellOverrideFromParams({
            job = xi.job.WAR, spellID = xi.magic.spell.FIRE_V, spentJobPoints = 9999,
        }))
        assert(not s.canUseSpellOverrideFromParams({
            job = xi.job.WHM, spellID = xi.magic.spell.DEATH, spentJobPoints = 9999,
        }))
        assert(not s.canUseSpellOverrideFromParams({
            job = xi.job.BLM, spellID = xi.magic.spell.FIRE_V, spentJobPoints = 9999,
        }))
        assert(not s.canUseSpellOverrideFromParams({
            job = xi.job.NIN, spellID = xi.magic.spell.UTSUSEMI_SAN, spentJobPoints = 0,
        }))
        assert(s.canUseSpellOverrideFromParams({
            job = xi.job.NIN, spellID = xi.magic.spell.UTSUSEMI_SAN, spentJobPoints = 100,
        }))
    end)
end)

describe('spentJobPointsForOverride', function()
    it('gates non-PC and main level < 99', function()
        assert(s.spentJobPointsForOverride(false, 99, 500) == 0)
        assert(s.spentJobPointsForOverride(true, 98, 500) == 0)
        assert(s.spentJobPointsForOverride(true, 99, 500) == 500)
        assert(s.spentJobPointsForOverride(true, 99, 0) == 0)
    end)
end)

describe('elemental tier catalogs', function()
    it('BLM tier-VI and RDM/GEO tier-V at 100 JP', function()
        for _, sp in ipairs({
            xi.magic.spell.FIRE_VI, xi.magic.spell.BLIZZARD_VI, xi.magic.spell.AERO_VI,
            xi.magic.spell.STONE_VI, xi.magic.spell.THUNDER_VI, xi.magic.spell.WATER_VI,
        }) do
            assert(s.getSpellJobPointCostForJob(xi.job.BLM, sp) == 100)
        end

        for _, job in ipairs({ xi.job.RDM, xi.job.GEO }) do
            for _, sp in ipairs({
                xi.magic.spell.FIRE_V, xi.magic.spell.BLIZZARD_V, xi.magic.spell.AERO_V,
                xi.magic.spell.STONE_V, xi.magic.spell.THUNDER_V, xi.magic.spell.WATER_V,
            }) do
                assert(s.getSpellJobPointCostForJob(job, sp) == 100)
            end
        end
    end)

    it('SCH storms 100 / helixes 1200; BRD threnody II 100', function()
        for _, sp in ipairs({
            xi.magic.spell.FIRESTORM_II, xi.magic.spell.HAILSTORM_II,
            xi.magic.spell.WINDSTORM_II, xi.magic.spell.SANDSTORM_II,
            xi.magic.spell.THUNDERSTORM_II, xi.magic.spell.RAINSTORM_II,
            xi.magic.spell.AURORASTORM_II, xi.magic.spell.VOIDSTORM_II,
        }) do
            assert(s.getSpellJobPointCostForJob(xi.job.SCH, sp) == 100)
        end

        for _, sp in ipairs({
            xi.magic.spell.PYROHELIX_II, xi.magic.spell.CRYOHELIX_II,
            xi.magic.spell.ANEMOHELIX_II, xi.magic.spell.GEOHELIX_II,
            xi.magic.spell.LUMINOHELIX_II, xi.magic.spell.NOCTOHELIX_II,
        }) do
            assert(s.getSpellJobPointCostForJob(xi.job.SCH, sp) == 1200)
        end

        for _, sp in ipairs({
            xi.magic.spell.FIRE_THRENODY_II, xi.magic.spell.ICE_THRENODY_II,
            xi.magic.spell.WIND_THRENODY_II, xi.magic.spell.EARTH_THRENODY_II,
            xi.magic.spell.LIGHTNING_THRENODY_II, xi.magic.spell.WATER_THRENODY_II,
            xi.magic.spell.LIGHT_THRENODY_II, xi.magic.spell.DARK_THRENODY_II,
        }) do
            assert(s.getSpellJobPointCostForJob(xi.job.BRD, sp) == 100)
        end
    end)
end)
