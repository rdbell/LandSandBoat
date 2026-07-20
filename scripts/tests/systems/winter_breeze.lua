require('scripts/actions/mobskills/winter_breeze')
describe('Winter Breeze mob skill', function()
    it('dispels one effect, always stuns, and messages by dispel result', function()
        local skill = require('scripts/actions/mobskills/winter_breeze')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local effect, message = nil, nil
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        local sk = { setMsg = function(_, v) message = v end }
        local target = { dispelStatusEffect = function() return xi.effect.NONE end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.NONE)
        assert(effect[1] == xi.effect.STUN and effect[2] == 1 and effect[4] == 2)
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
        target.dispelStatusEffect = function() return xi.effect.SLOW end
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.SLOW)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(message == xi.msg.basic.SKILL_ERASE)
    end)
end)
