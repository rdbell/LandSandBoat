require('scripts/actions/mobskills/di_glow')

describe('Di Glow mob skill', function()
    it('always admits, suppresses its combat message, and returns no damage', function()
        local message = nil
        local glow = require('scripts/actions/mobskills/di_glow')
        local skill = { setMsg = function(_, value) message = value end }

        assert(glow.onMobSkillCheck({}, {}, {}) == 0)
        assert(glow.onMobWeaponSkill({}, {}, skill, {}) == 0)
        assert(message == xi.msg.basic.NONE)
    end)
end)
