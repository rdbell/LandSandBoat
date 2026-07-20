require('scripts/actions/mobskills/skewer')
describe('Skewer mob skill', function()
    it('uses physical plan with crit and damages only after processing', function()
        local skillmod = require('scripts/actions/mobskills/skewer')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 30 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 70, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skillmod.onMobSkillCheck(target, mob, {}) == 0 and skillmod.onMobWeaponSkill(mob, target, {}, {}) == 70)
        assert(params.canCrit and damage == nil)
        xi.mobskills.processDamage = function() return true end
        skillmod.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 70)
    end)
end)
