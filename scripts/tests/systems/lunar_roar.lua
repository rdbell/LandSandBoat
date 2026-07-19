require('scripts/actions/mobskills/lunar_roar')

describe('Lunar Roar mob skill', function()
    it('removes at most six dispelable effects and reports their disappearance', function()
        local lunarRoar = require('scripts/actions/mobskills/lunar_roar')
        local removed, message = {}, nil
        local effects = {}
        for i = 1, 7 do
            effects[i] = {
                getEffectFlags = function() return xi.effectFlag.DISPELABLE end,
                getEffectType = function() return i end,
            }
        end
        effects[8] = {
            getEffectFlags = function() return 0 end,
            getEffectType = function() return 99 end,
        }
        local target = {
            getStatusEffects = function() return effects end,
            delStatusEffect = function(_, effect) removed[#removed + 1] = effect end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(lunarRoar.onMobSkillCheck(target, {}, skill) == 0)
        assert(lunarRoar.onMobWeaponSkill({}, target, skill, {}) == 6)
        assert(#removed == 6 and message == xi.msg.basic.DISAPPEAR_NUM)
    end)

    it('reports no effect when no status effect is dispelable', function()
        local lunarRoar = require('scripts/actions/mobskills/lunar_roar')
        local message = nil
        local target = {
            getStatusEffects = function()
                return { { getEffectFlags = function() return 0 end } }
            end,
            delStatusEffect = function() error('should not remove') end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(lunarRoar.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
