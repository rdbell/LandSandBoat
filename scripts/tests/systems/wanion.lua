require('scripts/actions/mobskills/wanion')
describe('Wanion mob skill', function()
    it('transfers present ailments and sets NONE', function()
        local skill = require('scripts/actions/mobskills/wanion')
        local status = xi.mobskills.mobStatusEffectMove
        local message, transferred, deleted = nil, nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local effects = {
            [xi.effect.POISON] = { getPower = function() return 10 end, getTick = function() return 3 end, getTimeRemaining = function() return 30000 end },
        }
        local mob = {
            getStatusEffect = function(_, e) return effects[e] end,
            delStatusEffect = function(_, e) deleted = e end,
        }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobStatusEffectMove = function(m, t, effect, power, tick, duration)
            transferred = { effect, power, tick, duration }
        end
        skill.onMobWeaponSkill(mob, {}, sk, {})
        xi.mobskills.mobStatusEffectMove = status
        assert(message == xi.msg.basic.NONE)
        assert(transferred[1] == xi.effect.POISON and transferred[2] == 10 and transferred[3] == 3 and transferred[4] == 30)
        assert(deleted == xi.effect.POISON)
    end)
end)
