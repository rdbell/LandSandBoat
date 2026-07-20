require('scripts/actions/mobskills/prishe_item_2')
describe('Prishe Item 2 mob skill', function()
    it('sets NONE message and returns 0', function()
        local skill = require('scripts/actions/mobskills/prishe_item_2')
        local msg = nil
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 0)
        assert(msg == xi.msg.basic.NONE)
    end)
end)
