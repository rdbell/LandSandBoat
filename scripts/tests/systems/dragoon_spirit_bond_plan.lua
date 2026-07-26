require('scripts/globals/job_utils/dragoon')

describe('Dragoon Spirit Bond plan', function()
    it('applies Spirit Bond to the player for 180 seconds', function()
        local player = {
            addStatusEffect = function(_, effect, params)
                assert(effect == xi.effect.SPIRIT_BOND)
                assert(params.duration == 180)
            end,
        }

        assert(xi.job_utils.dragoon.useSpiritBond(player, {}, {}) == xi.effect.SPIRIT_BOND)
    end)
end)
