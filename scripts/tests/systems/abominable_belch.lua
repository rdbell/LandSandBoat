require('scripts/actions/mobskills/abominable_belch')

describe('Abominable Belch mob skill', function()
    it('applies Plague, Silence, and Paralysis using TP-scaled durations', function()
        local effectMove = xi.mobskills.mobStatusEffectMove
        local calls, messages = {}, {}
        local mob, target = {}, {}
        local skill = {
            getTP = function() return 1500 end,
            setMsg = function(_, value) table.insert(messages, value) end,
        }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            table.insert(calls, { effect, power, tick, duration })
            return effect + 100
        end
        local belch = require('scripts/actions/mobskills/abominable_belch')
        assert(belch.onMobSkillCheck(target, mob, skill) == 0)
        assert(belch.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.PLAGUE)
        xi.mobskills.mobStatusEffectMove = effectMove
        assert(calls[1][1] == xi.effect.PLAGUE and calls[1][2] == 10 and calls[1][3] == 3 and calls[1][4] == 30)
        assert(calls[2][1] == xi.effect.SILENCE and calls[2][2] == 1 and calls[2][3] == 0 and calls[2][4] == 45)
        assert(calls[3][1] == xi.effect.PARALYSIS and calls[3][2] == 25 and calls[3][3] == 0 and calls[3][4] == 45)
        assert(messages[1] == xi.effect.PLAGUE + 100 and messages[2] == xi.effect.SILENCE + 100 and messages[3] == xi.effect.PARALYSIS + 100)
    end)
end)
