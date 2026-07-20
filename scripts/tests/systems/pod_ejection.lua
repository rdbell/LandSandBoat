require('scripts/actions/mobskills/pod_ejection')
describe('Pod Ejection mob skill', function()
    it('schedules gunpod spawn timer and returns 0', function()
        local skill = require('scripts/actions/mobskills/pod_ejection')
        local msg, delay = nil, nil
        local mob = {
            timer = function(_, d, cb) delay = d end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 0)
        assert(delay == 3000 and msg == 0)
    end)
end)
