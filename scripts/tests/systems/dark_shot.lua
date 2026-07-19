require('scripts/actions/mobskills/dark_shot')

describe('Dark Shot mob skill', function()
    it('is always available', function()
        local skill = require('scripts/actions/mobskills/dark_shot')

        assert(skill.onMobSkillCheck({}, {}, {}) == 0)
    end)

    it('reports no effect when no dispellable status effect is removed', function()
        local message = nil
        local target = {
            dispelStatusEffect = function(_, flag)
                assert(flag == xi.effectFlag.DISPELABLE)
                return xi.effect.NONE
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        local shot = require('scripts/actions/mobskills/dark_shot')

        assert(shot.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
    end)

    it('reports success when a dispellable status effect is removed', function()
        local message = nil
        local target = {
            dispelStatusEffect = function()
                return xi.effect.HASTE
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        local shot = require('scripts/actions/mobskills/dark_shot')

        assert(shot.onMobWeaponSkill({}, target, skill, {}) == 1)
        assert(message == xi.msg.basic.DISAPPEAR_NUM)
    end)
end)
