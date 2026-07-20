require('scripts/actions/mobskills/white_wind')
describe('White Wind mob skill', function()
    it('gates on UsedWhiteWind and heals floor(HP/7)*2 with SELF_HEAL_SECONDARY', function()
        local skill = require('scripts/actions/mobskills/white_wind')
        local healMove = xi.mobskills.mobHealMove
        local heal, message = nil, nil
        local mob = {
            getLocalVar = function(_, name)
                assert(name == 'UsedWhiteWind')
                return 0
            end,
            getHP = function() return 700 end,
        }
        local sk = { setMsg = function(_, v) message = v end }
        xi.mobskills.mobHealMove = function(entity, amount)
            heal = { entity, amount }
            return amount
        end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        mob.getLocalVar = function() return 1 end
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 200)
        xi.mobskills.mobHealMove = healMove
        assert(heal[1] == mob and heal[2] == 200)
        assert(message == xi.msg.basic.SELF_HEAL_SECONDARY)
    end)
end)
