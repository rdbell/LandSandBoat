require('scripts/actions/mobskills/water_wall')
describe('Water Wall mob skill', function()
    it('buffs target DEFENSE_BOOST power 100 for 60s', function()
        local skill = require('scripts/actions/mobskills/water_wall')
        local buff = xi.mobskills.mobBuffMove
        local message, params = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local target = {}
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        xi.mobskills.mobBuffMove = function(t, effect, power, tick, duration)
            params = { t, effect, power, tick, duration }
            return 101
        end
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.DEFENSE_BOOST and message == 101)
        xi.mobskills.mobBuffMove = buff
        assert(params[1] == target and params[2] == xi.effect.DEFENSE_BOOST and params[3] == 100 and params[5] == 60)
    end)
end)
