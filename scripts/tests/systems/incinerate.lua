describe('Incinerate mob skill', function()
    it('uses its Fire breath plan, raises the Energetic Eruca cap, and damages only after processing', function()
        local incinerate = require('scripts/actions/mobskills/incinerate')
        local move, process = xi.mobskills.mobBreathMove, xi.mobskills.processDamage
        local params, damage
        local mob = { getPool = function() return 0 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.FIRE } end
        xi.mobskills.processDamage = function() return false end
        assert(incinerate.onMobSkillCheck(target, mob, {}) == 0 and incinerate.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.percentMultipier == .1375 and params.damageCap == 700 and params.bonusDamage == 0 and params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.FIRE and params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)
        mob.getPool = function() return xi.mobPool.ENERGETIC_ERUCA end
        incinerate.onMobWeaponSkill(mob, target, {}, {})
        assert(params.damageCap == 800)
        xi.mobskills.processDamage = function() return true end
        incinerate.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.FIRE)
    end)
end)
