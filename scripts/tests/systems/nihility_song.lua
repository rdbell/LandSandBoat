require('scripts/actions/mobskills/nihility_song')

describe('Nihility Song mob skill', function()
    it('dispels Dispelable|Food effects and selects its message from the result', function()
        local song = require('scripts/actions/mobskills/nihility_song')
        local flags, message, effect = nil, nil, xi.effect.NONE
        local target = {
            dispelStatusEffect = function(_, value)
                flags = value
                return effect
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(song.onMobSkillCheck(target, {}, skill) == 0)
        assert(song.onMobWeaponSkill({}, target, skill, {}) == xi.effect.NONE)
        assert(flags == bit.bor(xi.effectFlag.DISPELABLE, xi.effectFlag.FOOD))
        assert(message == xi.msg.basic.SKILL_NO_EFFECT)

        effect = xi.effect.HASTE
        assert(song.onMobWeaponSkill({}, target, skill, {}) == xi.effect.HASTE)
        assert(message == xi.msg.basic.SKILL_ERASE)
    end)
end)
