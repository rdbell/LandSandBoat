-----------------------------------
-- Pure system tests for blue useDrainSpell plan (slice 6124).
-- Source: scripts/globals/bluemagic.lua ~454–537
-----------------------------------

describe('blue useDrainSpell pure plan', function()
    local MSG_MAGIC_NO_EFFECT = 75

    local function clamp(v, lo, hi)
        if v < lo then return lo end
        if v > hi then return hi end
        return v
    end

    local function baseDamage(blueSkill, dmgMultiplier, damageCap)
        local d = math.floor(blueSkill * 0.11)
        d = math.floor(d * dmgMultiplier)
        if damageCap > 0 then
            d = clamp(d, 0, damageCap)
        end
        return d
    end

    -- Pre-mitigation product floor chain after base (simplified inject mults)
    local function product(base, m)
        local d = base
        for _, k in ipairs({ 'resist', 'staff', 'sdt', 'dayWeather', 'mab',
            'burst', 'burstBonus', 'ebullience', 'damageAdj', 'bluePower' }) do
            d = math.floor(d * (m[k] or 1))
        end
        return d
    end

    local function use(p)
        if p.absorbedOrNull or p.isUndead then
            return { returnDamage = 0, setMsg = true, msg = MSG_MAGIC_NO_EFFECT }
        end
        local dmg = p.finalDamage or 0
        if p.mpDrain then
            return {
                returnDamage = dmg,
                transferMP = true,
                takeSpellDamage = false,
            }
        end
        return {
            returnDamage = dmg,
            takeSpellDamage = true,
            addCasterHP = true,
            updateEnmity = not p.targetIsPC,
            handleAfflatus = true,
        }
    end

    it('undead or absorb/null sets MAGIC_NO_EFFECT', function()
        local r = use({ isUndead = true, finalDamage = 100 })
        assert(r.returnDamage == 0 and r.msg == MSG_MAGIC_NO_EFFECT)
        r = use({ absorbedOrNull = true, finalDamage = 100 })
        assert(r.msg == MSG_MAGIC_NO_EFFECT)
    end)

    it('base damage skill * 0.11 * mult with cap', function()
        -- skill 300 → floor(33) * 2 = 66; cap 50 → 50
        assert(baseDamage(300, 2, 50) == 50)
        -- no cap
        assert(baseDamage(300, 2, 0) == 66)
        -- skill 100 → floor(11)*1 = 11
        assert(baseDamage(100, 1, 0) == 11)
    end)

    it('product floor chain', function()
        assert(product(100, { resist = 0.5, bluePower = 1.2 }) == 60)
    end)

    it('mp drain transfers MP without takeSpellDamage', function()
        local r = use({ finalDamage = 40, mpDrain = true })
        assert(r.transferMP == true and r.takeSpellDamage ~= true)
        assert(r.returnDamage == 40)
    end)

    it('hp drain takeSpellDamage and heal caster', function()
        local r = use({ finalDamage = 200, mpDrain = false, targetIsPC = false })
        assert(r.takeSpellDamage == true and r.addCasterHP == true)
        assert(r.updateEnmity == true and r.handleAfflatus == true)
    end)

    it('hp drain on PC skips enmity update flag', function()
        local r = use({ finalDamage = 100, targetIsPC = true })
        assert(r.updateEnmity ~= true)
    end)
end)
