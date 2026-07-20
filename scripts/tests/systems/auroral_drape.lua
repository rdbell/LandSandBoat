require('scripts/actions/mobskills/auroral_drape')
describe('Auroral Drape mob skill', function()
    it('prefers Silence then Blind and messages ENFEEB_IS or MISS', function()
        local skill = require('scripts/actions/mobskills/auroral_drape')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local calls, message = {}, nil
        local sk = { setMsg = function(_, v) message = v end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            table.insert(calls, { effect, power, tick, duration })
            if effect == xi.effect.SILENCE then return xi.msg.basic.SKILL_ENFEEB_IS end
            return xi.msg.basic.SKILL_MISS
        end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.SILENCE)
        assert(calls[1][1] == xi.effect.SILENCE and calls[1][2] == 1 and calls[1][4] == 60)
        assert(calls[2][1] == xi.effect.BLINDNESS and calls[2][2] == 50 and calls[2][4] == 90)
        assert(message == xi.msg.basic.SKILL_ENFEEB_IS)
        calls, message = {}, nil
        xi.mobskills.mobStatusEffectMove = function(_, _, effect)
            table.insert(calls, effect)
            return xi.msg.basic.SKILL_MISS
        end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == nil)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(message == xi.msg.basic.SKILL_MISS)
    end)
end)
