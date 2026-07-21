-----------------------------------
-- Pure system tests for regime addGovProwessBonusEffect pure plans (slice 6186).
-----------------------------------

describe('regime gov prowess pure plan', function()
    -- xi.effect pins (effect.codegen.lua)
    local PROWESS_CASKET_RATE   = 777
    local PROWESS_SKILL_RATE    = 778
    local PROWESS_CRYSTAL_YIELD = 779
    local PROWESS_TH            = 780
    local PROWESS_ATTACK_SPEED  = 781
    local PROWESS_HP_MP         = 782
    local PROWESS_ACC_RACC      = 783
    local PROWESS_ATT_RATT      = 784
    local PROWESS_MACC_MATK     = 785
    local PROWESS_CURE_POTENCY  = 786
    local PROWESS_WS_DMG        = 787
    local PROWESS_KILLER        = 788

    local prowessData =
    {
        { effect = PROWESS_CASKET_RATE,   basePower = 4,   addPower = 4,   maxStack = 5  },
        { effect = PROWESS_SKILL_RATE,    basePower = 4,   addPower = 4,   maxStack = 11 },
        { effect = PROWESS_CRYSTAL_YIELD, basePower = 4,   addPower = 4,   maxStack = 5  },
        { effect = PROWESS_TH,            basePower = 1,   addPower = 1,   maxStack = 3  },
        { effect = PROWESS_ATTACK_SPEED,  basePower = 400, addPower = 400, maxStack = 4  },
        { effect = PROWESS_HP_MP,         basePower = 3,   addPower = 1,   maxStack = 11 },
        { effect = PROWESS_ACC_RACC,      basePower = 4,   addPower = 4,   maxStack = 11 },
        { effect = PROWESS_ATT_RATT,      basePower = 4,   addPower = 4,   maxStack = 11 },
        { effect = PROWESS_MACC_MATK,     basePower = 4,   addPower = 4,   maxStack = 10 },
        { effect = PROWESS_CURE_POTENCY,  basePower = 4,   addPower = 4,   maxStack = 5  },
        { effect = PROWESS_WS_DMG,        basePower = 2,   addPower = 2,   maxStack = 5  },
        { effect = PROWESS_KILLER,        basePower = 4,   addPower = 4,   maxStack = 2  },
    }

    local function planMaxPower(p)
        return p.basePower + p.addPower * (p.maxStack - 1)
    end

    -- not e or e:getPower() < max → available
    local function planAvailable(p, hasEffect, power)
        if not hasEffect then
            return true
        end
        return power < planMaxPower(p)
    end

    local function planNextPower(p, hasEffect, power)
        if not hasEffect or power == 0 then
            return p.basePower
        end
        return power + p.addPower
    end

    -- messageBasic(p.effect - 168)
    local function planMessageID(effect)
        return effect - 168
    end

    -- filter available; pick by 1-based index into available list
    local function planPick(powers, pickIndex)
        local available = {}
        for i = 1, #prowessData do
            local p = prowessData[i]
            local power = powers[p.effect]
            local has = power ~= nil
            if planAvailable(p, has, power or 0) then
                table.insert(available, p)
            end
        end
        if #available == 0 then
            return nil
        end
        return available[pickIndex]
    end

    it('catalog size and first/last rows', function()
        assert(#prowessData == 12)
        assert(prowessData[1].effect == 777 and prowessData[1].maxStack == 5)
        assert(prowessData[12].effect == 788 and prowessData[12].maxStack == 2)
        assert(prowessData[5].basePower == 400) -- attack speed
        assert(prowessData[6].addPower == 1) -- HP/MP
    end)

    it('max power and available filter', function()
        local th = prowessData[4] -- TH: base1 add1 max3 → max power 3
        assert(planMaxPower(th) == 3)
        assert(planAvailable(th, false, 0))
        assert(planAvailable(th, true, 2))
        assert(not planAvailable(th, true, 3))
        local killer = prowessData[12] -- base4 add4 max2 → max 8
        assert(planMaxPower(killer) == 8)
        assert(not planAvailable(killer, true, 8))
    end)

    it('next power first vs stack', function()
        local p = prowessData[1]
        assert(planNextPower(p, false, 0) == 4)
        assert(planNextPower(p, true, 0) == 4)
        assert(planNextPower(p, true, 4) == 8)
        assert(planNextPower(p, true, 16) == 20)
    end)

    it('message id offset', function()
        assert(planMessageID(777) == 609)
        assert(planMessageID(788) == 620)
    end)

    it('picks from available only', function()
        -- all maxed except TH (index 4 in catalog → only available)
        local powers = {
            [777] = 20, -- max 4+4*4=20
            [778] = 44, -- max 4+4*10=44
            [779] = 20,
            -- TH absent
            [781] = 1600, -- 400+400*3
            [782] = 13, -- 3+1*10
            [783] = 44,
            [784] = 44,
            [785] = 40, -- 4+4*9
            [786] = 20,
            [787] = 10, -- 2+2*4
            [788] = 8,
        }
        local picked = planPick(powers, 1)
        assert(picked and picked.effect == PROWESS_TH)
        -- empty available
        powers[780] = 3
        assert(planPick(powers, 1) == nil)
    end)
end)
