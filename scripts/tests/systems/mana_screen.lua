require('scripts/actions/mobskills/mana_screen')

describe('Mana Screen mob skill', function()
    it('requests Magic Shield, sets the returned message, and returns the effect', function()
        local screen = require('scripts/actions/mobskills/mana_screen')
        local buff = xi.mobskills.mobBuffMove
        local message, calls = nil, {}
        local mob, target, action = {}, {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) table.insert(calls, { ... }); return 456 end

        assert(screen.onMobSkillCheck(target, mob, skill) == 0)
        assert(screen.onMobWeaponSkill(mob, target, skill, action) == xi.effect.MAGIC_SHIELD)
        assert(#calls == 1 and calls[1][1] == mob and calls[1][2] == xi.effect.MAGIC_SHIELD and calls[1][3] == 1 and calls[1][4] == 0 and calls[1][5] == 60 and message == 456)

        xi.mobskills.mobBuffMove = buff
    end)
end)
