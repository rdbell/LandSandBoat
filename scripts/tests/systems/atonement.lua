require('scripts/actions/mobskills/atonement')
describe('Atonement mob skill', function()
    it('uses breath plan with TP-scaled bonus damage', function()
        local skill = require('scripts/actions/mobskills/atonement')
        local breathMove = xi.mobskills.mobBreathMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local sk = { getTP = function() return 1000 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.BREATH, damageType = xi.damageType.ELEMENTAL }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 200)
        assert(params.percentMultipier == 0 and params.damageCap == 750 and params.bonusDamage == 100)
        assert(params.resistStat == xi.mod.INT and params.element == xi.element.NONE)
        assert(params.attackType == xi.attackType.BREATH and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 200)
        xi.mobskills.mobBreathMove = breathMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 200)
    end)
end)
