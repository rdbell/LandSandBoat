require('scripts/actions/mobskills/bubble_armor')
describe('Bubble Armor mob skill', function()
    it('applies self SHELL buff', function()
        local skill = require('scripts/actions/mobskills/bubble_armor')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, msg, buffTarget = nil, nil, nil
        local mob = {}
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobBuffMove = function(m, effect, power, tick, duration)
            buffTarget = m
            buff = { effect, power, tick, duration }
            return 100
        end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.SHELL)
        assert(buffTarget == mob and buff[1] == xi.effect.SHELL and buff[2] == 5000 and buff[4] == 180)
        assert(msg == 100)
        xi.mobskills.mobBuffMove = buffMove
    end)
end)
