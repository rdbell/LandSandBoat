require('scripts/actions/mobskills/pandemic_nip')
describe('Pandemic Nip mob skill', function()
    it('uses one-hit physical plan (debuff transfer TODO upstream)', function()
        local skill = require('scripts/actions/mobskills/pandemic_nip')
        local params = nil
        local origP, origD = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 55, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getWeaponDmg = function() return 30 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 55)
        assert(params.numHits == 1 and params.fTP[1] == 1.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
