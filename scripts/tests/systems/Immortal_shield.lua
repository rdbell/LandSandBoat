require('scripts/actions/mobskills/Immortal_shield')
describe('Immortal Shield mob skill', function()
    it('adds Magic Shield tick 1 subType 45 and messages SKILL_GAIN_EFFECT', function()
        local skill = require('scripts/actions/mobskills/Immortal_shield')
        local added, message = nil, nil
        local mob = {
            addStatusEffect = function(self, effect, opts)
                added = { effect, opts }
            end,
        }
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.MAGIC_SHIELD)
        assert(added[1] == xi.effect.MAGIC_SHIELD)
        assert(added[2].origin == mob and added[2].tick == 1 and added[2].subType == 45)
        assert(message == xi.msg.basic.SKILL_GAIN_EFFECT)
    end)
end)
