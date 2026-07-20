require('scripts/actions/mobskills/x-potion')
describe('X-Potion mob skill', function()
    it('heals 150 with SELF_HEAL', function()
        local skill = require('scripts/actions/mobskills/x-potion')
        local healMove = xi.mobskills.mobHealMove
        local heal, message = nil, nil
        xi.mobskills.mobHealMove = function(target, amount)
            heal = { target, amount }
            return amount
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 150)
        xi.mobskills.mobHealMove = healMove
        assert(heal[2] == 150 and message == xi.msg.basic.SELF_HEAL)
    end)
end)
