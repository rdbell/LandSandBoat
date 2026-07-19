require('scripts/actions/mobskills/dark_spore')

describe('Dark Spore mob skill', function()
    it('uses fixed Dark breath parameters and applies damage and Blindness only after successful processing', function()
        local darkSpore = require('scripts/actions/mobskills/dark_spore')
        local originalBreathMove = xi.mobskills.mobBreathMove
        local originalProcessDamage = xi.mobskills.processDamage
        local originalStatusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effect = nil, nil, nil
        local mob = {}
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(source, recipient, effectID, power, tick, duration)
            effect = { source, recipient, effectID, power, tick, duration }
        end

        assert(darkSpore.onMobSkillCheck(target, mob, skill) == 0)
        assert(darkSpore.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.percentMultipier == 0.25 and params.damageCap == 600 and params.bonusDamage == 0)
        assert(params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.DARK)
        assert(params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil and effect == nil)

        xi.mobskills.processDamage = function() return true end
        assert(darkSpore.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobBreathMove = originalBreathMove
        xi.mobskills.processDamage = originalProcessDamage
        xi.mobskills.mobStatusEffectMove = originalStatusEffectMove

        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.DARK)
        assert(effect[1] == mob and effect[2] == target and effect[3] == xi.effect.BLINDNESS)
        assert(effect[4] == 30 and effect[5] == 0 and effect[6] == 90)
    end)
end)
