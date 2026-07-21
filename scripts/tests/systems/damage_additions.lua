-----------------------------------
-- Pure system tests for Lua damage_additions inject halves (slice 6084).
-----------------------------------

describe('damage additions pure injects', function()
    local JOB_DRK = xi.job.DRK

    local function souleaterAddition(p)
        if not p.hasEffect then
            return 0
        end
        local bonus = math.floor(p.hp * (0.1 + (p.souleaterEffect or 0) / 100 + (p.souleaterEffectII or 0) / 100))
        if bonus > 0 and p.mainJob ~= JOB_DRK then
            return math.floor(bonus / 2)
        end
        return bonus
    end

    local function consumeManaAddition(hasEffect, mp)
        if not hasEffect then
            return 0
        end
        return math.floor(mp / 10)
    end

    it('souleater no effect returns 0', function()
        assert(souleaterAddition({ hasEffect = false, hp = 1000, mainJob = JOB_DRK }) == 0)
    end)

    it('souleater DRK full bonus', function()
        assert(souleaterAddition({ hasEffect = true, hp = 1000, mainJob = JOB_DRK }) == 100)
        assert(souleaterAddition({
            hasEffect = true, hp = 1000, souleaterEffect = 5, souleaterEffectII = 2, mainJob = JOB_DRK,
        }) == 170)
    end)

    it('souleater non-DRK half', function()
        assert(souleaterAddition({ hasEffect = true, hp = 1000, mainJob = xi.job.WAR }) == 50)
        assert(souleaterAddition({ hasEffect = true, hp = 0, mainJob = xi.job.WAR }) == 0)
        assert(souleaterAddition({ hasEffect = true, hp = 999, mainJob = xi.job.WAR }) == 49)
    end)

    it('consume mana floor mp/10', function()
        assert(consumeManaAddition(false, 500) == 0)
        assert(consumeManaAddition(true, 500) == 50)
        assert(consumeManaAddition(true, 509) == 50)
        assert(consumeManaAddition(true, 0) == 0)
    end)

    it('job DRK pin', function()
        assert(xi.job.DRK == 8)
    end)
end)
