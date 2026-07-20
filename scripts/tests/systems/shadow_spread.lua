require('scripts/actions/mobskills/shadow_spread')
describe('Shadow Spread mob skill', function()
    it('applies curse sleep blindness with first ENFEEB_IS return', function()
        local spread = require('scripts/actions/mobskills/shadow_spread')
        local status = xi.mobskills.mobStatusEffectMove
        local calls, message, pool = {}, nil, 0
        local mob = { getPool = function() return pool end }
        local skill = { setMsg = function(_, value) message = value end }
        local seq = { xi.msg.basic.SKILL_NO_EFFECT, xi.msg.basic.SKILL_ENFEEB_IS, xi.msg.basic.SKILL_NO_EFFECT }
        local i = 0
        xi.mobskills.mobStatusEffectMove = function(...)
            i = i + 1
            calls[i] = { ... }
            return seq[i]
        end
        assert(spread.onMobSkillCheck({}, mob, skill) == 0)
        assert(spread.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.SLEEP_I)
        xi.mobskills.mobStatusEffectMove = status
        assert(calls[1][3] == xi.effect.CURSE_I and calls[1][4] == 20 and calls[1][6] == 180)
        assert(calls[2][3] == xi.effect.SLEEP_I and calls[2][6] == 60)
        assert(calls[3][3] == xi.effect.BLINDNESS and calls[3][4] == 100 and message == xi.msg.basic.SKILL_ENFEEB_IS)
        pool = xi.mobPool.WREAKER
        i, calls = 0, {}
        seq = { xi.msg.basic.SKILL_NO_EFFECT, xi.msg.basic.SKILL_NO_EFFECT, xi.msg.basic.SKILL_NO_EFFECT }
        xi.mobskills.mobStatusEffectMove = function(...)
            i = i + 1
            calls[i] = { ... }
            return seq[i]
        end
        assert(spread.onMobWeaponSkill(mob, {}, skill, {}) == 0)
        xi.mobskills.mobStatusEffectMove = status
        assert(calls[2][6] == 120 and message == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
