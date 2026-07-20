require('scripts/actions/mobskills/triumphant_roar')
describe('Triumphant Roar mob skill', function()
    it('requires standing animation and buffs ATTACK_BOOST', function()
        local skill = require('scripts/actions/mobskills/triumphant_roar')
        local buff = xi.mobskills.mobBuffMove
        local message, buffParams = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { getAnimationSub = function() return 0 end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        mob.getAnimationSub = function() return 4 end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobBuffMove = function(m, effect, power, tick, duration)
            buffParams = { effect, power, tick, duration }
            return 101
        end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.ATTACK_BOOST and message == 101)
        xi.mobskills.mobBuffMove = buff
        assert(buffParams[1] == xi.effect.ATTACK_BOOST and buffParams[2] == 15 and buffParams[4] == 90)
    end)
end)
