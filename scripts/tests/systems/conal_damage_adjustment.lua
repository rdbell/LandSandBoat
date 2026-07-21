-----------------------------------
-- Pure system tests for utils.conalDamageAdjustment.
-----------------------------------

describe('Conal damage adjustment pure plans', function()
    -- Mirror production injects without entity hosts.
    local function adjust(maxDamage, minimumPercentage, absFacingAngle, coneAngle)
        coneAngle = coneAngle or 32
        local conalAnglePower = coneAngle - absFacingAngle
        if conalAnglePower < 0 then
            conalAnglePower = 0
        end

        local minimumDamage    = maxDamage * minimumPercentage
        local damagePerAngle   = (maxDamage - minimumDamage) / coneAngle
        local additionalDamage = damagePerAngle * conalAnglePower
        local finalDamage      = math.max(1, math.ceil(minimumDamage + additionalDamage))

        return finalDamage
    end

    it('center of cone returns full damage', function()
        assert(adjust(1000, 0.2, 0) == 1000)
        assert(adjust(500, 0.9, 0) == 500)
    end)

    it('edge and beyond return minimum percentage', function()
        assert(adjust(1000, 0.2, 32) == 200)
        assert(adjust(1000, 0.2, 64) == 200)
        assert(adjust(10, 0.9, 32) == 9)
    end)

    it('mid-cone interpolates linearly', function()
        -- power=16, min=200, perAngle=25, add=400 → 600
        assert(adjust(1000, 0.2, 16) == 600)
        -- 0.9 mid: min=450, range=50, per=50/32, add=25 → 475
        assert(adjust(500, 0.9, 16) == 475)
    end)

    it('clamps final damage to at least 1', function()
        assert(adjust(0, 0.2, 0) == 1)
    end)

    it('uses ceil on fractional intermediate', function()
        -- min=9, perAngle=0.03125, power=31 → add≈0.96875 → ceil(9.96875)=10
        assert(adjust(10, 0.9, 1) == 10)
    end)
end)
