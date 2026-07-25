-----------------------------------
-- Pure system tests for weaponskill single-hit shadow absorb dual-wire (slice 6752).
-- Calls production xi.weaponskills.singleHitShadowAbsorbFromParams.
-- Goldens match internal/shadowabsorb.SingleHitAbsorb (0877 deferred dual-wire).
-----------------------------------

require('scripts/globals/weaponskills')

local w = xi.weaponskills

describe('WS single-hit shadow pure pins', function()
    it('blink proc threshold', function()
        assert(w.wsBlinkProcThreshold == 80)
    end)
end)

describe('singleHitShadowAbsorbFromParams utsusemi', function()
    it('consumes one utsusemi and maps icons 1..3 only', function()
        local res = w.singleHitShadowAbsorbFromParams({
            utsusemi = 3, hasCopyImageEffect = true,
        })
        assert(res.absorbed and res.remaining == 2 and res.usedUtsusemi)
        assert(res.icon == xi.effect.COPY_IMAGE_2 and res.setIcon)
        assert(not res.delCopyImage and not res.delBlink)

        -- last shadow
        res = w.singleHitShadowAbsorbFromParams({
            utsusemi = 1, hasCopyImageEffect = true,
        })
        assert(res.absorbed and res.remaining == 0)
        assert(res.delCopyImage and res.delBlink)
        assert(not res.setIcon and res.icon == 0)

        -- remaining 4: no COPY_IMAGE_4 branch
        res = w.singleHitShadowAbsorbFromParams({
            utsusemi = 5, hasCopyImageEffect = true,
        })
        assert(res.remaining == 4 and res.icon == 0 and not res.setIcon)
    end)

    it('skips icon when copy-image effect missing', function()
        local res = w.singleHitShadowAbsorbFromParams({
            utsusemi = 2, hasCopyImageEffect = false,
        })
        assert(res.absorbed and res.remaining == 1)
        assert(res.icon == xi.effect.COPY_IMAGE and not res.setIcon)
    end)
end)

describe('singleHitShadowAbsorbFromParams blink', function()
    it('requires roll <= 80 when utsusemi is zero', function()
        local res = w.singleHitShadowAbsorbFromParams({
            blink = 2, blinkProcRoll = 81,
        })
        assert(not res.absorbed)

        res = w.singleHitShadowAbsorbFromParams({
            blink = 2, blinkProcRoll = 80,
        })
        assert(res.absorbed and res.usedBlink and res.remaining == 1)
        assert(not res.setIcon and res.icon == 0)

        res = w.singleHitShadowAbsorbFromParams({
            blink = 1, blinkProcRoll = 1,
        })
        assert(res.absorbed and res.remaining == 0)
        assert(res.delBlink and res.delCopyImage)
    end)

    it('misses without shadows even on good roll', function()
        local res = w.singleHitShadowAbsorbFromParams({
            blink = 0, blinkProcRoll = 1,
        })
        assert(not res.absorbed)
    end)
end)
