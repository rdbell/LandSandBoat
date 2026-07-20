require('scripts/actions/mobskills/tribulation')
describe('Tribulation mob skill', function()
    it('prefers blindness then bio and messages SKILL_ENFEEB_IS or MISS', function()
        local skill = require('scripts/actions/mobskills/tribulation')
        local status = xi.mobskills.mobStatusEffectMove
        local message, n = nil, 0
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        xi.mobskills.mobStatusEffectMove = function(_, _, effect)
            n = n + 1
            if effect == xi.effect.BLINDNESS then return xi.msg.basic.SKILL_ENFEEB_IS end
            return 0
        end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.BLINDNESS and message == xi.msg.basic.SKILL_ENFEEB_IS)
        n = 0
        xi.mobskills.mobStatusEffectMove = function(_, _, effect)
            if effect == xi.effect.BIO then return xi.msg.basic.SKILL_ENFEEB_IS end
            return 0
        end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.BIO and message == xi.msg.basic.SKILL_ENFEEB_IS)
        xi.mobskills.mobStatusEffectMove = function() return 0 end
        local ret = skill.onMobWeaponSkill({}, {}, sk, {})
        xi.mobskills.mobStatusEffectMove = status
        assert(ret == nil and message == xi.msg.basic.SKILL_MISS)
    end)
end)
