require('scripts/actions/mobskills/murk')

describe('Murk mob skill', function()
    it('applies Slow then Weight and selects its message from host results', function()
        local murk = require('scripts/actions/mobskills/murk')
        local status = xi.mobskills.mobStatusEffectMove
        local calls, message = {}, nil
        local slowResult, weightResult = xi.msg.basic.SKILL_ENFEEB_IS, xi.msg.basic.SKILL_ENFEEB_IS
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobStatusEffectMove = function(...)
            local args = { ... }
            table.insert(calls, args)
            if args[3] == xi.effect.SLOW then
                return slowResult
            end
            return weightResult
        end

        assert(murk.onMobSkillCheck({}, {}, skill) == 0)
        assert(murk.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.SLOW)
        assert(#calls == 2)
        assert(calls[1][3] == xi.effect.SLOW and calls[1][4] == 8500 and calls[1][5] == 0 and calls[1][6] == 90 and calls[1][7] == 0 and calls[1][8] == 1)
        assert(calls[2][3] == xi.effect.WEIGHT and calls[2][4] == 44 and calls[2][5] == 0 and calls[2][6] == 120)
        assert(message == xi.msg.basic.SKILL_ENFEEB_IS)

        slowResult = xi.msg.basic.SKILL_MISS
        calls = {}
        assert(murk.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.WEIGHT)
        assert(message == xi.msg.basic.SKILL_ENFEEB_IS)

        weightResult = xi.msg.basic.SKILL_MISS
        calls = {}
        assert(murk.onMobWeaponSkill({}, {}, skill, {}) == nil)
        assert(message == xi.msg.basic.SKILL_MISS)

        xi.mobskills.mobStatusEffectMove = status
    end)
end)
