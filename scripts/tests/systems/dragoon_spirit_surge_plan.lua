require('scripts/globals/job_utils/dragoon')

describe('Dragoon Spirit Surge plan', function()
    it('transfers wyvern state, resets jumps, and applies Spirit Surge', function()
        local wyvern = {
            getTP = function() return 750 end,
            getHP = function() return 300 end,
            getMaxHP = function() return 1000 end,
            getMainLvl = function() return 80 end,
            delTP = function(_, amount) assert(amount == 750) end,
        }
        local resetRecasts = {}
        local player = {
            getPet = function() return wyvern end,
            addTP = function(_, amount) assert(amount == 750) end,
            despawnPet = function() end,
            resetRecast = function(_, recastType, recastID)
                assert(recastType == xi.recast.ABILITY)
                table.insert(resetRecasts, recastID)
            end,
            addStatusEffect = function(_, effect, params)
                assert(effect == xi.effect.SPIRIT_SURGE)
                assert(params.power == 250)
                assert(params.duration == 60)
                assert(params.subPower == 17)
            end,
            addHP = function(_, amount) assert(amount == 300) end,
        }

        xi.job_utils.dragoon.useSpiritSurge(player, player, {})
        assert(#resetRecasts == 3)
        assert(resetRecasts[1] == 158)
        assert(resetRecasts[2] == 159)
        assert(resetRecasts[3] == 160)
    end)
end)
