require('scripts/actions/mobskills/tortoise_song')
describe('Tortoise Song mob skill', function()
    it('dispels song/roll flags and messages by count', function()
        local skill = require('scripts/actions/mobskills/tortoise_song')
        local message, flags = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local target = {
            dispelAllStatusEffect = function(_, f)
                flags = f
                return 0
            end,
        }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == 0 and message == xi.msg.basic.SKILL_NO_EFFECT)
        assert(flags == bit.bor(xi.effectFlag.SONG, xi.effectFlag.ROLL))
        target.dispelAllStatusEffect = function() return 3 end
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == 3 and message == xi.msg.basic.DISAPPEAR_NUM)
    end)
end)
