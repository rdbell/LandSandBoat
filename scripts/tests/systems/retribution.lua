require('scripts/actions/mobskills/retribution')
describe('Retribution mob skill', function()
    it('uses its blunt physical plan with attack multiplier and damages only after processing', function()
        local ret = require('scripts/actions/mobskills/retribution')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(ret.onMobSkillCheck(target, mob, {}) == 0 and ret.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 2 and params.fTP[2] == 2.5 and params.fTP[3] == 3 and params.attackMultiplier[1] == 1.5 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        ret.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
