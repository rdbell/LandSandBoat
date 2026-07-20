require('scripts/actions/mobskills/shock_strike')
describe('Shock Strike mob skill', function()
    it('uses blunt physical plan and applies Stun after processing', function()
        local strike = require('scripts/actions/mobskills/shock_strike')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(strike.onMobSkillCheck(target, mob, {}) == 0 and strike.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 3.5 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        strike.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.STUN and statusParams[6] == 12)
    end)
end)
