require('scripts/actions/mobskills/magic_fruit')

describe('Magic Fruit mob skill', function()
    it('sets the self-heal message and forwards 188/1024 of the caster maximum HP', function()
        local magicFruit = require('scripts/actions/mobskills/magic_fruit')
        local heal = xi.mobskills.mobHealMove
        local amount, message = nil, nil
        local mob = { getMaxHP = function() return 1024 end }
        local target = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobHealMove = function(actualTarget, value) assert(actualTarget == target); amount = value; return 456 end

        assert(magicFruit.onMobSkillCheck(target, mob, skill) == 0)
        assert(magicFruit.onMobWeaponSkill(mob, target, skill, {}) == 456)
        assert(message == xi.msg.basic.SELF_HEAL and amount == 188)

        xi.mobskills.mobHealMove = heal
    end)
end)
