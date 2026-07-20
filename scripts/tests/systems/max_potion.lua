require('scripts/actions/mobskills/max_potion')

describe('Max Potion mob skill', function()
    it('allows use, requests a fixed self-heal, and returns the actual heal', function()
        local potion = require('scripts/actions/mobskills/max_potion')
        local healMove = xi.mobskills.mobHealMove
        local receiver, amount, message = nil, nil, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobHealMove = function(who, value)
            receiver, amount = who, value
            return 456
        end
        assert(potion.onMobSkillCheck(target, mob, skill) == 0)
        assert(potion.onMobWeaponSkill(mob, target, skill, {}) == 456)
        xi.mobskills.mobHealMove = healMove
        assert(receiver == target and amount == 500 and message == xi.msg.basic.SELF_HEAL)
    end)
end)
