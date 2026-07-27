require('scripts/globals/job_utils/paladin')

describe('Paladin Shield Bash', function()
    it('applies deterministic shield, modifier, and job-point damage before host effects', function()
        local random = math.random
        local immune = xi.data.statusEffect.isTargetImmune
        local stoneskin = utils.handleStoneskin
        local damage, enmity, message
        math.random = function() return 1 end
        xi.data.statusEffect.isTargetImmune = function() return true end
        utils.handleStoneskin = function(_, amount) return amount end

        local player = {
            getShieldSize = function() return 3 end,
            getJobPointLevel = function(_, jobPoint)
                assert(jobPoint == xi.jp.SHIELD_BASH_EFFECT)
                return 2
            end,
            getMainLvl = function() return 75 end,
            getMainJob = function() return xi.job.PLD end,
            getMod = function(_, mod)
                assert(mod == xi.mod.SHIELD_BASH)
                return 5
            end,
        }
        local target = {
            takeDamage = function(_, amount) damage = amount end,
            updateEnmityFromDamage = function(_, _, amount) enmity = amount end,
        }
        local ability = { setMsg = function(_, value) message = value end }

        local result = xi.job_utils.paladin.useShieldBash(player, target, ability)

        math.random = random
        xi.data.statusEffect.isTargetImmune = immune
        utils.handleStoneskin = stoneskin
        assert(result == 85.85 and damage == 85.85 and enmity == 85.85 and message == xi.msg.basic.JA_DAMAGE)
    end)
end)
