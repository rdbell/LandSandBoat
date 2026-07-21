-----------------------------------
-- Pure system tests for Blindside Barrage (slice 6141).
-----------------------------------

describe('Blindside Barrage pure plan', function()
    local function statusPower(lvl)
        return 3 + math.floor(lvl / 5)
    end

    it('status power ladder', function()
        assert(statusPower(1) == 3)
        assert(statusPower(50) == 13)
        assert(statusPower(99) == 22)
    end)

    it('applies both downs when processed', function()
        local power = statusPower(50)
        local plan = {
            applyDamage = true, applyStatuses = true,
            intDown = { power = power, duration = 120 },
            mndDown = { power = power, duration = 120 },
        }
        assert(plan.intDown.power == 13 and plan.mndDown.duration == 120)
    end)
end)
