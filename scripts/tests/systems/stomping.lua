require('scripts/actions/mobskills/stomping')
describe('Stomping mob skill', function()
    it('uses physical plan fTP 1.5 attackMultiplier 1.5', function()
        local skill = require('scripts/actions/mobskills/stomping')
        local params = nil
        local origP, origD = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 60, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 60)
        assert(params.fTP[1] == 1.5 and params.attackMultiplier[1] == 1.5)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
