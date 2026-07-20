describe('Infernal Pestilence mob skill', function()
    it('uses its Water breath plan and applies Plague only after processing', function()
        local pestilence = require('scripts/actions/mobskills/infernal_pestilence')
        local move, process, effect = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, plague
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.WATER } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) plague = { ... } end
        assert(pestilence.onMobSkillCheck(target, {}, {}) == 0 and pestilence.onMobWeaponSkill({}, target, {}, {}) == 123)
        assert(params.percentMultipier == .05 and params.damageCap == 200 and params.bonusDamage == 0 and params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0 and params.resistStat == xi.mod.INT and params.element == xi.element.WATER and params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.WATER and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and plague == nil)
        xi.mobskills.processDamage = function() return true end
        pestilence.onMobWeaponSkill({}, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage[1] == 123 and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.WATER)
        assert(plague[1] == xi.effect.PLAGUE and plague[2] == 5 and plague[3] == 3 and plague[4] == 780)
    end)
end)
