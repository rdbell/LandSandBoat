require('scripts/globals/job_utils/dark_knight')

describe('Dark Knight Weapon Bash host', function()
    it('applies damage, enmity, resolved Stun, and animation', function()
        local immune = xi.data.statusEffect.isTargetImmune
        local resistant = xi.data.statusEffect.isTargetResistant
        local nullified = xi.data.statusEffect.isEffectNullified
        local resistRate = xi.combat.magicHitRate.calculateResistRate
        local resistSuccess = xi.data.statusEffect.isResistRateSuccessfull
        local random = math.random
        local damage
        local enmity
        local applied
        local animation
        local player = {
            getMainJob = function() return xi.job.DRK end,
            getMainLvl = function() return 75 end,
            getSubJob = function() return xi.job.WAR end,
            getSubLvl = function() return 37 end,
            getMod = function(_, mod)
                assert(mod == xi.mod.WEAPON_BASH)
                return 10
            end,
            getWeaponSkillType = function(_, slot)
                assert(slot == xi.slot.MAIN)
                return xi.skill.POLEARM
            end,
        }
        local target = {
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.WEAPON_BASH_EFFECT)
                return 2
            end,
            takeDamage = function(_, amount, source, attackType, damageType)
                damage = { amount = amount, source = source, attackType = attackType, damageType = damageType }
            end,
            updateEnmityFromDamage = function(_, source, amount)
                enmity = { source = source, amount = amount }
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
            getID = function() return 42 end,
        }
        local action = {
            setAnimation = function(_, targetID, value)
                animation = { targetID = targetID, value = value }
            end,
        }

        xi.data.statusEffect.isTargetImmune = function() return false end
        xi.data.statusEffect.isTargetResistant = function() return false end
        xi.data.statusEffect.isEffectNullified = function() return false end
        xi.combat.magicHitRate.calculateResistRate = function() return 0.5 end
        xi.data.statusEffect.isResistRateSuccessfull = function() return true end
        math.random = function(low, high)
            assert(low == 2 and high == 8)
            return 8
        end

        assert(xi.job_utils.dark_knight.useWeaponBash(player, target, {}, action) == 51)

        xi.data.statusEffect.isTargetImmune = immune
        xi.data.statusEffect.isTargetResistant = resistant
        xi.data.statusEffect.isEffectNullified = nullified
        xi.combat.magicHitRate.calculateResistRate = resistRate
        xi.data.statusEffect.isResistRateSuccessfull = resistSuccess
        math.random = random

        assert(damage.amount == 51 and damage.source == player and damage.attackType == xi.attackType.PHYSICAL and damage.damageType == xi.damageType.BLUNT)
        assert(enmity.source == player and enmity.amount == 51)
        assert(applied.effect == xi.effect.STUN and applied.params.power == 1 and applied.params.duration == 4 and applied.params.origin == player)
        assert(animation.targetID == 42 and animation.value == 203)
    end)

    it('skips Stun when the target is immune but still damages and animates', function()
        local immune = xi.data.statusEffect.isTargetImmune
        local resistRate = xi.combat.magicHitRate.calculateResistRate
        local damage
        local animation
        local player = {
            getMainJob = function() return xi.job.DRK end,
            getMainLvl = function() return 99 end,
            getSubJob = function() return xi.job.WAR end,
            getSubLvl = function() return 49 end,
            getMod = function() return 0 end,
            getWeaponSkillType = function() return xi.skill.GREAT_SWORD end,
        }
        local target = {
            getJobPointLevel = function() return 0 end,
            takeDamage = function(_, amount) damage = amount end,
            updateEnmityFromDamage = function() end,
            addStatusEffect = function() error('immune Stun must not be added') end,
            getID = function() return 7 end,
        }
        local action = {
            setAnimation = function(_, _, value) animation = value end,
        }

        xi.data.statusEffect.isTargetImmune = function() return true end
        xi.combat.magicHitRate.calculateResistRate = function()
            error('immune Stun must not calculate resistance')
        end

        assert(xi.job_utils.dark_knight.useWeaponBash(player, target, {}, action) == 27)

        xi.data.statusEffect.isTargetImmune = immune
        xi.combat.magicHitRate.calculateResistRate = resistRate

        assert(damage == 27 and animation == 201)
    end)
end)
