require('scripts/actions/mobskills/zephyr_mantle')
describe('Zephyr Mantle mob skill', function()
    it('applies Blink with random power 4-10 for 180s', function()
        local skill = require('scripts/actions/mobskills/zephyr_mantle')
        local buffMove = xi.mobskills.mobBuffMove
        local origRandom = math.random
        local buff, message = nil, nil
        math.random = function(a, b)
            assert(a == 4 and b == 10)
            return 7
        end
        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
            return 55
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.BLINK)
        math.random = origRandom
        xi.mobskills.mobBuffMove = buffMove
        assert(buff[1] == xi.effect.BLINK and buff[2] == 7 and buff[3] == 0 and buff[4] == 180)
        assert(message == 55)
    end)
end)
