require('scripts/globals/job_utils/warrior')

describe('Warrior Retaliation host', function()
    it('applies its fixed self-origin effect', function()
        local applied
        local player = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.warrior.useRetaliation(player, player, {}) == xi.effect.RETALIATION)
        assert(applied.effect == xi.effect.RETALIATION)
        assert(applied.params.power == 1 and applied.params.duration == 180)
        assert(applied.params.origin == player)
    end)
end)
