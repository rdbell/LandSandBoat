require('scripts/globals/job_utils/dark_knight')

describe('Dark Knight Consume Mana host', function()
    it('adds the fixed self-origin Consume Mana request', function()
        local applied
        local player = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.dark_knight.useConsumeMana(player, {}, {}) == xi.effect.CONSUME_MANA)
        assert(applied.effect == xi.effect.CONSUME_MANA)
        assert(applied.params.power == 1 and applied.params.duration == 60 and applied.params.origin == player)
    end)
end)
