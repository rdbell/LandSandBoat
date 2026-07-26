require('scripts/globals/job_utils/dragoon')

describe('Dragoon Spirit Link Stoneskin handling', function()
    local function runWithStoneskin(stoneskinPower)
        local damage
        local effect = {
            getPower = function() return stoneskinPower end,
            setPower = function(_, power) assert(power == stoneskinPower - 300) end,
        }
        local wyvern = {
            getTP = function() return 0 end,
            getHP = function() return 800 end,
            getMaxHP = function() return 1000 end,
            getID = function() return 202 end,
            getLocalVar = function() return 0 end,
            delStatusEffect = function() end,
            addStatusEffect = function() end,
            delTP = function() end,
            addHP = function() end,
        }
        local player = {
            getPet = function() return wyvern end,
            getHP = function() return 1000 end,
            getMerit = function() return 0 end,
            getMainLvl = function() return 75 end,
            getMod = function(_, mod)
                if mod == xi.mod.STONESKIN then
                    return stoneskinPower
                end
                return 0
            end,
            getJobPointLevel = function() return 0 end,
            hasStatusEffect = function(_, effectID) return effectID == xi.effect.STONESKIN end,
            getStatusEffect = function() return effect end,
            delMod = function(_, mod, amount)
                assert(mod == xi.mod.STONESKIN)
                assert(amount == 300)
            end,
            delStatusEffect = function(_, effectID)
                assert(effectID == xi.effect.STONESKIN)
            end,
            addTP = function() end,
            takeDamage = function(_, amount) damage = amount end,
            getEquipID = function() return 0 end,
            getID = function() return 101 end,
        }
        local action = { ID = function() end }
        local random = math.random
        math.random = function() return 30 end
        xi.job_utils.dragoon.useSpiritLink(player, {}, {}, action)
        math.random = random
        return damage
    end

    it('reduces a stronger Stoneskin and uses the original power in damage', function()
        assert(runWithStoneskin(400) == -100)
    end)

    it('removes Stoneskin at or below the drain and applies remaining damage', function()
        assert(runWithStoneskin(200) == 100)
    end)
end)
