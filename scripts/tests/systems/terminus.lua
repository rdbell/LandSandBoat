require('scripts/actions/mobskills/terminus')
describe('Terminus mob skill', function()
    it('uses ranged piercing plan fTP 2.5/5/7.5 skip defenses', function()
        local skill = require('scripts/actions/mobskills/terminus')
        local params = nil
        local origR, origD = xi.mobskills.mobRangedMove, xi.mobskills.processDamage
        xi.mobskills.mobRangedMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 2.5 and params.fTP[3] == 7.5 and params.skipParry)
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage = origR, origD
    end)
end)
