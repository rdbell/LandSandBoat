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

    it('applies resolved Stun with random duration scaled by resistance', function()
        local immune = xi.data.statusEffect.isTargetImmune
        local resistant = xi.data.statusEffect.isTargetResistant
        local nullified = xi.data.statusEffect.isEffectNullified
        local resistRate = xi.combat.magicHitRate.calculateResistRate
        local resistSuccess = xi.data.statusEffect.isResistRateSuccessfull
        local random = math.random
        local stoneskin = utils.handleStoneskin
        local applied
        xi.data.statusEffect.isTargetImmune = function() return false end
        xi.data.statusEffect.isTargetResistant = function() return false end
        xi.data.statusEffect.isEffectNullified = function() return false end
        xi.combat.magicHitRate.calculateResistRate = function() return 0.5 end
        xi.data.statusEffect.isResistRateSuccessfull = function() return true end
        math.random = function(low, high)
            if low == 2 and high == 8 then return 8 end
            assert(low == 1 and high == 5)
            return 1
        end
        utils.handleStoneskin = function(_, amount) return amount end

        local player = {
            getShieldSize = function() return 1 end,
            getJobPointLevel = function() return 0 end,
            getMainLvl = function() return 75 end,
            getMainJob = function() return xi.job.PLD end,
            getMod = function() return 0 end,
        }
        local target = {
            addStatusEffect = function(_, effect, params) applied = { effect = effect, params = params } end,
            takeDamage = function() end,
            updateEnmityFromDamage = function() end,
        }

        xi.job_utils.paladin.useShieldBash(player, target, { setMsg = function() end })

        xi.data.statusEffect.isTargetImmune = immune
        xi.data.statusEffect.isTargetResistant = resistant
        xi.data.statusEffect.isEffectNullified = nullified
        xi.combat.magicHitRate.calculateResistRate = resistRate
        xi.data.statusEffect.isResistRateSuccessfull = resistSuccess
        math.random = random
        utils.handleStoneskin = stoneskin
        assert(applied.effect == xi.effect.STUN and applied.params.power == 1 and applied.params.duration == 4 and applied.params.origin == player)
    end)
end)
