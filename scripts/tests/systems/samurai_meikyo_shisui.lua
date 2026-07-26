require('scripts/globals/job_utils/samurai')

describe('Samurai Meikyo Shisui host', function()
    it('applies its fixed self-origin effect and grants TP', function()
        local applied
        local grantedTP = 0
        local player = {
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
            addTP = function(_, amount)
                grantedTP = grantedTP + amount
            end,
        }

        assert(xi.job_utils.samurai.useMeikyoShisui(player, player, {}) == 0)
        assert(applied.effect == xi.effect.MEIKYO_SHISUI)
        assert(applied.params.power == 1 and applied.params.duration == 30)
        assert(applied.params.origin == player)
        assert(grantedTP == 3000)
    end)
end)
