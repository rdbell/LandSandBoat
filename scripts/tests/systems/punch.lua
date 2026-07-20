require('scripts/actions/mobskills/punch')

describe('Punch mob skill', function()
    it('uses its blunt physical plan and damages only after processing', function()
        local punch = require('scripts/actions/mobskills/punch')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(punch.onMobSkillCheck(target, mob, {}) == 0 and punch.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 3.5 and params.damageType == xi.damageType.BLUNT and damage == nil)
        xi.mobskills.processDamage = function() return true end
        punch.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
