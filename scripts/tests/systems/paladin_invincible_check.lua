require('scripts/globals/job_utils/paladin')

describe('Paladin Invincible check', function()
    it('adds job-point enmity and applies the one-hour recast reduction', function()
        local ve
        local recast
        local player = {
            getJobPointLevel = function(_, jobPoint)
                assert(jobPoint == xi.jp.INVINCIBLE_EFFECT)
                return 3
            end,
            getMod = function(_, mod)
                assert(mod == xi.mod.ONE_HOUR_RECAST)
                return 10
            end,
        }
        local ability = {
            getVE = function() return 50 end,
            setVE = function(_, value) ve = value end,
            getRecast = function() return 3600 end,
            setRecast = function(_, value) recast = value end,
        }

        local msg, status = xi.job_utils.paladin.checkInvincible(player, {}, ability)

        assert(ve == 350 and recast == 3000 and msg == 0 and status == 0)
    end)
end)
