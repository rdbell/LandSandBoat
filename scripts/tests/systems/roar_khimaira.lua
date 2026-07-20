require('scripts/actions/mobskills/roar_khimaira')
describe('Roar Khimaira mob skill', function()
    it('always allows use and emits NONE with zero return', function()
        local roar = require('scripts/actions/mobskills/roar_khimaira')
        local message = nil
        local skill = { setMsg = function(_, value) message = value end }
        assert(roar.onMobSkillCheck({}, {}, skill) == 0)
        assert(roar.onMobWeaponSkill({}, {}, skill, {}) == 0)
        assert(message == xi.msg.basic.NONE)
    end)
end)
