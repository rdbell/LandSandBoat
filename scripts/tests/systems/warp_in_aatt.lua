require('scripts/actions/mobskills/warp_in_aatt')
describe('Warp In Aatt mob skill', function()
    it('skill check 1 and sets NONE', function()
        local skill = require('scripts/actions/mobskills/warp_in_aatt')
        local msg = nil
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 1)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 0)
        assert(msg == xi.msg.basic.NONE)
    end)
end)
