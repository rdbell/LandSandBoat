require('scripts/actions/mobskills/prishe_item_1')
describe('Prishe Item 1 mob skill', function()
    it('always fails skill check and sets NONE', function()
        local skill = require('scripts/actions/mobskills/prishe_item_1')
        local msg = nil
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 1)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 0)
        assert(msg == xi.msg.basic.NONE)
    end)
end)
