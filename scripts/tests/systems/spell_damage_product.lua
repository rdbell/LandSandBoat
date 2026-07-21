-----------------------------------
-- Pure system tests for useDamageSpell floor-product inject (slice 6092).
-- Mirrors the ordered floor-after-each-multiply chain in
-- scripts/globals/spells/damage_spell.lua useDamageSpell (~1139–1170).
-----------------------------------

describe('spell damage useDamageSpell product pure injects', function()
    ---@param p table inject fields (baseDamage + optional mults; nil mult = 1)
    ---@return number floored product (may be negative when absorbed)
    local function product(p)
        if p.nullified then
            return 0
        end

        local function m(name)
            local v = p[name]
            if v == nil then
                return 1
            end
            return v
        end

        local finalDamage = p.baseDamage or 0
        finalDamage = math.floor(finalDamage * m('mtdr'))
        finalDamage = math.floor(finalDamage * m('staff'))
        finalDamage = math.floor(finalDamage * m('affinity'))
        finalDamage = math.floor(finalDamage * m('resistTier'))
        finalDamage = math.floor(finalDamage * m('additionalResistTier'))
        finalDamage = math.floor(finalDamage * m('magicBurst'))
        finalDamage = math.floor(finalDamage * m('magicBurstBonus'))
        finalDamage = math.floor(finalDamage * m('dayAndWeather'))
        finalDamage = math.floor(finalDamage * m('magicBonusDiff'))
        finalDamage = math.floor(finalDamage * m('targetMagicDamageAdjustment'))
        finalDamage = math.floor(finalDamage * m('sdt'))
        finalDamage = math.floor(finalDamage * m('critical'))
        finalDamage = math.floor(finalDamage * m('divineSeal'))
        finalDamage = math.floor(finalDamage * m('divineEmblem'))
        finalDamage = math.floor(finalDamage * m('elementalSeal'))
        finalDamage = math.floor(finalDamage * m('ebullience'))
        finalDamage = math.floor(finalDamage * m('skillType'))
        finalDamage = math.floor(finalDamage * m('ninSkill'))
        finalDamage = math.floor(finalDamage * m('ninFutae'))
        finalDamage = math.floor(finalDamage * m('ninjutsu'))
        finalDamage = math.floor(finalDamage * m('undeadDivine'))
        finalDamage = math.floor(finalDamage * m('scarletDelirium'))
        finalDamage = math.floor(finalDamage * m('steamJacket'))
        finalDamage = math.floor(finalDamage * m('helixMerit'))
        finalDamage = math.floor(finalDamage * m('aoeResistance'))
        finalDamage = math.floor(finalDamage * m('actionType'))
        finalDamage = math.floor(finalDamage * m('absorb'))
        finalDamage = math.floor(finalDamage * m('nukeWall'))
        return finalDamage
    end

    it('nullified short-circuits to zero', function()
        assert(product({ baseDamage = 1000, nullified = true, magicBurst = 2 }) == 0)
    end)

    it('identity multipliers leave base floored', function()
        assert(product({ baseDamage = 100 }) == 100)
        assert(product({ baseDamage = 100.9 }) == 100) -- first floor only via mtdr=1
        -- base * 1 floors once: math.floor(100.9 * 1) == 100
        assert(product({ baseDamage = 50.4 }) == 50)
    end)

    it('floors after each multiply (order-sensitive)', function()
        -- floor(100 * 1.5) = 150; floor(150 * 1.5) = 225
        assert(product({ baseDamage = 100, mtdr = 1.5, staff = 1.5 }) == 225)
        -- vs naive floor(100 * 1.5 * 1.5) = floor(225) = 225 same here
        -- order-sensitive case: floor(10 * 1.4) = 14; floor(14 * 1.4) = 19
        -- naive floor(10 * 1.4 * 1.4) = floor(19.6) = 19 same
        -- stronger: floor(7 * 1.3) = 9; floor(9 * 1.3) = 11; floor(11 * 1.3) = 14
        -- naive floor(7 * 1.3^3) = floor(15.379) = 15  → product floors lower
        local got = product({
            baseDamage = 7,
            mtdr = 1.3,
            staff = 1.3,
            affinity = 1.3,
        })
        assert(got == 14, 'got ' .. tostring(got))
        assert(math.floor(7 * 1.3 * 1.3 * 1.3) == 15)
    end)

    it('zero inject zeroes product at that step', function()
        assert(product({ baseDamage = 500, resistTier = 0 }) == 0)
        assert(product({ baseDamage = 500, sdt = 0 }) == 0)
    end)

    it('absorb negative path keeps sign through floors', function()
        -- absorbFactor -1: floor(100 * -1) = -100
        assert(product({ baseDamage = 100, absorb = -1 }) == -100)
        -- floor(100 * 1.5) = 150; floor(150 * -1) = -150
        assert(product({ baseDamage = 100, mtdr = 1.5, absorb = -1 }) == -150)
    end)

    it('full chain golden with mixed injects', function()
        -- base 200
        -- * 0.8 mtdr → 160
        -- * 1.1 staff → 176
        -- * 1.2 affinity → 211
        -- * 0.5 resist → 105
        -- * 1.0 add resist → 105
        -- * 1.35 burst → 141
        -- * 1.1 burst bonus → 155
        -- * 1.1 day → 170
        -- * 1.2 mab → 204
        -- * 0.9 mdmg adj → 183
        -- * 1.0 sdt → 183
        -- * 1.25 crit → 228
        -- remaining identity
        -- * 1.0 absorb → 228
        -- * 0.8 nuke wall → 182
        local got = product({
            baseDamage = 200,
            mtdr = 0.8,
            staff = 1.1,
            affinity = 1.2,
            resistTier = 0.5,
            additionalResistTier = 1,
            magicBurst = 1.35,
            magicBurstBonus = 1.1,
            dayAndWeather = 1.1,
            magicBonusDiff = 1.2,
            targetMagicDamageAdjustment = 0.9,
            sdt = 1,
            critical = 1.25,
            nukeWall = 0.8,
        })
        assert(got == 182, 'got ' .. tostring(got))
    end)

    it('nuke wall applies after absorb in product order', function()
        -- floor(100 * -1) = -100; floor(-100 * 0.5) = -50
        assert(product({ baseDamage = 100, absorb = -1, nukeWall = 0.5 }) == -50)
    end)
end)
