require('scripts/actions/mobskills/prishe_item_2')
describe('Prishe Item 2 mob skill', function()
    it('always allows use and emits NONE with zero return', function()
        local item = require('scripts/actions/mobskills/prishe_item_2')
        local message = nil
        local skill = { setMsg = function(_, value) message = value end }
        assert(item.onMobSkillCheck({}, {}, skill) == 0)
        assert(item.onMobWeaponSkill({}, {}, skill, {}) == 0)
        assert(message == xi.msg.basic.NONE)
    end)
end)
