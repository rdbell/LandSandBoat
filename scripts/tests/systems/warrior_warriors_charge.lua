-- Host-level coverage for xi.job_utils.warrior.useWarriorsCharge.
require('scripts/globals/job_utils/warrior')

describe("Warrior's Charge host", function()
    it('applies merit-derived power as a self-origin fixed-duration effect', function()
        local applied
        local player = {
            getMerit = function()
                return 10
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.warrior.useWarriorsCharge(player, player, {}) == xi.effect.WARRIORS_CHARGE)
        assert(applied.effect == xi.effect.WARRIORS_CHARGE)
        assert(applied.params.power == 5 and applied.params.duration == 60)
        assert(applied.params.origin == player)
    end)
end)
