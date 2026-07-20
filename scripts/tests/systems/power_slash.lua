require('scripts/actions/mobskills/power_slash')
describe('Power Slash mob skill', function()
    it('uses its slashing physical plan with crit chances and damages only after processing', function()
        local slash = require('scripts/actions/mobskills/power_slash')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(slash.onMobSkillCheck(target, mob, {}) == 0 and slash.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.canCrit and params.criticalChance[1]==0.2 and params.criticalChance[2]==0.4 and params.criticalChance[3]==0.6 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        slash.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
