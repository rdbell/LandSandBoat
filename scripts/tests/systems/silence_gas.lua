require('scripts/actions/mobskills/silence_gas')
describe('Silence Gas mob skill', function()
    it('uses dark breath plan and applies Silence after processing', function()
        local gas = require('scripts/actions/mobskills/silence_gas')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local random = math.random
        local params, damage, statusParams = nil, nil, nil
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        math.random = function(a, b)
            assert(a == 15 and b == 60)
            return 30
        end
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.BREATH, damageType = xi.damageType.DARK }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(gas.onMobSkillCheck(target, {}, {}) == 0 and gas.onMobWeaponSkill({}, target, {}, {}) == 100)
        assert(params.percentMultipier == 0.25 and params.damageCap == 800 and params.element == xi.element.DARK and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        gas.onMobWeaponSkill({}, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        math.random = random
        assert(damage[1] == 100 and statusParams[3] == xi.effect.SILENCE and statusParams[6] == 30)
    end)
end)
