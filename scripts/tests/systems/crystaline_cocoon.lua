require('scripts/actions/mobskills/crystaline_cocoon')

describe('Crystaline Cocoon mob skill', function()
    it('applies Protect then Shell and forwards only the Protect message', function()
        local crystalineCocoon = require('scripts/actions/mobskills/crystaline_cocoon')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buffs, message = {}, nil
        local mob = {}
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            table.insert(buffs, { target, effect, power, tick, duration })
            return #buffs == 1 and 123 or 456
        end

        assert(crystalineCocoon.onMobSkillCheck(nil, mob, skill) == 0)
        assert(crystalineCocoon.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.PROTECT)

        xi.mobskills.mobBuffMove = originalBuffMove

        assert(#buffs == 2)
        assert(buffs[1][1] == mob and buffs[1][2] == xi.effect.PROTECT)
        assert(buffs[1][3] == 50 and buffs[1][4] == 0 and buffs[1][5] == 120)
        assert(buffs[2][1] == mob and buffs[2][2] == xi.effect.SHELL)
        assert(buffs[2][3] == 781 and buffs[2][4] == 0 and buffs[2][5] == 120)
        assert(message == 123)
    end)
end)
