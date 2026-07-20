require('scripts/actions/mobskills/sheep_charge_melee')
describe('Sheep Charge Melee mob skill', function()
    it('uses blunt physical plan with fTP 1.5 and damages only after processing', function()
        local charge = require('scripts/actions/mobskills/sheep_charge_melee')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 20 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 40, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(charge.onMobSkillCheck(target, mob, {}) == 0 and charge.onMobWeaponSkill(mob, target, {}, {}) == 40)
        assert(params.fTP[1] == 1.5 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        charge.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 40)
    end)
end)
