require('scripts/actions/mobskills/reactor_overheat')
describe('Reactor Overheat mob skill', function()
    it('uses fire magical plan and applies Plague after processing', function()
        local overheat = require('scripts/actions/mobskills/reactor_overheat')
        local move, process, status, random = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        math.random = function(low, high) assert(low==30 and high==60); return 45 end
        xi.mobskills.processDamage = function() return false end
        assert(overheat.onMobSkillCheck(target, mob, {}) == 0 and overheat.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[2] == 1.5 and params.element == xi.element.FIRE)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        overheat.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, math.random = move, process, status, random
        assert(damage[1] == 123 and statusParams[3] == xi.effect.PLAGUE and statusParams[4] == 5 and statusParams[6] == 45)
    end)
end)
