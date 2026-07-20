require('scripts/actions/mobskills/shibaraku')
describe('Shibaraku mob skill', function()
    it('uses slashing physical plan and applies Stun after processing', function()
        local skillmod = require('scripts/actions/mobskills/shibaraku')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skillmod.onMobSkillCheck(target, mob, {}) == 0 and skillmod.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.fTP[1] == 2.2 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skillmod.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 90 and statusParams[3] == xi.effect.STUN and statusParams[6] == 4)
    end)
end)
