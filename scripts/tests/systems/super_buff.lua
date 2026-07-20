require('scripts/actions/mobskills/super_buff')
describe('Super Buff mob skill', function()
    it('applies SUPER_BUFF for 30s and sets NONE message', function()
        local skill = require('scripts/actions/mobskills/super_buff')
        local message, effectArgs = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local target = {
            addStatusEffect = function(_, effect, opts)
                effectArgs = { effect, opts }
            end,
        }
        local mob = {}
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 0)
        assert(message == xi.msg.basic.NONE)
        assert(effectArgs[1] == xi.effect.SUPER_BUFF)
        assert(effectArgs[2].duration == 30 and effectArgs[2].icon == 0 and effectArgs[2].origin == mob)
    end)
end)
