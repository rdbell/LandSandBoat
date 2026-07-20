describe('Fire Break mob skill', function()
    it('uses its Fire breath plan and damages only after processing', function()
        local fireBreak = require('scripts/actions/mobskills/fire_break')
        local breathMove, processDamage = xi.mobskills.mobBreathMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = {}
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(fireBreak.onMobSkillCheck(target, mob, {}) == 0)
        assert(fireBreak.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.percentMultipier == 0.123 and params.damageCap == 700 and params.bonusDamage == 0)
        assert(params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.FIRE and params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(fireBreak.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage = breathMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.FIRE)
    end)
end)
