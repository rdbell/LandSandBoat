require('scripts/globals/job_utils/dragoon')

describe('Dragoon Steady Wing plan', function()
    it('applies protected tier-five stoneskin and reports the wyvern target', function()
        local effect = {
            delEffectFlag = function(_, flag) assert(flag == xi.effectFlag.DISPELABLE) end,
            setTier = function(_, tier) assert(tier == 5) end,
        }
        local wyvern = {
            getPetID = function() return xi.petId.WYVERN end,
            getMaxHP = function() return 1000 end,
            getHP = function() return 700 end,
            getID = function() return 202 end,
            addStatusEffect = function(_, status, params)
                assert(status == xi.effect.STONESKIN)
                assert(params.power == 600)
                assert(params.duration == 300)
                return true
            end,
            getStatusEffect = function(_, status)
                assert(status == xi.effect.STONESKIN)
                return effect
            end,
        }
        local player = {
            getPet = function() return wyvern end,
            getID = function() return 101 end,
        }
        local action = {
            ID = function(_, playerID, wyvernID)
                assert(playerID == 101)
                assert(wyvernID == 202)
            end,
        }

        xi.job_utils.dragoon.useSteadyWing(player, {}, {}, action)
    end)

    it('still reports the wyvern when stoneskin application fails', function()
        local wyvern = {
            getPetID = function() return xi.petId.WYVERN end,
            getMaxHP = function() return 1000 end,
            getHP = function() return 700 end,
            getID = function() return 202 end,
            addStatusEffect = function() return false end,
            getStatusEffect = function() error('failed stoneskin must not be adjusted') end,
        }
        local player = {
            getPet = function() return wyvern end,
            getID = function() return 101 end,
        }
        local action = {
            ID = function(_, playerID, wyvernID)
                assert(playerID == 101)
                assert(wyvernID == 202)
            end,
        }

        xi.job_utils.dragoon.useSteadyWing(player, {}, {}, action)
    end)
end)
