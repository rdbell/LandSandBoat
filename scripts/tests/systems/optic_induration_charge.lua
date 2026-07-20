require('scripts/actions/mobskills/optic_induration_charge')
describe('Optic Induration Charge mob skill', function()
    it('sets NONE message and returns 0 (Zdei mixin host)', function()
        local skill = require('scripts/actions/mobskills/optic_induration_charge')
        local msg = nil
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 0)
        assert(msg == xi.msg.basic.NONE)
    end)
end)
