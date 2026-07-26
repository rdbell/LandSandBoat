require('scripts/globals/job_utils/samurai')

describe('Samurai Blade Bash host', function()
    it('applies resolved Stun and Plague requests, then publishes animation and damage message', function()
        local immune = xi.data.statusEffect.isTargetImmune
        local resistant = xi.data.statusEffect.isTargetResistant
        local nullified = xi.data.statusEffect.isEffectNullified
        local resistRate = xi.combat.magicHitRate.calculateResistRate
        local applied = {}
        local animation
        local msg
        local player = {
            getMerit = function()
                return 15
            end,
            getWeaponSkillType = function()
                return xi.skill.POLEARM
            end,
        }
        local target = {
            getID = function()
                return 42
            end,
            addStatusEffect = function(_, effect, params)
                table.insert(applied, { effect = effect, params = params })
            end,
        }
        local ability = {
            setMsg = function(_, value)
                msg = value
            end,
        }
        local action = {
            setAnimation = function(_, targetID, value)
                animation = { targetID = targetID, value = value }
            end,
        }

        xi.data.statusEffect.isTargetImmune = function()
            return false
        end
        xi.data.statusEffect.isTargetResistant = function()
            return false
        end
        xi.data.statusEffect.isEffectNullified = function()
            return false
        end
        xi.combat.magicHitRate.calculateResistRate = function(_, _, _, _, _, _, _, effect)
            return effect == xi.effect.STUN and 0.5 or 0.25
        end

        assert(xi.job_utils.samurai.useBladeBash(player, target, ability, action) == 0)

        xi.data.statusEffect.isTargetImmune = immune
        xi.data.statusEffect.isTargetResistant = resistant
        xi.data.statusEffect.isEffectNullified = nullified
        xi.combat.magicHitRate.calculateResistRate = resistRate

        assert(#applied == 2)
        assert(applied[1].effect == xi.effect.STUN and applied[1].params.power == 1 and applied[1].params.duration == 3 and applied[1].params.origin == player)
        assert(applied[2].effect == xi.effect.PLAGUE and applied[2].params.power == 5 and applied[2].params.duration == 7.5 and applied[2].params.origin == player)
        assert(animation.targetID == 42 and animation.value == 203 and msg == xi.msg.basic.JA_DAMAGE)
    end)

    it('skips immune effects while still publishing animation and damage message', function()
        local immune = xi.data.statusEffect.isTargetImmune
        local calculate = xi.combat.magicHitRate.calculateResistRate
        local animation
        local msg
        local player = {
            getMerit = function()
                return 0
            end,
            getWeaponSkillType = function()
                return xi.skill.GREAT_KATANA
            end,
        }
        local target = {
            getID = function()
                return 42
            end,
            addStatusEffect = function()
                error('immune effects must not be added')
            end,
        }
        local ability = {
            setMsg = function(_, value)
                msg = value
            end,
        }
        local action = {
            setAnimation = function(_, _, value)
                animation = value
            end,
        }

        xi.data.statusEffect.isTargetImmune = function()
            return true
        end
        xi.combat.magicHitRate.calculateResistRate = function()
            error('immune effects must not calculate resistance')
        end

        assert(xi.job_utils.samurai.useBladeBash(player, target, ability, action) == 0)

        xi.data.statusEffect.isTargetImmune = immune
        xi.combat.magicHitRate.calculateResistRate = calculate

        assert(animation == 201 and msg == xi.msg.basic.JA_DAMAGE)
    end)
end)
