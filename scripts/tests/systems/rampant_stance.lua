require('scripts/actions/mobskills/rampant_stance')
describe('Rampant Stance mob skill', function()
    it('uses its threefold blunt plan and applies Stun after processing', function()
        local stance = require('scripts/actions/mobskills/rampant_stance')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(stance.onMobSkillCheck(target, mob, {}) == 0 and stance.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 3 and params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        stance.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and statusParams[3] == xi.effect.STUN and statusParams[4] == 1 and statusParams[6] == 5)
    end)
end)
