-----------------------------------
-- Pure system tests for finalMagicNonSpellAdjustments full plan (slice 6111).
-- Source: scripts/globals/magic.lua ~83–108 (+ optional addBonusesAbility)
-----------------------------------

describe('finalMagicNonSpellAdjustments pure execute plan', function()
    local DAMAGE_TYPE_ELEMENTAL = 5

    local function clamp(dmg)
        if dmg < -99999 then return -99999 end
        if dmg > 99999 then return 99999 end
        return dmg
    end

    local function handlePhalanx(dmg, mod)
        if dmg <= 0 then return dmg end
        local out = dmg - (mod or 0)
        if out < 0 then return 0 end
        if out > 99999 then return 99999 end
        return out
    end

    local function adjust(p)
        local dmg = math.floor((p.base or 0) * (p.damageAdj or 1))
        dmg = math.floor(dmg * (p.absorption or 1))
        dmg = math.floor(dmg * (p.nullification or 1))
        if p.useAfterSevere then
            dmg = p.afterSevere or dmg
        end
        dmg = handlePhalanx(dmg, p.phalanxMod)
        -- ofa / stoneskin simplified for tests when provided as final injects
        if p.useStoneskin then
            dmg = p.stoneskin or dmg
        end
        return clamp(dmg)
    end

    local function execute(p)
        local base = p.base or 0
        if p.applyBonuses then
            -- identity bonuses for smoke
            base = math.floor(base * (p.affinity or 1))
            base = math.floor(base * (p.magicDef or 1))
            base = math.floor(base * (p.dayWeather or 1))
            local mab = 1
            if p.includeMAB ~= false then
                mab = (100 + (p.matt or 0)) / (100 + (p.mdef or 0))
            end
            if mab < 0 then mab = 0 end
            base = math.floor(base * mab)
        end
        p.base = base
        local intermediate = adjust(p)
        local r = { intermediate = intermediate, element = p.element or 0 }
        if intermediate < 0 then
            local healed = -intermediate
            if p.useAbsorbHealed then
                healed = p.absorbHealed or 0
            end
            r.healTarget = true
            r.healAmount = healed
            r.final = -healed
            return r
        end
        r.final = intermediate
        if intermediate > 0 then
            r.takeDamage = true
            r.actionDamageType = DAMAGE_TYPE_ELEMENTAL + r.element
        end
        return r
    end

    it('identity damage path', function()
        local r = execute({ base = 100, element = 1 })
        assert(r.final == 100 and r.takeDamage == true)
        assert(r.actionDamageType == DAMAGE_TYPE_ELEMENTAL + 1)
    end)

    it('absorb path signed return', function()
        local r = execute({ base = 50, absorption = -1 })
        assert(r.healTarget == true and r.healAmount == 50 and r.final == -50)
        assert(r.takeDamage ~= true)
    end)

    it('absorb uses host addHP amount', function()
        local r = execute({ base = 50, absorption = -1, useAbsorbHealed = true, absorbHealed = 30 })
        assert(r.healAmount == 30 and r.final == -30)
    end)

    it('phalanx reduces positive', function()
        local r = execute({ base = 100, phalanxMod = 25 })
        assert(r.final == 75)
    end)

    it('zero no takeDamage', function()
        local r = execute({ base = 0 })
        assert(r.final == 0 and r.takeDamage ~= true and r.healTarget ~= true)
    end)

    it('optional bonuses front-end identity', function()
        local r = execute({
            base = 80, applyBonuses = true,
            affinity = 1, magicDef = 1, dayWeather = 1, matt = 0, mdef = 0,
            element = 2,
        })
        assert(r.final == 80)
    end)
end)
