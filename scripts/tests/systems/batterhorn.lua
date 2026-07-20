require('scripts/actions/mobskills/batterhorn')
describe('Batterhorn mob skill', function()
    it('admits in-front targets and uses Slashing plan', function()
        local skill = require('scripts/actions/mobskills/batterhorn')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 60 end }
        local target = {
            isInfront = function() return false end,
            takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        target.isInfront = function() return true end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.fTP[1] == 2.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 90)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 90)
    end)
end)
