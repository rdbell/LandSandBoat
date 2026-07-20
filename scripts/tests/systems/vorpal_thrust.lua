require('scripts/actions/mobskills/vorpal_thrust')
describe('Vorpal Thrust mob skill', function()
    it('uses piercing physical plan with TP-scaled crit chance', function()
        local skill = require('scripts/actions/mobskills/vorpal_thrust')
        local params = nil
        local origP, origD = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 80, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.damageType == xi.damageType.PIERCING and params.canCrit and params.criticalChance[3] == 0.9)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
