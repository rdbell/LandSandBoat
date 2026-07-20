require('scripts/actions/mobskills/sand_breath')
describe('Sand Breath mob skill', function()
    it('uses earth breath plan and applies Blindness after processing', function()
        local breath = require('scripts/actions/mobskills/sand_breath')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(breath.onMobSkillCheck(target, {}, {}) == 0 and breath.onMobWeaponSkill({}, target, {}, {}) == 123)
        assert(params.percentMultipier == 0.083 and params.damageCap == 333 and params.element == xi.element.EARTH)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        breath.onMobWeaponSkill({}, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and statusParams[3] == xi.effect.BLINDNESS and statusParams[4] == 20 and statusParams[6] == 120)
    end)
end)
