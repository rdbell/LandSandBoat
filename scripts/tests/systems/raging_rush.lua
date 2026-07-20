require('scripts/actions/mobskills/raging_rush')
describe('Raging Rush mob skill', function()
    it('uses its threefold slashing plan with crit chances and damages only after processing', function()
        local rush = require('scripts/actions/mobskills/raging_rush')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(rush.onMobSkillCheck(target, mob, {}) == 0 and rush.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 3 and params.canCrit and params.criticalChance[1] == 0.1 and params.criticalChance[2] == 0.3 and params.criticalChance[3] == 0.5 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        rush.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
