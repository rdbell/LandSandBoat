require('scripts/globals/job_utils/ninja')

describe('Ninja Yonin', function()
    it('clears Innin and Yonin before applying Yonin to the target', function()
        local calls = {}
        local player = {}
        local target = {
            delStatusEffect = function(_, id) table.insert(calls, { op = 'del', id = id }) end,
            addStatusEffect = function(_, id, values) table.insert(calls, { op = 'add', id = id, values = values }) end,
        }

        local result = xi.job_utils.ninja.useYonin(player, target, {}, {})

        assert(result == xi.effect.YONIN)
        assert(calls[1].op == 'del' and calls[1].id == xi.effect.INNIN)
        assert(calls[2].op == 'del' and calls[2].id == xi.effect.YONIN)
        assert(calls[3].op == 'add' and calls[3].id == xi.effect.YONIN)
        assert(calls[3].values.power == 30 and calls[3].values.duration == 300 and calls[3].values.tick == 15)
        assert(calls[3].values.origin == player)
    end)
end)
