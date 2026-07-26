-- Host-level coverage for xi.job_utils.warrior.useBrazenRush.
require('scripts/globals/job_utils/warrior')

describe('Warrior Brazen Rush host', function()
    it('applies its fixed self-origin ticking effect', function()
        local applied
        local player = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.warrior.useBrazenRush(player, player, {}) == xi.effect.BRAZEN_RUSH)
        assert(applied.effect == xi.effect.BRAZEN_RUSH)
        assert(applied.params.power == 100 and applied.params.duration == 30)
        assert(applied.params.tick == 3 and applied.params.origin == player)
    end)
end)
