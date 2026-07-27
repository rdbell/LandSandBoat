require('scripts/globals/job_utils/paladin')

describe('Paladin Intervene check', function()
    it('requires a shield without changing ability recast', function()
        local recastChanged = false
        local player = { getShieldSize = function() return 0 end }
        local ability = { setRecast = function() recastChanged = true end }

        local msg, status = xi.job_utils.paladin.checkIntervene(player, {}, ability)

        assert(msg == xi.msg.basic.REQUIRES_SHIELD and status == 0 and not recastChanged)
    end)

    it('applies the one-hour recast reduction when a shield is equipped', function()
        local recast
        local player = {
            getShieldSize = function() return 1 end,
            getMod = function(_, mod)
                assert(mod == xi.mod.ONE_HOUR_RECAST)
                return 10
            end,
        }
        local ability = {
            getRecast = function() return 3600 end,
            setRecast = function(_, value) recast = value end,
        }

        local msg, status = xi.job_utils.paladin.checkIntervene(player, {}, ability)

        assert(msg == 0 and status == 0 and recast == 3000)
    end)
end)
