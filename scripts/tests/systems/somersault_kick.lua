require('scripts/actions/mobskills/somersault_kick')
describe('Somersault Kick mob skill', function()
    it('uses blunt physical plan and damages only after processing', function()
        local kick = require('scripts/actions/mobskills/somersault_kick')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 25 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 40, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(kick.onMobSkillCheck(target, mob, {}) == 0 and kick.onMobWeaponSkill(mob, target, {}, {}) == 40)
        assert(params.fTP[1] == 1.5 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        kick.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 40)
    end)
end)
