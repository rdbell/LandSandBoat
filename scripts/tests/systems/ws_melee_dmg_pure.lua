-----------------------------------
-- Pure system tests for getMeleeDmg dual-wire (slice 6754).
-- Calls production xi.weaponskills.meleeDmgFromParams.
-- Goldens match internal/wsformula.MeleeDmg (1040).
-----------------------------------

require('scripts/globals/weaponskills')

local w = xi.weaponskills

local function almost(a, b)
    return math.abs(a - b) < 1e-9
end

describe('WS melee dmg pure pins', function()
    it('h2h skill constants', function()
        assert(almost(w.h2hSkillFactor, 0.11))
        assert(almost(w.h2hSkillBase, 3.0))
        assert(almost(w.h2hSkillDamageFromParams({ skillLevel = 0 }), 3))
        assert(almost(w.h2hSkillDamageFromParams({ skillLevel = 100 }), 14))
        assert(almost(w.h2hSkillDamageFromParams({ skillLevel = 250 }), 30.5))
    end)
end)

describe('meleeDmgFromParams', function()
    it('non-H2H passthrough ignores kick/footwork', function()
        local main, off = w.meleeDmgFromParams({
            weaponType = xi.skill.SWORD or 3,
            kick = true, mainhandDmg = 40, offhandDmg = 25,
            h2hSkillLevel = 300, hasFootwork = true, kickDmg = 99,
        })
        assert(almost(main, 40) and almost(off, 25))
    end)

    it('H2H no kick adds skill term and mirrors offhand', function()
        -- 50 + 100*0.11+3 = 64
        local main, off = w.meleeDmgFromParams({
            weaponType = xi.skill.HAND_TO_HAND,
            kick = false, mainhandDmg = 50, offhandDmg = 1,
            h2hSkillLevel = 100, hasFootwork = true, kickDmg = 99,
        })
        assert(almost(main, 64) and almost(off, 64))
    end)

    it('H2H kick without footwork keeps mainhand base', function()
        -- 40 + 3 = 43
        local main, off = w.meleeDmgFromParams({
            weaponType = xi.skill.HAND_TO_HAND,
            kick = true, mainhandDmg = 40, offhandDmg = 0,
            h2hSkillLevel = 0, hasFootwork = false, kickDmg = 120,
        })
        assert(almost(main, 43) and almost(off, 43))
    end)

    it('H2H kick with footwork uses kick damage', function()
        -- 80 + 200*0.11+3 = 105
        local main, off = w.meleeDmgFromParams({
            weaponType = xi.skill.HAND_TO_HAND,
            kick = true, mainhandDmg = 50, offhandDmg = 10,
            h2hSkillLevel = 200, hasFootwork = true, kickDmg = 80,
        })
        assert(almost(main, 105) and almost(off, 105))
    end)

    it('skill NONE matches H2H path', function()
        -- 70 + 50*0.11+3 = 78.5
        local main, off = w.meleeDmgFromParams({
            weaponType = xi.skill.NONE,
            kick = true, mainhandDmg = 1, offhandDmg = 2,
            h2hSkillLevel = 50, hasFootwork = true, kickDmg = 70,
        })
        assert(almost(main, 78.5) and almost(off, 78.5))

        main, off = w.meleeDmgFromParams({
            weaponType = xi.skill.NONE,
            kick = false, mainhandDmg = 20, offhandDmg = 99,
            h2hSkillLevel = 0,
        })
        assert(almost(main, 23) and almost(off, 23))
    end)
end)
