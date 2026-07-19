require('scripts/actions/mobskills/blank_gaze_dispel')

describe('Blank Gaze Dispel mob skill', function()
    it('dispels a facing target and reports erase when an effect is removed', function()
        local blankGaze = require('scripts/actions/mobskills/blank_gaze_dispel')
        local message = nil
        local target = {
            isFacing = function(_, mob) return mob ~= nil end,
            dispelStatusEffect = function() return 123 end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(blankGaze.onMobSkillCheck(target, {}, skill) == 0)
        assert(blankGaze.onMobWeaponSkill({}, target, skill, {}) == 123)
        assert(message == xi.msg.basic.SKILL_ERASE)
    end)

    it('does not dispel a target not facing the mob or with no dispelable effect', function()
        local blankGaze = require('scripts/actions/mobskills/blank_gaze_dispel')
        local calls, message = 0, nil
        local target = {
            isFacing = function() return false end,
            dispelStatusEffect = function() calls = calls + 1 return xi.effect.NONE end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(blankGaze.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(calls == 0 and message == xi.msg.basic.SKILL_NO_EFFECT)

        target.isFacing = function() return true end
        assert(blankGaze.onMobWeaponSkill({}, target, skill, {}) == xi.effect.NONE)
        assert(calls == 1 and message == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
