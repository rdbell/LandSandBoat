require('scripts/actions/mobskills/tachi_fudo')
describe('Tachi Fudo mob skill', function()
    it('uses physical plan fTP 3.75/4.75/5.75 attackMultiplier 2', function()
        local skill = require('scripts/actions/mobskills/tachi_fudo')
        local params = nil
        local origP, origD = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 3.75 and params.attackMultiplier[1] == 2.0)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
