require('scripts/actions/mobskills/entangle')

describe('Entangle mob skill', function()
    it('attempts Bind and forwards the status-effect message', function()
        local entangle = require('scripts/actions/mobskills/entangle')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local request, message = nil, nil
        local mob = {}
        local target = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(source, recipient, effect, power, tick, duration)
            request = { source, recipient, effect, power, tick, duration }
            return 777
        end

        assert(entangle.onMobSkillCheck(target, mob, skill) == 0)
        assert(entangle.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.BIND)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(request[1] == mob and request[2] == target and request[3] == xi.effect.BIND and request[4] == 1 and request[5] == 0 and request[6] == 60 and message == 777)
    end)
end)
