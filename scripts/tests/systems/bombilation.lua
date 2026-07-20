require('scripts/actions/mobskills/bombilation')
describe('Bombilation mob skill', function()
    it('resets TP or reports no effect', function()
        local skill = require('scripts/actions/mobskills/bombilation')
        local msg, tp = nil, 1500
        local target = {
            getTP = function() return tp end,
            setTP = function(_, v) tp = v end,
        }
        local skillObj = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, target, skillObj, {}) == 0)
        assert(tp == 0 and msg == xi.msg.basic.TP_REDUCED)
        tp = 0
        msg = nil
        assert(skill.onMobWeaponSkill({}, target, skillObj, {}) == 0)
        assert(msg == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
