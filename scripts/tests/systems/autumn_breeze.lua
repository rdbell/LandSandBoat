require('scripts/actions/mobskills/autumn_breeze')
describe('Autumn Breeze mob skill', function()
    it('heals random 100-400 with SELF_HEAL', function()
        local skill = require('scripts/actions/mobskills/autumn_breeze')
        local healMove = xi.mobskills.mobHealMove
        local origRandom = math.random
        local heal, message = nil, nil
        math.random = function(a, b)
            assert(a == 100 and b == 400)
            return 250
        end
        xi.mobskills.mobHealMove = function(target, amount)
            heal = { target, amount }
            return amount
        end
        local sk = { setMsg = function(_, v) message = v end }
        local mob = {}
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 250)
        math.random = origRandom
        xi.mobskills.mobHealMove = healMove
        assert(heal[1] == mob and heal[2] == 250 and message == xi.msg.basic.SELF_HEAL)
    end)
end)
