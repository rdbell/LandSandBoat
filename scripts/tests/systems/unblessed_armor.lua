require('scripts/actions/mobskills/unblessed_armor')
describe('Unblessed Armor mob skill', function()
    it('buffs SHELL power 5000 for 180s', function()
        local skill = require('scripts/actions/mobskills/unblessed_armor')
        local buff = xi.mobskills.mobBuffMove
        local message, params = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        xi.mobskills.mobBuffMove = function(m, effect, power, tick, duration)
            params = { effect, power, tick, duration }
            return 101
        end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.SHELL and message == 101)
        xi.mobskills.mobBuffMove = buff
        assert(params[1] == xi.effect.SHELL and params[2] == 5000 and params[4] == 180)
    end)
end)
