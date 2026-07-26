require('scripts/globals/job_utils/samurai')

describe('Samurai Seigan host', function()
    it('replaces both stances on a two-handed target', function()
        local removed = {}
        local applied
        local player = {}
        local target = {
            isWeaponTwoHanded = function()
                return true
            end,
            delStatusEffect = function(_, effect)
                table.insert(removed, effect)
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.samurai.useSeigan(player, target, {}) == xi.effect.SEIGAN)
        assert(#removed == 2 and removed[1] == xi.effect.HASSO and removed[2] == xi.effect.SEIGAN)
        assert(applied.effect == xi.effect.SEIGAN)
        assert(applied.params.power == nil and applied.params.duration == 300 and applied.params.origin == player)
    end)

    it('does not replace stances on a target without a two-handed weapon', function()
        local calls = 0
        local target = {
            isWeaponTwoHanded = function()
                return false
            end,
            delStatusEffect = function()
                calls = calls + 1
            end,
            addStatusEffect = function()
                calls = calls + 1
            end,
        }

        assert(xi.job_utils.samurai.useSeigan({}, target, {}) == xi.effect.SEIGAN)
        assert(calls == 0)
    end)
end)
