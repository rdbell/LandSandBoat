require('scripts/actions/mobskills/lightning_blade')

describe('Lightning Blade mob skill', function()
    it('forwards the Enthunder buff result to its message and returns Enthunder', function()
        local blade = require('scripts/actions/mobskills/lightning_blade')
        local buff = xi.mobskills.mobBuffMove
        local call, message = nil, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) call = { ... }; return 456 end

        assert(blade.onMobSkillCheck({}, mob, skill) == 0)
        assert(blade.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.ENTHUNDER)
        xi.mobskills.mobBuffMove = buff
        assert(call[1] == mob and call[2] == xi.effect.ENTHUNDER and call[3] == 65 and call[4] == 0 and call[5] == 30 and message == 456)
    end)
end)
