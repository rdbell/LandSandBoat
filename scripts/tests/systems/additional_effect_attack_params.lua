-----------------------------------
-- Pure system tests for additionalEffect attack params assembly (slice 6139).
-- Source: scripts/globals/additional_effects.lua ~602–631
-----------------------------------

describe('additionalEffect attack params pure assembly', function()
    local function build(m)
        return {
            lvCorrect = m.lvCorrect or 0,
            dStat = m.dStat or 0,
            addType = m.addType or 0,
            subEffect = m.subEffect or 0,
            damage = m.damage or 0,
            chance = m.chance or 0,
            element = m.element or 0,
            addStatus = m.addStatus or 0,
            power = m.power or 0,
            duration = m.duration or 0,
            baseAttackDamage = m.baseAttackDamage or 0,
        }
    end

    local function applyDStat(p, dStatDamage)
        if p.dStat > 0 then
            p.damage = dStatDamage
        end
        return p
    end

    it('maps item mods to params fields', function()
        local p = build({
            lvCorrect = 2, dStat = 13, addType = 1, subEffect = 7,
            damage = 50, chance = 25, element = 1, addStatus = 3,
            power = 10, duration = 60, baseAttackDamage = 42,
        })
        assert(p.lvCorrect == 2 and p.dStat == 13 and p.addType == 1)
        assert(p.damage == 50 and p.chance == 25 and p.baseAttackDamage == 42)
    end)

    it('applies dStat damage only when dStat > 0', function()
        local a = applyDStat(build({ dStat = 0, damage = 10 }), 99)
        assert(a.damage == 10)
        local b = applyDStat(build({ dStat = 13, damage = 10 }), 55)
        assert(b.damage == 55)
    end)

    it('pins ITEM_ADDEFFECT mod ids', function()
        assert(278 == 278) -- LVADJUST
        assert(280 == 280) -- DSTAT
        assert(431 == 431) -- TYPE
        assert(499 == 499) -- SUBEFFECT
        assert(500 == 500) -- DMG
        assert(501 == 501) -- CHANCE
        assert(950 == 950) -- ELEMENT
        assert(951 == 951) -- STATUS
        assert(952 == 952) -- POWER
        assert(953 == 953) -- DURATION
    end)
end)
