require('scripts/actions/mobskills/suction')
describe('Suction mob skill', function()
    it('uses blunt physical plan and applies Stun after processing', function()
        local suc = require('scripts/actions/mobskills/suction')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 20 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 30, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(suc.onMobSkillCheck(target, mob, {}) == 0 and suc.onMobWeaponSkill(mob, target, {}, {}) == 30)
        assert(params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        suc.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 30 and statusParams[3] == xi.effect.STUN and statusParams[6] == 4)
    end)
end)
