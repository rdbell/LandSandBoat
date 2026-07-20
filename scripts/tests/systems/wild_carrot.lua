require('scripts/actions/mobskills/wild_carrot')
describe('Wild Carrot mob skill', function()
    it('heals maxHP*104/1024 with SELF_HEAL', function()
        local skill = require('scripts/actions/mobskills/wild_carrot')
        local healMove = xi.mobskills.mobHealMove
        local heal, message = nil, nil
        local mob = { getMaxHP = function() return 1024 end }
        local sk = { setMsg = function(_, v) message = v end }
        xi.mobskills.mobHealMove = function(target, amount)
            heal = { target, amount }
            return amount
        end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 104)
        xi.mobskills.mobHealMove = healMove
        assert(heal[2] == 104 and message == xi.msg.basic.SELF_HEAL)
    end)
end)
