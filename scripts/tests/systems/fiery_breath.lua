describe('Fiery Breath mob skill', function()
    it('enforces its gates and applies adjusted Light breath damage only after processing', function()
        local fieryBreath = require('scripts/actions/mobskills/fiery_breath')
        local breathMove, processDamage, conal = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, utils.conalDamageAdjustment
        local params, damage, adjustment = nil, nil, nil
        local mighty, front, animation = false, false, 0
        local mob = {
            hasStatusEffect = function() return mighty end,
            getAnimationSub = function() return animation end,
        }
        local target = {
            isInfront = function(_, source, angle) return front and source == mob and angle == 128 end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        utils.conalDamageAdjustment = function(_, _, _, value, multiplier)
            adjustment = { value, multiplier }
            return 246
        end
        mighty, front = true, true
        assert(fieryBreath.onMobSkillCheck(target, mob, {}) == 1)
        mighty, front, animation = false, false, 0
        assert(fieryBreath.onMobSkillCheck(target, mob, {}) == 1)
        front, animation = true, 1
        assert(fieryBreath.onMobSkillCheck(target, mob, {}) == 1)
        animation = 0
        assert(fieryBreath.onMobSkillCheck(target, mob, {}) == 0)
        assert(fieryBreath.onMobWeaponSkill(mob, target, {}, {}) == 246)
        assert(params.percentMultipier == 0.2 and params.damageCap == 1400 and params.bonusDamage == 0)
        assert(params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.LIGHT)
        assert(params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.LIGHT and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(adjustment[1] == 123 and adjustment[2] == 0.9 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(fieryBreath.onMobWeaponSkill(mob, target, {}, {}) == 246)
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, utils.conalDamageAdjustment = breathMove, processDamage, conal
        assert(damage[1] == 246 and damage[2] == mob and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.LIGHT)
    end)
end)
