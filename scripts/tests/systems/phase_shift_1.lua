require('scripts/actions/mobskills/phase_shift_1')
describe('Phase Shift 1 mob skill', function()
    it('always fails skill check and uses fTP 3.0 physical plan', function()
        local skill = require('scripts/actions/mobskills/phase_shift_1')
        local params = nil
        local origP, origD = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 3.0 and params.canCrit and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_2)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
