require('scripts/actions/mobskills/damsel_memento')

describe('Damsel Memento mob skill', function()
    it('allows rolls from one through five out of one hundred', function()
        local random = math.random
        local skill = require('scripts/actions/mobskills/damsel_memento')
        math.random = function(lower, upper)
            assert(lower == 1 and upper == 100)
            return 1
        end
        assert(skill.onMobSkillCheck({}, {}, {}) == 0)
        math.random = function() return 5 end
        assert(skill.onMobSkillCheck({}, {}, {}) == 0)
        math.random = function() return 6 end
        assert(skill.onMobSkillCheck({}, {}, {}) == 1)
        math.random = random
    end)

    it('removes waltzable and erasable effects then requests a five-percent heal', function()
        local removals, message, heal = {}, nil, nil
        local mob = {
            getMaxHP = function() return 1000 end,
            delStatusEffectsByFlag = function(_, flag, silent) table.insert(removals, { flag, silent }) end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        local healMove = xi.mobskills.mobHealMove
        xi.mobskills.mobHealMove = function(_, amount) heal = amount; return 50 end
        local memento = require('scripts/actions/mobskills/damsel_memento')
        assert(memento.onMobWeaponSkill(mob, {}, skill, {}) == 50)
        xi.mobskills.mobHealMove = healMove
        assert(removals[1][1] == xi.effectFlag.WALTZABLE and removals[1][2] == false)
        assert(removals[2][1] == xi.effectFlag.ERASABLE and removals[2][2] == false)
        assert(message == xi.msg.basic.SELF_HEAL_NOHP and heal == 50)
    end)
end)
