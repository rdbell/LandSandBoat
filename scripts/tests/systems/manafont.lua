require('scripts/actions/mobskills/manafont')

describe('Manafont mob skill', function()
    it('requests Manafont, sets the uses message, and returns the effect', function()
        local manafont = require('scripts/actions/mobskills/manafont')
        local buff = xi.mobskills.mobBuffMove
        local calls, message = {}, nil
        local mob, target, action = {}, {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...) table.insert(calls, { ... }) end

        assert(manafont.onMobSkillCheck(target, mob, skill) == 0)
        assert(manafont.onMobWeaponSkill(mob, target, skill, action) == xi.effect.MANAFONT)
        assert(#calls == 1 and calls[1][1] == mob and calls[1][2] == xi.effect.MANAFONT and calls[1][3] == 1 and calls[1][4] == 0 and calls[1][5] == 60 and message == xi.msg.basic.USES)

        xi.mobskills.mobBuffMove = buff
    end)
end)
