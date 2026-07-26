require('scripts/globals/job_utils/dragoon')

describe('Dragoon Deep Breathing plan', function()
    it('only grants Magic Attack Boost to a wyvern', function()
        local pet
        local player = { getPet = function() return pet end }

        xi.job_utils.dragoon.useDeepBreathing(player, {}, {}, {})

        pet = {
            getPetID = function() return xi.petId.LUOPAN end,
            addStatusEffect = function() error('non-wyvern must not receive effect') end,
        }
        xi.job_utils.dragoon.useDeepBreathing(player, {}, {}, {})

        pet = {
            getPetID = function() return xi.petId.WYVERN end,
            addStatusEffect = function(_, effect, params)
                assert(effect == xi.effect.MAGIC_ATK_BOOST)
                assert(params.duration == 180)
                assert(params.origin == player)
            end,
        }
        xi.job_utils.dragoon.useDeepBreathing(player, {}, {}, {})
    end)
end)
