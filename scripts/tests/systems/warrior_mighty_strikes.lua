require('scripts/globals/job_utils/warrior')

describe('Warrior Mighty Strikes host', function()
    it('applies its fixed self-origin effect', function()
        local applied
        local player = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.warrior.useMightyStrikes(player, player, {}) == xi.effect.MIGHTY_STRIKES)
        assert(applied.effect == xi.effect.MIGHTY_STRIKES)
        assert(applied.params.power == 1 and applied.params.duration == 45)
        assert(applied.params.origin == player)
    end)
end)
