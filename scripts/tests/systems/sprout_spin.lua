require('scripts/actions/mobskills/sprout_spin')
describe('Sprout Spin mob skill', function()
    it('uses blunt physical plan and damages only after processing', function()
        local spin = require('scripts/actions/mobskills/sprout_spin')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 25 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 50, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(spin.onMobSkillCheck(target, mob, {}) == 0 and spin.onMobWeaponSkill(mob, target, {}, {}) == 50)
        assert(params.fTP[1] == 2 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        spin.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 50)
    end)
end)
