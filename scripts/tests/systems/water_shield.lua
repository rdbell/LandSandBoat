require('scripts/actions/mobskills/water_shield')
describe('Water Shield mob skill', function()
    it('buffs EVASION_BOOST power 20 for 30s', function()
        local skill = require('scripts/actions/mobskills/water_shield')
        local buff = xi.mobskills.mobBuffMove
        local message, params = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        xi.mobskills.mobBuffMove = function(m, effect, power, tick, duration)
            params = { effect, power, tick, duration }
            return 101
        end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.EVASION_BOOST and message == 101)
        xi.mobskills.mobBuffMove = buff
        assert(params[1] == xi.effect.EVASION_BOOST and params[2] == 20 and params[4] == 30)
    end)
end)
