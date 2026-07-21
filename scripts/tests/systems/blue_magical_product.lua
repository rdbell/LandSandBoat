-----------------------------------
-- Pure system tests for blue magical post-FinalD product (slice 6128).
-- Source: scripts/globals/bluemagic.lua useMagicalSpell ~424–449
-----------------------------------

describe('blue magical post-FinalD product pure plan', function()
    local function product(p)
        local d = p.finalD or 0 -- pre-mitigation MagicalFinalD.FinalDamage
        d = math.floor(d * (p.resist or 1))
        d = math.floor(d * (p.staff or 1))
        d = math.floor(d * (p.sdt or 1))
        d = math.floor(d * (p.dayWeather or 1))
        d = math.floor(d * (p.steamJacket or 1))
        d = math.floor(d * (p.mab or 1))
        local consumeBA = false
        local triggerBurst = false
        if p.hasBurstAffinity or p.hasAzureLore then
            if (p.skillchainCount or 0) > 0 then
                d = math.floor(d * (p.ifMagicBurst or 1))
                d = math.floor(d * (p.ifMagicBurstBonus or 1))
                triggerBurst = true
            end
            if p.hasBurstAffinity then
                consumeBA = true
            end
        end
        d = math.floor(d * (p.ebullience or 1))
        d = math.floor(d * (p.bluePower or 1))
        return {
            damage = d,
            triggerBurst = triggerBurst,
            consumeBurstAffinity = consumeBA,
        }
    end

    it('floor chain without burst', function()
        -- 100 * 0.5 resist = 50; * 1.2 mab = 60; * 1.1 blue = 66
        local r = product({
            finalD = 100, resist = 0.5, mab = 1.2, bluePower = 1.1,
            staff = 1, sdt = 1, dayWeather = 1, steamJacket = 1, ebullience = 1,
        })
        assert(r.damage == 66 and r.triggerBurst ~= true and r.consumeBurstAffinity ~= true)
    end)

    it('burst path when BA and skillchain', function()
        local r = product({
            finalD = 100, resist = 1, staff = 1, sdt = 1, dayWeather = 1,
            steamJacket = 1, mab = 1, ebullience = 1, bluePower = 1,
            hasBurstAffinity = true, skillchainCount = 2,
            ifMagicBurst = 1.5, ifMagicBurstBonus = 1.1,
        })
        -- 100 * 1.5 = 150; * 1.1 = 165
        assert(r.damage == 165 and r.triggerBurst == true and r.consumeBurstAffinity == true)
    end)

    it('azure lore without SC does not burst but consumes BA if present', function()
        local r = product({
            finalD = 80, resist = 1, staff = 1, sdt = 1, dayWeather = 1,
            steamJacket = 1, mab = 1, ebullience = 1, bluePower = 1,
            hasAzureLore = true, hasBurstAffinity = true, skillchainCount = 0,
        })
        assert(r.damage == 80 and r.triggerBurst ~= true and r.consumeBurstAffinity == true)
    end)

    it('azure lore alone does not consume BA', function()
        local r = product({
            finalD = 50, resist = 1, staff = 1, sdt = 1, dayWeather = 1,
            steamJacket = 1, mab = 1, ebullience = 1, bluePower = 1,
            hasAzureLore = true, skillchainCount = 1,
            ifMagicBurst = 2, ifMagicBurstBonus = 1,
        })
        assert(r.damage == 100 and r.triggerBurst == true and r.consumeBurstAffinity ~= true)
    end)

    it('ebullience and blue power last', function()
        local r = product({
            finalD = 100, resist = 1, staff = 1, sdt = 1, dayWeather = 1,
            steamJacket = 1, mab = 1, ebullience = 1.25, bluePower = 0.8,
        })
        -- 100 * 1.25 = 125; * 0.8 = 100
        assert(r.damage == 100)
    end)
end)
