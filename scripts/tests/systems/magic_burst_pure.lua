-----------------------------------
-- Pure system tests for magicburst dual-wire helpers (slice 6704).
-- Calls production xi.magicburst pure exports.
-- Goldens match internal/magicburstlua (0994).
-----------------------------------

require('scripts/globals/magicburst')

local mb = xi.magicburst
local el = xi.element
local sc = xi.skillchainType

describe('doesElementMatchWeaponskill', function()
    it('matches Lv1 singles and rejects SCProp 0', function()
        assert(not mb.doesElementMatchWeaponskill(el.FIRE, 0))
        assert(not mb.doesElementMatchWeaponskill(el.FIRE, sc.NONE))

        assert(mb.doesElementMatchWeaponskill(el.LIGHT, sc.TRANSFIXION))
        assert(not mb.doesElementMatchWeaponskill(el.DARK, sc.TRANSFIXION))
        assert(mb.doesElementMatchWeaponskill(el.DARK, sc.COMPRESSION))
        assert(mb.doesElementMatchWeaponskill(el.FIRE, sc.LIQUEFACTION))
        assert(not mb.doesElementMatchWeaponskill(el.ICE, sc.LIQUEFACTION))
        assert(mb.doesElementMatchWeaponskill(el.EARTH, sc.SCISSION))
        assert(mb.doesElementMatchWeaponskill(el.WATER, sc.REVERBERATION))
        assert(mb.doesElementMatchWeaponskill(el.WIND, sc.DETONATION))
        assert(mb.doesElementMatchWeaponskill(el.ICE, sc.INDURATION))
        assert(mb.doesElementMatchWeaponskill(el.THUNDER, sc.IMPACTION))
    end)

    it('matches Lv2 pairs and Lv3/Lv4 quads', function()
        assert(mb.doesElementMatchWeaponskill(el.DARK, sc.GRAVITATION))
        assert(mb.doesElementMatchWeaponskill(el.EARTH, sc.GRAVITATION))
        assert(not mb.doesElementMatchWeaponskill(el.FIRE, sc.GRAVITATION))

        assert(mb.doesElementMatchWeaponskill(el.WATER, sc.DISTORTION))
        assert(mb.doesElementMatchWeaponskill(el.ICE, sc.DISTORTION))
        assert(mb.doesElementMatchWeaponskill(el.FIRE, sc.FUSION))
        assert(mb.doesElementMatchWeaponskill(el.LIGHT, sc.FUSION))
        assert(mb.doesElementMatchWeaponskill(el.WIND, sc.FRAGMENTATION))
        assert(mb.doesElementMatchWeaponskill(el.THUNDER, sc.FRAGMENTATION))

        for _, scType in ipairs({ sc.LIGHT, sc.LIGHT_II }) do
            assert(mb.doesElementMatchWeaponskill(el.FIRE, scType))
            assert(mb.doesElementMatchWeaponskill(el.WIND, scType))
            assert(mb.doesElementMatchWeaponskill(el.THUNDER, scType))
            assert(mb.doesElementMatchWeaponskill(el.LIGHT, scType))
            assert(not mb.doesElementMatchWeaponskill(el.ICE, scType))
            assert(not mb.doesElementMatchWeaponskill(el.DARK, scType))
        end

        for _, scType in ipairs({ sc.DARKNESS, sc.DARKNESS_II }) do
            assert(mb.doesElementMatchWeaponskill(el.ICE, scType))
            assert(mb.doesElementMatchWeaponskill(el.EARTH, scType))
            assert(mb.doesElementMatchWeaponskill(el.WATER, scType))
            assert(mb.doesElementMatchWeaponskill(el.DARK, scType))
            assert(not mb.doesElementMatchWeaponskill(el.FIRE, scType))
            assert(not mb.doesElementMatchWeaponskill(el.LIGHT, scType))
        end
    end)

    it('rejects out-of-range element and SC', function()
        assert(not mb.doesElementMatchWeaponskill(el.NONE, sc.LIQUEFACTION))
        assert(not mb.doesElementMatchWeaponskill(9, sc.LIQUEFACTION))
        assert(not mb.doesElementMatchWeaponskill(el.FIRE, 17))
    end)
end)

describe('formMagicBurstFromParams', function()
    it('returns tier and count on match', function()
        local tier, count = mb.formMagicBurstFromParams({
            actionElement = el.FIRE, hasResonance = true,
            resonanceTier = 1, resonancePower = sc.LIQUEFACTION, resonanceSubPower = 2,
        })
        assert(tier == 1 and count == 2)

        tier, count = mb.formMagicBurstFromParams({
            actionElement = el.LIGHT, hasResonance = true,
            resonanceTier = 4, resonancePower = sc.LIGHT_II, resonanceSubPower = 5,
        })
        assert(tier == 4 and count == 5)

        -- subPower 0 still returned
        tier, count = mb.formMagicBurstFromParams({
            actionElement = el.WATER, hasResonance = true,
            resonanceTier = 1, resonancePower = sc.REVERBERATION, resonanceSubPower = 0,
        })
        assert(tier == 1 and count == 0)
    end)

    it('gates element, resonance, tier, and membership', function()
        local tier, count = mb.formMagicBurstFromParams({})
        assert(tier == 0 and count == 0)

        tier, count = mb.formMagicBurstFromParams({
            actionElement = el.NONE, hasResonance = true, resonanceTier = 1,
            resonancePower = sc.LIQUEFACTION, resonanceSubPower = 1,
        })
        assert(tier == 0 and count == 0)

        tier, count = mb.formMagicBurstFromParams({
            actionElement = el.FIRE, hasResonance = false, resonanceTier = 1,
            resonancePower = sc.LIQUEFACTION, resonanceSubPower = 1,
        })
        assert(tier == 0 and count == 0)

        tier, count = mb.formMagicBurstFromParams({
            actionElement = el.FIRE, hasResonance = true, resonanceTier = 0,
            resonancePower = sc.LIQUEFACTION, resonanceSubPower = 1,
        })
        assert(tier == 0 and count == 0)

        tier, count = mb.formMagicBurstFromParams({
            actionElement = el.ICE, hasResonance = true, resonanceTier = 2,
            resonancePower = sc.LIQUEFACTION, resonanceSubPower = 3,
        })
        assert(tier == 0 and count == 0)

        tier, count = mb.formMagicBurstFromParams({
            actionElement = el.FIRE, hasResonance = true, resonanceTier = 2,
            resonancePower = sc.NONE, resonanceSubPower = 3,
        })
        assert(tier == 0 and count == 0)
    end)
end)
