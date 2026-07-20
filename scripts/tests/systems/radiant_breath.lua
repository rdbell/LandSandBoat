require('scripts/actions/mobskills/radiant_breath')
describe('Radiant Breath mob skill', function()
    it('uses light breath plan and applies Slow and Silence after processing', function()
        local breath = require('scripts/actions/mobskills/radiant_breath')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, calls = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1000 end }
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.BREATH, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            calls[#calls + 1] = { ... }
        end
        xi.mobskills.processDamage = function() return false end
        assert(breath.onMobSkillCheck(target, mob, skill) == 0 and breath.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.percentMultipier == 0.125 and params.damageCap == 700 and params.bonusDamage == 104)
        assert(params.element == xi.element.LIGHT and params.resistStat == xi.mod.INT)
        assert(damage == nil and #calls == 0)
        xi.mobskills.processDamage = function() return true end
        breath.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123)
        assert(calls[1][3] == xi.effect.SLOW and calls[1][4] == 1250 and calls[1][6] == 120)
        assert(calls[2][3] == xi.effect.SILENCE and calls[2][4] == 1 and calls[2][6] == 120)
    end)
end)
