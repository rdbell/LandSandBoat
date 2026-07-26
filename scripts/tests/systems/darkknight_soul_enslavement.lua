require('scripts/globals/job_utils/dark_knight')

describe('Dark Knight Soul Enslavement host', function()
    it('adds the fixed self-origin Soul Enslavement request', function()
        local applied
        local player = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }

        assert(xi.job_utils.dark_knight.useSoulEnslavement(player, {}, {}) == xi.effect.SOUL_ENSLAVEMENT)
        assert(applied.effect == xi.effect.SOUL_ENSLAVEMENT)
        assert(applied.params.duration == 30 and applied.params.power == nil and applied.params.origin == player)
    end)
end)
