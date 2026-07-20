require('scripts/actions/mobskills/stellar_arrow')
describe('Stellar Arrow mob skill', function()
    it('uses physical piercing NUMSHADOWS_3 fTP 2.0', function()
        local skill = require('scripts/actions/mobskills/stellar_arrow')
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
        assert(params.fTP[1] == 2.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
