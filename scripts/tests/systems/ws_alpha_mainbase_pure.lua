-----------------------------------
-- Pure system tests for WS alpha + mainBase dual-wire (slice 6756).
-- Calls production xi.weaponskills.alphaFromParams / mainBaseFromParams.
-- Goldens match internal/wsformula.Alpha / MainBase (1003).
-----------------------------------

require('scripts/globals/weaponskills')

local w = xi.weaponskills

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('WS alphaFromParams', function()
    it('Adoulin path always returns 1', function()
        for _, lvl in ipairs({ 1, 30, 60, 75, 99, 0, -1 }) do
            assert(w.alphaFromParams({
                level = lvl, useAdoulinWeaponSkillChanges = true,
            }) == 1)
        end
    end)

    it('legacy level bands', function()
        assert(almost(w.alphaFromParams({ level = 1 }), 1))
        assert(almost(w.alphaFromParams({ level = 5 }), 1))
        -- 1 - floor(6/6)/100 = 0.99
        assert(almost(w.alphaFromParams({ level = 6 }), 0.99))
        assert(almost(w.alphaFromParams({ level = 12 }), 0.98))
        -- level 59 still >5 branch: 1 - floor(59/6)/100 = 1 - 0.09
        assert(almost(w.alphaFromParams({ level = 59 }), 1 - math.floor(59 / 6) / 100))
        -- 0.9 - floor(0/2)/100 = 0.9
        assert(almost(w.alphaFromParams({ level = 60 }), 0.9))
        assert(almost(w.alphaFromParams({ level = 61 }), 0.9))
        -- floor(2/2)=1 → 0.89
        assert(almost(w.alphaFromParams({ level = 62 }), 0.89))
        assert(almost(w.alphaFromParams({ level = 75 }), 0.9 - math.floor(15 / 2) / 100))
        assert(almost(w.alphaFromParams({ level = 76 }), 0.85))
        assert(almost(w.alphaFromParams({ level = 99 }), 0.85))
    end)
end)

describe('WS mainBaseFromParams', function()
    it('floors weapon + fSTR + bonus + wsc*alpha', function()
        -- floor(50 + 10 + 0 + 20 * 1) = 80
        assert(w.mainBaseFromParams({
            weaponDamage = 50, fSTR = 10, bonusWSmods = 0, wsc = 20, alpha = 1,
        }) == 80)
        -- floor(50 + 10 + 5 + 20 * 0.85) = floor(50+10+5+17) = 82
        assert(w.mainBaseFromParams({
            weaponDamage = 50, fSTR = 10, bonusWSmods = 5, wsc = 20, alpha = 0.85,
        }) == 82)
        -- floor(10.7 + 0 + 0 + 0) = 10
        assert(w.mainBaseFromParams({ weaponDamage = 10.7 }) == 10)
        -- floor(0 + 0 + 0 + 10 * 0.99) = floor(9.9) = 9
        assert(w.mainBaseFromParams({ wsc = 10, alpha = 0.99 }) == 9)
    end)
end)
