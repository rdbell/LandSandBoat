require('scripts/globals/job_utils/samurai')

describe('Samurai Hagakure host', function()
    it('refreshes the self-origin effect with its sub-power', function()
        local calls = {}
        local player = {
            delStatusEffect = function(_, effect)
                table.insert(calls, { operation = 'delete', effect = effect })
            end,
            addStatusEffect = function(_, effect, params)
                table.insert(calls, { operation = 'add', effect = effect, params = params })
            end,
        }

        assert(xi.job_utils.samurai.useHagakure(player, {}, {}) == xi.effect.HAGAKURE)
        assert(#calls == 2 and calls[1].operation == 'delete' and calls[1].effect == xi.effect.HAGAKURE)
        assert(calls[2].operation == 'add' and calls[2].effect == xi.effect.HAGAKURE)
        assert(calls[2].params.power == 400 and calls[2].params.duration == 60)
        assert(calls[2].params.origin == player and calls[2].params.subPower == 1000)
    end)
end)
