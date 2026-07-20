require('scripts/actions/mobskills/beak_lunge')
describe('Beak Lunge mob skill', function()
    it('delegates Apkallu check and uses Piercing plan', function()
        local skill = require('scripts/actions/mobskills/beak_lunge')
        local canUse = xi.apkallu.canUseAbility
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage, threshold = nil, nil, nil
        xi.apkallu.canUseAbility = function(_, t) threshold = t; return 0 end
        local mob = { getWeaponDmg = function() return 45 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(threshold == 10)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.fTP[1] == 2.0 and params.damageType == xi.damageType.PIERCING and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        xi.apkallu.canUseAbility = canUse
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 80)
    end)
end)
