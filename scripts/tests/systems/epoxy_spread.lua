require('scripts/actions/mobskills/epoxy_spread')

describe('Epoxy Spread mob skill', function()
    it('attempts 90-second Bind and forwards the status-effect message', function()
        local epoxy = require('scripts/actions/mobskills/epoxy_spread')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local request, message = nil, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(source, recipient, effect, power, tick, duration)
            request = { source, recipient, effect, power, tick, duration }
            return 777
        end

        assert(epoxy.onMobSkillCheck(target, mob, skill) == 0)
        assert(epoxy.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.BIND)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(request[1] == mob and request[2] == target and request[3] == xi.effect.BIND and request[4] == 1 and request[5] == 0 and request[6] == 90 and message == 777)
    end)
end)
