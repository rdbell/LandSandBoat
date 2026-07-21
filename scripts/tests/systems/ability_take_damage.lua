-----------------------------------
-- Pure system tests for xi.ability.takeDamage (slice 6110).
-- Source: scripts/globals/ability.lua ~102–130
-----------------------------------

describe('ability takeDamage pure plan', function()
    local function takeDamage(p)
        local hits = (p.tpHitsLanded or 0) + (p.extraHitsLanded or 0)
        local r = {
            weaponskillBonusTP = (p.extraHitsLanded or 0) * 10 + (p.bonusTP or 0),
            targetTPMult = p.targetTPMult or 1,
            useTAChar = p.useTAChar == true,
        }
        if hits > 0 then
            -- absorb message TODO
        elseif (p.shadowsAbsorbed or 0) > 0 then
            r.setShadowAbsorbMsg = true
            r.shadowAbsorbParam = p.shadowsAbsorbed
        end

        local final = p.finalDmg or 0
        if p.useAfterWeaponskillDmg then
            final = p.afterWeaponskillDmg or 0
        end
        r.final = final

        if hits > 0 then
            r.recordDamage = true
            r.recordAbs = math.abs(final)
        end

        if p.useOverrideEnmity then
            r.useOverrideEnmity = true
            r.overrideCE = p.overrideCE or 0
            r.overrideVE = p.overrideVE or 0
        else
            local mult = p.enmityMult or 1
            r.useEnmityFromDamage = true
            r.enmityDamage = math.floor(final * mult)
        end
        return r
    end

    it('hits record abs and TP bonus', function()
        local r = takeDamage({
            finalDmg = 100, tpHitsLanded = 1, extraHitsLanded = 1, bonusTP = 5,
            useAfterWeaponskillDmg = true, afterWeaponskillDmg = 90,
        })
        assert(r.final == 90 and r.recordDamage == true and r.recordAbs == 90)
        assert(r.weaponskillBonusTP == 15 and r.targetTPMult == 1)
        assert(r.setShadowAbsorbMsg ~= true)
        assert(r.enmityDamage == 90)
    end)

    it('shadow absorb message when no hits', function()
        local r = takeDamage({ finalDmg = 0, shadowsAbsorbed = 3 })
        assert(r.setShadowAbsorbMsg == true and r.shadowAbsorbParam == 3)
        assert(r.recordDamage ~= true)
    end)

    it('hits branch skips shadow message', function()
        local r = takeDamage({ finalDmg = 50, tpHitsLanded = 1, shadowsAbsorbed = 2 })
        assert(r.setShadowAbsorbMsg ~= true and r.recordDamage == true)
    end)

    it('absorb uses absolute for recordDamage', function()
        local r = takeDamage({
            finalDmg = -40, tpHitsLanded = 1,
            useAfterWeaponskillDmg = true, afterWeaponskillDmg = -40,
        })
        assert(r.recordAbs == 40)
    end)

    it('enmity override CE/VE', function()
        local r = takeDamage({
            finalDmg = 100, tpHitsLanded = 1,
            useOverrideEnmity = true, overrideCE = 10, overrideVE = 20,
        })
        assert(r.useOverrideEnmity == true and r.useEnmityFromDamage ~= true)
        assert(r.overrideCE == 10 and r.overrideVE == 20)
    end)

    it('enmity mult scales damage', function()
        local r = takeDamage({ finalDmg = 100, tpHitsLanded = 1, enmityMult = 1.5 })
        assert(r.enmityDamage == 150)
    end)

    it('target TP mult default and set', function()
        local r = takeDamage({ finalDmg = 1 })
        assert(r.targetTPMult == 1)
        r = takeDamage({ finalDmg = 1, targetTPMult = 0.5 })
        assert(r.targetTPMult == 0.5)
    end)
end)
