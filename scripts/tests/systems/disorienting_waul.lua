require('scripts/actions/mobskills/disorienting_waul')

describe('Disorienting Waul mob skill', function()
    it('always requests fixed Amnesia and forwards its helper message', function()
        local waul = require('scripts/actions/mobskills/disorienting_waul')
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local request, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) request = { ... }; return 456 end

        assert(waul.onMobSkillCheck({}, {}, {}) == 0)
        assert(waul.onMobWeaponSkill('mob', 'target', skill, {}) == xi.effect.AMNESIA)
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(request[1] == 'mob' and request[2] == 'target' and request[3] == xi.effect.AMNESIA)
        assert(request[4] == 35 and request[5] == 0 and request[6] == 60 and request[7] == 80 and message == 456)
    end)
end)
