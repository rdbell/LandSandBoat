-- Host-level coverage for xi.job_utils.warrior.useTomahawk.
require('scripts/globals/job_utils/warrior')

describe('Warrior Tomahawk host', function()
    it('applies the target effect fields and consumes one ammo', function()
        local applied
        local removedAmmo
        local player = {
            getMerit = function()
                return 25
            end,
            removeAmmo = function(_, count)
                removedAmmo = count
            end,
        }
        local target = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        xi.job_utils.warrior.useTomahawk(player, target, {})

        assert(applied.effect == xi.effect.TOMAHAWK)
        assert(applied.params.power == 25 and applied.params.duration == 40)
        assert(applied.params.tick == 3 and applied.params.icon == 0)
        assert(applied.params.origin == player)
        assert(removedAmmo == 1)
    end)
end)
