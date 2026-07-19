require('scripts/actions/mobskills/curse')

describe('Curse mob skill', function()
    it('applies its fixed Curse effect and forwards the helper message', function()
        local curse = require('scripts/actions/mobskills/curse')
        local originalStatusEffectMove = xi.mobskills.mobStatusEffectMove
        local request, message = nil, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobStatusEffectMove = function(source, recipient, effect, power, tick, duration)
            request = { source, recipient, effect, power, tick, duration }
            return 123
        end

        assert(curse.onMobSkillCheck(target, mob, skill) == 0)
        assert(curse.onMobWeaponSkill(mob, target, skill, nil) == xi.effect.CURSE_I)

        xi.mobskills.mobStatusEffectMove = originalStatusEffectMove

        assert(request[1] == mob and request[2] == target and request[3] == xi.effect.CURSE_I)
        assert(request[4] == 25 and request[5] == 0 and request[6] == 2280)
        assert(message == 123)
    end)
end)
