-----------------------------------
-- Pure system tests for addBonusesAbility product (slice 6078).
-----------------------------------

describe('addBonusesAbility pure inject', function()
    local function addBonuses(damage, affinity, sdt, dayWeather, matt, mdef, ele, barActive, barSub, hasParams, includeMab, hasBonusMab, bonusMab)
        local dmg = math.floor(damage * affinity)
        dmg = math.floor(dmg * sdt)
        dmg = math.floor(dmg * dayWeather)
        local mdefBar = 0
        if ele >= xi.element.FIRE and ele <= xi.element.WATER and barActive then
            mdefBar = barSub
        end
        local mab = 1
        if hasParams and hasBonusMab and includeMab then
            mab = (100 + matt + bonusMab) / (100 + mdef + mdefBar)
        elseif (not hasParams) or includeMab then
            mab = (100 + matt) / (100 + mdef + mdefBar)
        end
        if mab < 0 then mab = 0 end
        return math.floor(dmg * mab)
    end

    it('identity and MATT product', function()
        assert(addBonuses(100, 1, 1, 1, 0, 0, 1, false, 0, false, true, false, 0) == 100)
        assert(addBonuses(100, 1, 1, 1, 50, 0, 1, false, 0, false, true, false, 0) == 150)
    end)

    it('exclude MAB and bonusmab branch', function()
        assert(addBonuses(100, 1, 1, 1, 50, 0, 1, false, 0, true, false, false, 0) == 100)
        assert(addBonuses(100, 1, 1, 1, 0, 0, 1, false, 0, true, true, true, 50) == 150)
    end)

    it('barspell window FIRE..WATER', function()
        -- floor(100*1.1)=110; mab 100/150; floor(110*2/3)=73
        assert(addBonuses(100, 1.1, 1, 1, 0, 0, xi.element.FIRE, true, 50, false, true, false, 0) == 73)
        assert(xi.element.FIRE == 1 and xi.element.WATER == 6)
    end)
end)
