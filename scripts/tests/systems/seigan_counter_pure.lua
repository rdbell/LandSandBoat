-----------------------------------
-- Pure system tests for Seigan counter dual-wire helpers (slice 6692).
-- Calls production xi.combat.counter pure exports.
-- Goldens match internal/seigancounter (0905 / 1575).
-----------------------------------

require('scripts/globals/combat/counter')

local ctr = xi.combat.counter

describe('seigan counter constants', function()
    it('pins base rate and facing cone', function()
        assert(ctr.baseCounterRate == 25)
        assert(ctr.facingCone == 64)
    end)
end)

describe('checkSeiganCounterFromParams gates', function()
    local function base(overrides)
        local p = {
            hasThirdEye = true, hasSeigan = true, isFacing64 = true, isEngaged = true,
            isPC = true, isWeaponTwoHanded = true,
            thirdEyeCounterRateMod = 0, hitRateFactor = 1.0, roll1to100 = 1,
        }
        if overrides then
            for k, v in pairs(overrides) do
                p[k] = v
            end
        end
        return p
    end

    it('succeeds when all gates pass and roll is low', function()
        assert(ctr.checkSeiganCounterFromParams(base()))
    end)

    it('fails early gates', function()
        assert(not ctr.checkSeiganCounterFromParams(base({ hasThirdEye = false })))
        assert(not ctr.checkSeiganCounterFromParams(base({ hasSeigan = false })))
        assert(not ctr.checkSeiganCounterFromParams(base({ isFacing64 = false })))
        assert(not ctr.checkSeiganCounterFromParams(base({ isEngaged = false })))
        assert(not ctr.checkSeiganCounterFromParams(base({ isPC = true, isWeaponTwoHanded = false })))
    end)

    it('allows non-PC without 2H weapon', function()
        assert(ctr.checkSeiganCounterFromParams(base({
            isPC = false, isWeaponTwoHanded = false, roll1to100 = 1,
        })))
    end)
end)

describe('checkSeiganCounterFromParams roll threshold', function()
    it('compares roll to base*hitRateFactor', function()
        -- base 25, hitRate 1.0 → threshold 25; roll 25 succeeds, 26 fails
        assert(ctr.checkSeiganCounterFromParams({
            hasThirdEye = true, hasSeigan = true, isFacing64 = true, isEngaged = true,
            isPC = false, thirdEyeCounterRateMod = 0, hitRateFactor = 1.0, roll1to100 = 25,
        }))
        assert(not ctr.checkSeiganCounterFromParams({
            hasThirdEye = true, hasSeigan = true, isFacing64 = true, isEngaged = true,
            isPC = false, thirdEyeCounterRateMod = 0, hitRateFactor = 1.0, roll1to100 = 26,
        }))

        -- mod +10 → base 35
        assert(ctr.checkSeiganCounterFromParams({
            hasThirdEye = true, hasSeigan = true, isFacing64 = true, isEngaged = true,
            isPC = false, thirdEyeCounterRateMod = 10, hitRateFactor = 1.0, roll1to100 = 35,
        }))
        assert(not ctr.checkSeiganCounterFromParams({
            hasThirdEye = true, hasSeigan = true, isFacing64 = true, isEngaged = true,
            isPC = false, thirdEyeCounterRateMod = 10, hitRateFactor = 1.0, roll1to100 = 36,
        }))

        -- hitRate 0.5 → threshold 12.5; roll 12 succeeds, 13 fails
        assert(ctr.checkSeiganCounterFromParams({
            hasThirdEye = true, hasSeigan = true, isFacing64 = true, isEngaged = true,
            isPC = false, thirdEyeCounterRateMod = 0, hitRateFactor = 0.5, roll1to100 = 12,
        }))
        assert(not ctr.checkSeiganCounterFromParams({
            hasThirdEye = true, hasSeigan = true, isFacing64 = true, isEngaged = true,
            isPC = false, thirdEyeCounterRateMod = 0, hitRateFactor = 0.5, roll1to100 = 13,
        }))
    end)
end)
