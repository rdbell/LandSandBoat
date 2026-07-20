require('scripts/actions/mobskills/wheel_of_impregnability')
describe('Wheel of Impregnability mob skill', function()
    it('denies existing shields and applies PHYSICAL_SHIELD with animation sub 1', function()
        local skill = require('scripts/actions/mobskills/wheel_of_impregnability')
        local message, anim, effectArgs = nil, nil, nil
        local sk = {
            setMsg = function(_, v) message = v end,
            setFinalAnimationSub = function(_, v) anim = v end,
        }
        local has = { [xi.effect.PHYSICAL_SHIELD] = true }
        local mob = {
            hasStatusEffect = function(_, e) return has[e] == true end,
            addStatusEffect = function(_, effect, opts) effectArgs = { effect, opts } end,
        }
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        has = {}
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.PHYSICAL_SHIELD)
        assert(message == xi.msg.basic.SKILL_GAIN_EFFECT and anim == 1)
        assert(effectArgs[1] == xi.effect.PHYSICAL_SHIELD and effectArgs[2].power == 1 and effectArgs[2].origin == mob)
    end)
end)
