require('scripts/globals/job_utils/warrior')

describe('Warrior Restraint host', function()
    it('applies its fixed self-origin effect without an explicit power', function()
        local applied
        local player = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.warrior.useRestraint(player, player, {}) == xi.effect.RESTRAINT)
        assert(applied.effect == xi.effect.RESTRAINT)
        assert(applied.params.power == nil and applied.params.duration == 300)
        assert(applied.params.origin == player)
    end)
end)
