require('scripts/actions/mobskills/tachi_rana')
describe('Tachi Rana mob skill', function()
    it('uses three-hit accuracy-scaled physical plan', function()
        local skill = require('scripts/actions/mobskills/tachi_rana')
        local params = nil
        local origP, origD = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 90, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.numHits == 3 and params.accuracyModifier[3] == 60)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
