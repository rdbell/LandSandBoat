require('scripts/actions/mobskills/bastion_of_twilight')
describe('Bastion of Twilight mob skill', function()
    it('gates on shields and applies Magic Shield with animSub 2', function()
        local skill = require('scripts/actions/mobskills/bastion_of_twilight')
        local effects, added, finalSub, message = {}, nil, nil, nil
        local mob = {
            hasStatusEffect = function(_, e) return effects[e] end,
            addStatusEffect = function(self, effect, opts) added = { effect, opts } end,
        }
        local sk = {
            setMsg = function(_, v) message = v end,
            setFinalAnimationSub = function(_, v) finalSub = v end,
        }
        effects[xi.effect.MAGIC_SHIELD] = true
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        effects = {}
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.MAGIC_SHIELD)
        assert(added[1] == xi.effect.MAGIC_SHIELD and added[2].power == 1 and added[2].origin == mob)
        assert(finalSub == 2 and message == xi.msg.basic.SKILL_GAIN_EFFECT)
    end)
end)
