require('scripts/actions/mobskills/tachi_enpi')
describe('Tachi Enpi mob skill', function()
    it('uses two-hit physical plan fTP 1/1.5/2', function()
        local skill = require('scripts/actions/mobskills/tachi_enpi')
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
        assert(params.numHits == 2 and params.fTP[1] == 1.0 and params.fTP[3] == 2.0)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
