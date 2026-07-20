require('scripts/actions/mobskills/blockhead')
describe('Blockhead mob skill', function()
    it('uses one-hit Blunt plan fTP 2.0', function()
        local skill = require('scripts/actions/mobskills/blockhead')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 60, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 60)
        assert(params.fTP[1] == 2.0 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 60)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 60)
    end)
end)
