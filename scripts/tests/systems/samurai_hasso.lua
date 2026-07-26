require('scripts/globals/job_utils/samurai')

describe('Samurai Hasso host', function()
    it('replaces both stances on the target with its target-calculated bonus', function()
        local removed = {}
        local applied
        local player = {}
        local target = {
            getMainJob = function()
                return xi.job.SAM
            end,
            getSubJob = function()
                return 0
            end,
            getMainLvl = function()
                return 75
            end,
            getSubLvl = function()
                return 0
            end,
            getJobPointLevel = function()
                return 5
            end,
            delStatusEffect = function(_, effect)
                table.insert(removed, effect)
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.samurai.useHasso(player, target, {}) == xi.effect.HASSO)
        assert(#removed == 2 and removed[1] == xi.effect.HASSO and removed[2] == xi.effect.SEIGAN)
        assert(applied.effect == xi.effect.HASSO)
        assert(math.abs(applied.params.power - (75 / 7 + 5)) < 0.000001)
        assert(applied.params.duration == 300 and applied.params.origin == player)
    end)

    it('does not replace stances when the target has no Samurai job', function()
        local calls = 0
        local target = {
            getMainJob = function()
                return 0
            end,
            getSubJob = function()
                return 0
            end,
            getMainLvl = function()
                return 99
            end,
            getSubLvl = function()
                return 99
            end,
            getJobPointLevel = function()
                return 10
            end,
            delStatusEffect = function()
                calls = calls + 1
            end,
            addStatusEffect = function()
                calls = calls + 1
            end,
        }

        assert(xi.job_utils.samurai.useHasso({}, target, {}) == xi.effect.HASSO)
        assert(calls == 0)
    end)
end)
