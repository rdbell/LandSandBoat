require('scripts/actions/mobskills/tenzen_ranged_low')
describe('Tenzen Ranged Low mob skill', function()
    it('uses ranged piercing plan fTP 1.5 with RANGED_ATTACK_HIT', function()
        local skill = require('scripts/actions/mobskills/tenzen_ranged_low')
        local params = nil
        local origR, origD = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        xi.mobskills.mobRangedMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 60, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 60)
        assert(params.fTP[1] == 1.5 and params.primaryMessage == xi.msg.basic.RANGED_ATTACK_HIT and params.skipParry)
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = origR, origD
    end)
end)
