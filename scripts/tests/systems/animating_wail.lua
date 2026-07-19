require('scripts/actions/mobskills/animating_wail')

describe('Animating Wail mob skill', function()
    it('uses the TP-scaled Haste buff and forwards its message', function()
        local args, message = nil, nil
        local tp = 1500
        local mob = { getTP = function() return tp end }
        local skill = { setMsg = function(_, value) message = value end }
        local buffMove = xi.mobskills.mobBuffMove
        xi.mobskills.mobBuffMove = function(...)
            args = { ... }
            return 321
        end

        local wail = require('scripts/actions/mobskills/animating_wail')
        assert(wail.onMobSkillCheck({}, {}, {}) == 0)
        assert(wail.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.HASTE)
        assert(args[2] == xi.effect.HASTE and args[3] == 3000 and args[4] == 0 and args[5] == 105 and message == 321)

        tp = 3000
        assert(wail.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.HASTE)
        xi.mobskills.mobBuffMove = buffMove
        assert(args[5] == 150)
    end)
end)
