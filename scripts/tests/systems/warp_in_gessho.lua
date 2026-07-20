require('scripts/actions/mobskills/warp_in_gessho')
describe('Warp In Gessho mob skill', function()
    it('skill check 0 and sets NONE', function()
        local skill = require('scripts/actions/mobskills/warp_in_gessho')
        local msg = nil
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 0)
        assert(msg == xi.msg.basic.NONE)
    end)
end)
