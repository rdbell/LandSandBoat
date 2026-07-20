require('scripts/actions/mobskills/dragon_breath')

describe('Dragon Breath mob skill', function()
    it('requires a front target and usable animation, then applies adjusted Fire breath damage only after processing', function()
        local breath = require('scripts/actions/mobskills/dragon_breath')
        local breathMove, processDamage, conal = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, utils.conalDamageAdjustment
        local params, damage, adjustment = nil, nil, nil
        local front, animation = false, 0
        local mob = { getAnimationSub = function() return animation end }
        local target = {
            isInfront = function(_, source, angle) return front and source == mob and angle == 128 end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        utils.conalDamageAdjustment = function(_, _, _, value, multiplier)
            adjustment = { value, multiplier }
            return 246
        end

        assert(breath.onMobSkillCheck(target, mob, {}) == 1)
        front, animation = true, 1
        assert(breath.onMobSkillCheck(target, mob, {}) == 1)
        animation = 0
        assert(breath.onMobSkillCheck(target, mob, {}) == 0)
        assert(breath.onMobWeaponSkill(mob, target, {}, {}) == 246)
        assert(params.percentMultipier == 0.15 and params.damageCap == 1596 and params.bonusDamage == 0)
        assert(params.mAccuracyBonus[1] == 0 and params.mAccuracyBonus[2] == 0 and params.mAccuracyBonus[3] == 0)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.FIRE)
        assert(params.attackType == xi.attackType.BREATH and params.damageType == xi.damageType.FIRE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(adjustment[1] == 123 and adjustment[2] == 0.2 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(breath.onMobWeaponSkill(mob, target, {}, {}) == 246)
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, utils.conalDamageAdjustment = breathMove, processDamage, conal
        assert(damage[1] == 246 and damage[2] == mob and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.FIRE)
    end)
end)
