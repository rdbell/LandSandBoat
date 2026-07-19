require('scripts/actions/mobskills/death_trap')

describe('Death Trap mob skill', function()
    it('always attempts Stun and Poison, resets enmity, and reports Stun before Poison', function()
        local effects, resetTarget, message = {}, nil, nil
        local mob = { resetEnmity = function(_, target) resetTarget = target end }
        local target = {}
        local skill = { setMsg = function(_, value) message = value end }
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local random = math.random
        local results = { xi.msg.basic.SKILL_ENFEEB_IS, xi.msg.basic.SKILL_ENFEEB_IS }
        math.random = function(minimum, maximum)
            assert(minimum == 10 and maximum == 15)
            return 12
        end
        xi.mobskills.mobStatusEffectMove = function(...)
            effects[#effects + 1] = { ... }
            return results[#effects]
        end

        local trap = require('scripts/actions/mobskills/death_trap')
        assert(trap.onMobSkillCheck({}, {}, {}) == 0)
        assert(trap.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.STUN)
        assert(#effects == 2 and resetTarget == target and message == xi.msg.basic.SKILL_ENFEEB_IS)
        assert(effects[1][3] == xi.effect.STUN and effects[1][4] == 1 and effects[1][5] == 0 and effects[1][6] == 12)
        assert(effects[2][3] == xi.effect.POISON and effects[2][4] == 10 and effects[2][5] == 0 and effects[2][6] == 300)

        effects, resetTarget, message = {}, nil, nil
        results = { xi.msg.basic.SKILL_NO_EFFECT, xi.msg.basic.SKILL_ENFEEB_IS }
        assert(trap.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.POISON)
        assert(#effects == 2 and resetTarget == target and message == xi.msg.basic.SKILL_ENFEEB_IS)

        effects, resetTarget, message = {}, nil, nil
        results = { xi.msg.basic.SKILL_NO_EFFECT, xi.msg.basic.SKILL_NO_EFFECT }
        assert(trap.onMobWeaponSkill(mob, target, skill, {}) == xi.effect.NONE)
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        math.random = random
        assert(#effects == 2 and resetTarget == target and message == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
