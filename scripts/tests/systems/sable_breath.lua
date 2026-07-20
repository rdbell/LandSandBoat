require('scripts/actions/mobskills/sable_breath')
describe('Sable Breath mob skill', function()
    it('requires front target and anim sub 0, applies conal adjustment', function()
        local breath = require('scripts/actions/mobskills/sable_breath')
        local move, process = xi.mobskills.mobBreathMove, xi.mobskills.processDamage
        local conal = utils.conalDamageAdjustment
        local inFront, animSub, params, damage, conalArgs = true, 0, nil, nil, nil
        local mob = { getAnimationSub = function() return animSub end }
        local target = {
            isInfront = function(_, m, angle) assert(angle == 128); return inFront end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        inFront = false; assert(breath.onMobSkillCheck(target, mob, {}) == 1)
        inFront, animSub = true, 1; assert(breath.onMobSkillCheck(target, mob, {}) == 1)
        inFront, animSub = true, 0; assert(breath.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.BREATH, damageType = xi.damageType.DARK }
        end
        utils.conalDamageAdjustment = function(m, t, s, dmg, factor)
            conalArgs = { dmg, factor }
            return dmg * factor
        end
        xi.mobskills.processDamage = function() return false end
        assert(breath.onMobWeaponSkill(mob, target, {}, {}) == 40)
        assert(params.percentMultipier == 0.20 and params.damageCap == 1400 and params.element == xi.element.DARK)
        assert(conalArgs[1] == 200 and conalArgs[2] == 0.2 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        breath.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage = move, process
        utils.conalDamageAdjustment = conal
        assert(damage[1] == 40)
    end)
end)
