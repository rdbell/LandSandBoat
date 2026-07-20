require('scripts/actions/mobskills/rhino_attack')
describe('Rhino Attack mob skill', function()
    it('uses its blunt physical plan with attack multiplier and crit', function()
        local atk = require('scripts/actions/mobskills/rhino_attack')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(atk.onMobSkillCheck(target, mob, {}) == 0 and atk.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.attackMultiplier[1] == 2 and params.canCrit and params.criticalChance[1] == 0.10 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        atk.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
