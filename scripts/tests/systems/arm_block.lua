require('scripts/actions/mobskills/arm_block')
describe('Arm Block mob skill', function()
    it('gates on DEFENSE_BOOST and applies power 25 with TP duration', function()
        local skill = require('scripts/actions/mobskills/arm_block')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, message = nil, nil
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
            return 50
        end
        local mob = {
            hasStatusEffect = function(_, effect)
                return effect == xi.effect.DEFENSE_BOOST
            end,
        }
        local sk = {
            setMsg = function(_, v) message = v end,
            getTP = function() return 1000 end,
        }
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        mob.hasStatusEffect = function() return false end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.DEFENSE_BOOST)
        xi.mobskills.mobBuffMove = buffMove
        assert(buff[1] == xi.effect.DEFENSE_BOOST and buff[2] == 25 and buff[4] == 600)
        assert(message == 50)
    end)
end)
