require('scripts/actions/mobskills/tachi_shoha')
describe('Tachi Shoha mob skill', function()
    it('uses two-hit physical plan fTP 1.375/2.1875/2.6875', function()
        local skill = require('scripts/actions/mobskills/tachi_shoha')
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
        assert(params.numHits == 2 and params.fTP[1] == 1.375 and params.attackMultiplier[1] == 1.375)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
