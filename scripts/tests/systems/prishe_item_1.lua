require('scripts/actions/mobskills/prishe_item_1')
describe('Prishe Item 1 mob skill', function()
    it('always rejects use and emits NONE with zero return when forced', function()
        local item = require('scripts/actions/mobskills/prishe_item_1')
        local message = nil
        local skill = { setMsg = function(_, value) message = value end }
        assert(item.onMobSkillCheck({}, {}, skill) == 1)
        assert(item.onMobWeaponSkill({}, {}, skill, {}) == 0)
        assert(message == xi.msg.basic.NONE)
    end)
end)
