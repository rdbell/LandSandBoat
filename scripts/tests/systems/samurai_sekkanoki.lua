require('scripts/globals/job_utils/samurai')

describe('Samurai Sekkanoki host', function()
    it('refreshes Sekkanoki on its target with the actor as origin', function()
        local calls = {}
        local player = {}
        local target = {
            delStatusEffect = function(_, effect)
                table.insert(calls, { operation = 'delete', effect = effect })
            end,
            addStatusEffect = function(_, effect, params)
                table.insert(calls, { operation = 'add', effect = effect, params = params })
            end,
        }

        assert(xi.job_utils.samurai.useSekkanoki(player, target, {}) == xi.effect.SEKKANOKI)
        assert(#calls == 2 and calls[1].operation == 'delete' and calls[1].effect == xi.effect.SEKKANOKI)
        assert(calls[2].operation == 'add' and calls[2].effect == xi.effect.SEKKANOKI)
        assert(calls[2].params.power == 1 and calls[2].params.duration == 60 and calls[2].params.origin == player)
    end)
end)
