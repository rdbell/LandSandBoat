require('scripts/actions/mobskills/spiral_spin')
describe('Spiral Spin mob skill', function()
    it('uses physical plan and applies status after processing', function()
        local skillmod = require('scripts/actions/mobskills/spiral_spin')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 30 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 50, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skillmod.onMobSkillCheck(target, mob, {}) == 0 and skillmod.onMobWeaponSkill(mob, target, {}, {}) == 50)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skillmod.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 50 and statusParams[3] == xi.effect.ACCURACY_DOWN and statusParams[6] == 60)
    end)
end)
