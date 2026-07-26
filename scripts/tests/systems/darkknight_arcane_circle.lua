require('scripts/globals/job_utils/dark_knight')

describe('Dark Knight Arcane Circle host', function()
    it('adds the target Arcane Circle request from actor job and modifiers', function()
        local applied
        local player = {
            getMainJob = function()
                return xi.job.DRK
            end,
            getMod = function(_, mod)
                return mod == xi.mod.ARCANE_CIRCLE_POTENCY and 3 or 45
            end,
        }
        local target = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.dark_knight.useArcaneCircle(player, target, {}) == xi.effect.ARCANE_CIRCLE)
        assert(applied.effect == xi.effect.ARCANE_CIRCLE)
        assert(applied.params.power == 18 and applied.params.duration == 225 and applied.params.origin == player)
    end)
end)
