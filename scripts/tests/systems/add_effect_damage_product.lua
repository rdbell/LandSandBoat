-----------------------------------
-- Pure system tests for executeAddEffectDamage product inject (slice 6093).
-- Mirrors scripts/globals/combat/action_additional_effect_damage.lua
-- base power + floor-product + resist gate (~126–161).
-----------------------------------

describe('add effect damage product pure injects', function()
    local function baseDamage(basePower, actorStatMod, targetStatMod)
        return basePower + actorStatMod - targetStatMod
    end

    ---@param p table
    ---@return number damage (may be 0 when gated)
    local function product(p)
        if p.hasEnspell then
            return 0
        end
        if p.roll and p.chance and p.roll > p.chance then
            return 0
        end
        if p.limitUndead and p.targetUndead then
            return 0
        end

        local function m(name)
            local v = p[name]
            if v == nil then
                return 1
            end
            return v
        end

        local resist = m('resist')
        local lowest = p.lowestResist or 0.125
        if resist < lowest then
            return 0
        end

        local damage = p.base or 0
        damage = math.floor(damage * m('absorption'))
        damage = math.floor(damage * m('nullification'))
        damage = math.floor(damage * m('damageTypeSDT'))
        damage = math.floor(damage * m('physicalElementSDT'))
        damage = math.floor(damage * m('magicalElementSDT'))
        damage = math.floor(damage * m('staff'))
        damage = math.floor(damage * m('affinity'))
        damage = math.floor(damage * m('dayWeather'))
        damage = math.floor(damage * m('magicDiff'))
        damage = math.floor(damage * m('resist'))
        damage = math.floor(damage * m('forcedResistTier'))
        return damage
    end

    it('base power is basePower + actorStat - targetStat', function()
        assert(baseDamage(10, 50, 20) == 40)
        assert(baseDamage(0, 0, 5) == -5)
        assert(baseDamage(100, 0, 0) == 100)
    end)

    it('enspell / proc miss / undead limit early-out', function()
        assert(product({ base = 100, hasEnspell = true }) == 0)
        assert(product({ base = 100, chance = 50, roll = 51 }) == 0)
        assert(product({ base = 100, chance = 50, roll = 50 }) == 100)
        assert(product({ base = 100, limitUndead = true, targetUndead = true }) == 0)
        assert(product({ base = 100, limitUndead = true, targetUndead = false }) == 100)
    end)

    it('resist below lowestResist auto-fails', function()
        assert(product({ base = 100, resist = 0.1, lowestResist = 0.125 }) == 0)
        -- At the floor boundary the gate passes; resist still multiplies.
        assert(product({ base = 100, resist = 0.125, lowestResist = 0.125 }) == 12)
        assert(product({ base = 100, resist = 1.0, lowestResist = 0.125 }) == 100)
        assert(product({ base = 100, resist = 0.5 }) == 50) -- default lowest 0.125
    end)

    it('floors after each multiply', function()
        -- floor(7*1.3)=9; floor(9*1.3)=11; floor(11*1.3)=14
        local got = product({
            base = 7,
            absorption = 1.3,
            nullification = 1.3,
            damageTypeSDT = 1.3,
        })
        assert(got == 14, 'got ' .. tostring(got))
    end)

    it('absorb path keeps negative through floors', function()
        assert(product({ base = 80, absorption = -1 }) == -80)
        assert(product({ base = 80, absorption = -1, staff = 1.5 }) == -120)
    end)

    it('full chain golden', function()
        -- 50 * 1 * 1 * 0.9 = 45; *1.1 staff = 49; *1.2 aff = 58; *0.5 resist = 29
        local got = product({
            base = 50,
            damageTypeSDT = 0.9,
            staff = 1.1,
            affinity = 1.2,
            resist = 0.5,
        })
        assert(got == 29, 'got ' .. tostring(got))
    end)
end)
