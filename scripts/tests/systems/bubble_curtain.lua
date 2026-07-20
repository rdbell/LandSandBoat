require('scripts/actions/mobskills/bubble_curtain')
describe('Bubble Curtain mob skill', function()
    it('applies target SHELL buff', function()
        local skill = require('scripts/actions/mobskills/bubble_curtain')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, msg, buffTarget = nil, nil, nil
        local target = {}
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobBuffMove = function(t, effect, power, tick, duration)
            buffTarget = t
            buff = { effect, power, tick, duration }
            return 100
        end
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.SHELL)
        assert(buffTarget == target and buff[1] == xi.effect.SHELL and buff[2] == 5000 and buff[4] == 180)
        assert(msg == 100)
        xi.mobskills.mobBuffMove = buffMove
    end)
end)
