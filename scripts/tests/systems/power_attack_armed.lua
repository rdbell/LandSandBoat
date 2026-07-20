require('scripts/actions/mobskills/power_attack_armed')
describe('Power Attack Armed mob skill', function()
    it('uses its H2H physical plan and damages only after processing', function()
        local atk = require('scripts/actions/mobskills/power_attack_armed')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(atk.onMobSkillCheck(target, mob, {}) == 0 and atk.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 2 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        atk.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
