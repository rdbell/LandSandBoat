require('scripts/actions/mobskills/sinuate_rush')
describe('Sinuate Rush mob skill', function()
    it('uses blunt physical plan with crit and damages only after processing', function()
        local rush = require('scripts/actions/mobskills/sinuate_rush')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(rush.onMobSkillCheck(target, mob, {}) == 0 and rush.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.fTP[1] == 2 and params.canCrit and damage == nil)
        xi.mobskills.processDamage = function() return true end
        rush.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 80)
    end)
end)
