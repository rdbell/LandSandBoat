require('scripts/globals/job_utils/dragoon')

describe('Dragoon Spirit Link core plan', function()
    it('grants regen, transfers TP, drains the master, and heals the wyvern', function()
        local wyvern = {
            getTP = function() return 500 end,
            getHP = function() return 800 end,
            getMaxHP = function() return 1000 end,
            getID = function() return 202 end,
            getLocalVar = function() return 0 end,
            delStatusEffect = function() end,
            addStatusEffect = function(_, effect, params)
                assert(effect == xi.effect.REGEN)
                assert(params.power == 25)
                assert(params.duration == 90)
                assert(params.tick == 3)
            end,
            delTP = function(_, amount) assert(amount == 250) end,
            addHP = function(_, amount) assert(amount == 540) end,
        }
        local player = {
            getPet = function() return wyvern end,
            getHP = function() return 1000 end,
            getMerit = function() return 0 end,
            getMainLvl = function() return 75 end,
            getMod = function() return 0 end,
            getJobPointLevel = function(_, jp)
                if jp == xi.jp.SPIRIT_LINK_EFFECT then
                    return 10
                end
                return 0
            end,
            hasStatusEffect = function() return false end,
            addTP = function(_, amount) assert(amount == 250) end,
            takeDamage = function(_, amount) assert(amount == 270) end,
            getEquipID = function() return 0 end,
            getID = function() return 101 end,
        }
        local action = {
            ID = function(_, playerID, wyvernID)
                assert(playerID == 101)
                assert(wyvernID == 202)
            end,
        }

        local random = math.random
        math.random = function(low, high)
            assert(low == 25)
            assert(high == 35)
            return 30
        end
        xi.job_utils.dragoon.useSpiritLink(player, {}, {}, action)
        math.random = random
    end)
end)
