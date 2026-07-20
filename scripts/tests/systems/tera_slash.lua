require('scripts/actions/mobskills/tera_slash')
describe('Tera Slash mob skill', function()
    it('uses physical IGNORE_SHADOWS fTP 5.0', function()
        local skill = require('scripts/actions/mobskills/tera_slash')
        local params = nil
        local origP, origD = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 200, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.fTP[1] == 5.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
