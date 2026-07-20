require('scripts/actions/mobskills/carnal_nightmare')
describe('Carnal Nightmare mob skill', function()
    it('resets TP fully or halfway for pets', function()
        local skill = require('scripts/actions/mobskills/carnal_nightmare')
        local msg, tp = nil, 1000
        local target = {
            getTP = function() return tp end,
            setTP = function(_, v) tp = v end,
            isPet = function() return false end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == 0)
        assert(tp == 0 and msg == xi.msg.basic.TP_REDUCED)
        tp, msg = 0, nil
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == 0)
        assert(msg == xi.msg.basic.SKILL_NO_EFFECT)
        tp, msg = 1000, nil
        target.isPet = function() return true end
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == 500)
        assert(tp == 500 and msg == xi.msg.basic.TP_REDUCED)
    end)
end)
