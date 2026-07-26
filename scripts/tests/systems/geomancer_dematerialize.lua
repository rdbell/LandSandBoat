require('scripts/globals/job_utils/geomancer')

describe('Geomancer Dematerialize host', function()
    it('applies the luopan effect and routes the action when a luopan exists', function()
        local addedEffect
        local addedOptions
        local routed
        local luopan = {
            getPetID = function() return xi.petId.LUOPAN end,
            getID = function() return 42 end,
            addStatusEffect = function(_, effect, options)
                addedEffect = effect
                addedOptions = options
            end,
        }
        local player = {
            getPet = function() return luopan end,
            getID = function() return 7 end,
        }
        local action = {
            ID = function(_, sourceID, targetID) routed = { sourceID, targetID } end,
        }

        assert(xi.job_utils.geomancer.dematerialize(player, {}, {}, action) == xi.effect.DEMATERIALIZE)
        assert(addedEffect == xi.effect.DEMATERIALIZE)
        assert(addedOptions.duration == 60 and addedOptions.origin == player and addedOptions.tick == 3)
        assert(routed[1] == 7 and routed[2] == 42)
    end)

    it('returns its effect without mutating or routing when no luopan exists', function()
        local player = { getPet = function() return nil end }
        local action = { ID = function() error('must not route without a luopan') end }

        assert(xi.job_utils.geomancer.dematerialize(player, {}, {}, action) == xi.effect.DEMATERIALIZE)
    end)
end)
