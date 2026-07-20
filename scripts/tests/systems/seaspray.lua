require('scripts/actions/mobskills/seaspray')
describe('Seaspray mob skill', function()
    it('uses water breath plan and applies Slow after processing', function()
        local spray = require('scripts/actions/mobskills/seaspray')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local random = math.random
        local params, damage, statusParams = nil, nil, nil
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        math.random = function(a, b)
            assert(a == 30 and b == 60)
            return 45
        end
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.BREATH, damageType = xi.damageType.WATER }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(spray.onMobSkillCheck(target, {}, {}) == 0 and spray.onMobWeaponSkill({}, target, {}, {}) == 80)
        assert(params.percentMultipier == 0.125 and params.damageCap == 500 and params.element == xi.element.WATER and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        spray.onMobWeaponSkill({}, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        math.random = random
        assert(damage[1] == 80 and statusParams[3] == xi.effect.SLOW and statusParams[4] == 3500 and statusParams[6] == 45)
    end)
end)
