-----------------------------------
-- Pure system tests for doDrainingSpell full plan (slice 6120).
-- Source: scripts/globals/spells/absorb_spell.lua ~87–215
-----------------------------------

describe('doDrainingSpell pure plan', function()
    local MSG_MAGIC_NO_EFFECT = 75
    local MSG_MAGIC_RESIST = 85
    local MSG_NO_EFFECT = 283

    local function clamp(v, lo, hi)
        if v < lo then return lo end
        if v > hi then return hi end
        return v
    end

    local function shouldBoost(has, power, sub, overflow)
        if overflow <= 0 then return false end
        if not has then return true end
        return power == 0 and sub < overflow
    end

    local function use(p)
        if p.isUndead then
            return { returnDisplay = 0, msg = MSG_MAGIC_NO_EFFECT }
        end
        if p.absorbedOrNull then
            return { returnDisplay = 0, msg = MSG_MAGIC_RESIST }
        end
        if (p.targetPoints or 0) == 0 then
            return { returnDisplay = 0, msg = MSG_NO_EFFECT }
        end

        local dmg = p.finalDamage or 0
        local displayCap = p.displayCap or 0
        local r = {
            damage = dmg,
            takeSpellDamage = p.absorbsHP == true,
            addCasterHP = p.absorbsHP == true,
            addCasterMP = p.absorbsHP ~= true,
            delTargetMP = p.absorbsHP ~= true,
            setMsg = false,
        }

        if p.raisesMaxHP then
            displayCap = 9999 - (p.casterHP or 0)
            local overflow = dmg + (p.casterHP or 0) - (p.casterMaxHP or 0)
            if shouldBoost(p.hasMaxHPBoost, p.maxHPEffectPower or 0, p.maxHPEffectSubpower or 0, overflow) then
                r.applyMaxHPBoost = true
                r.maxHPBoostSubPower = overflow
                r.maxHPBoostDuration = 180 + 180 * (p.darkMagicDurationMod or 0) / 100
            end
        end

        r.returnDisplay = clamp(dmg, 0, displayCap)
        return r
    end

    it('undead MAGIC_NO_EFFECT', function()
        local r = use({ isUndead = true, targetPoints = 1000, finalDamage = 200 })
        assert(r.returnDisplay == 0 and r.msg == MSG_MAGIC_NO_EFFECT)
    end)

    it('absorb null MAGIC_RESIST', function()
        local r = use({ absorbedOrNull = true, targetPoints = 500, finalDamage = 100 })
        assert(r.msg == MSG_MAGIC_RESIST)
    end)

    it('zero target points NO_EFFECT', function()
        local r = use({ targetPoints = 0, finalDamage = 50 })
        assert(r.msg == MSG_NO_EFFECT)
    end)

    it('drain success clamps display and flags takeDamage', function()
        local r = use({
            absorbsHP = true, targetPoints = 2000, finalDamage = 400,
            displayCap = 300, casterHP = 700, casterMaxHP = 1000,
        })
        assert(r.takeSpellDamage == true and r.addCasterHP == true)
        assert(r.returnDisplay == 300 and r.damage == 400)
        assert(r.setMsg ~= true)
    end)

    it('aspir success transfers MP', function()
        local r = use({
            absorbsHP = false, targetPoints = 500, finalDamage = 120, displayCap = 80,
        })
        assert(r.addCasterMP == true and r.delTargetMP == true)
        assert(r.returnDisplay == 80 and r.takeSpellDamage ~= true)
    end)

    it('drain II overflow applies max HP boost', function()
        local r = use({
            absorbsHP = true, raisesMaxHP = true,
            targetPoints = 5000, finalDamage = 200,
            displayCap = 100, casterHP = 900, casterMaxHP = 1000,
            hasMaxHPBoost = false,
        })
        assert(r.applyMaxHPBoost == true and r.maxHPBoostSubPower == 100)
        assert(r.maxHPBoostDuration == 180)
        assert(r.returnDisplay == 200)
    end)

    it('percent max HP boost blocks overwrite', function()
        local r = use({
            absorbsHP = true, raisesMaxHP = true,
            targetPoints = 5000, finalDamage = 200,
            casterHP = 900, casterMaxHP = 1000,
            hasMaxHPBoost = true, maxHPEffectPower = 10,
        })
        assert(r.applyMaxHPBoost ~= true)
    end)
end)
