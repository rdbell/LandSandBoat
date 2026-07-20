require('scripts/actions/mobskills/clobber')
describe('Clobber mob skill', function()
    it('admits targets behind and uses physical plan', function()
        local skill = require('scripts/actions/mobskills/clobber')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local behind = false
        local mob = { getWeaponDmg = function() return 50 end }
        local target = {
            isBehind = function() return behind end,
            takeDamage = function(_, v) damage = v end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        behind = true
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 3.0 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
