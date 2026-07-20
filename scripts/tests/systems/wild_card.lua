require('scripts/actions/mobskills/wild_card')
describe('Wild Card mob skill', function()
    it('sets target TP to 3000 and messages TP_INCREASE', function()
        local skill = require('scripts/actions/mobskills/wild_card')
        local tp, message = nil, nil
        local target = { setTP = function(_, v) tp = v end }
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == 3000)
        assert(tp == 3000 and message == xi.msg.basic.TP_INCREASE)
    end)
end)
