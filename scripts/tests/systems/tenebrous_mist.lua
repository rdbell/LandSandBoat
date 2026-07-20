require('scripts/actions/mobskills/tenebrous_mist')
describe('Tenebrous Mist mob skill', function()
    it('resets TP or reports no effect', function()
        local skill = require('scripts/actions/mobskills/tenebrous_mist')
        local msg, tp = nil, 500
        local target = {
            getTP = function() return tp end,
            setTP = function(_, v) tp = v end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == 0)
        assert(tp == 0 and msg == xi.msg.basic.TP_REDUCED)
        tp, msg = 0, nil
        skill.onMobWeaponSkill({}, target, sk, {})
        assert(msg == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
