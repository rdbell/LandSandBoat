require('scripts/actions/mobskills/reactor_overload')
describe('Reactor Overload mob skill', function()
    it('uses wind magical plan and applies Silence after processing', function()
        local overload = require('scripts/actions/mobskills/reactor_overload')
        local move, process, status, random = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        math.random = function(low, high) assert(low==15 and high==60); return 30 end
        xi.mobskills.processDamage = function() return false end
        assert(overload.onMobSkillCheck(target, mob, {}) == 0 and overload.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.element == xi.element.WIND and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        overload.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random = move, process, status, random
        assert(damage[1] == 123 and statusParams[3] == xi.effect.SILENCE and statusParams[6] == 30)
    end)
end)
