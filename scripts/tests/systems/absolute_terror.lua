require('scripts/actions/mobskills/absolute_terror')

describe('Absolute Terror mob skill', function()
    it('rejects each active special state, rear target, and animation-sub gate', function()
        local terror = require('scripts/actions/mobskills/absolute_terror')
        local active = {}
        local mob = {
            hasStatusEffect = function(_, effect) return active[effect] or false end,
            getAnimationSub = function() return 0 end,
        }
        local target = { isInfront = function() return true end }
        assert(terror.onMobSkillCheck(target, mob, {}) == 0)
        for _, effect in ipairs({ xi.effect.MIGHTY_STRIKES, xi.effect.SUPER_BUFF, xi.effect.INVINCIBLE, xi.effect.BLOOD_WEAPON }) do
            active[effect] = true
            assert(terror.onMobSkillCheck(target, mob, {}) == 1)
            active[effect] = nil
        end
        target.isInfront = function() return false end
        assert(terror.onMobSkillCheck(target, mob, {}) == 1)
        target.isInfront = function() return true end
        mob.getAnimationSub = function() return 1 end
        assert(terror.onMobSkillCheck(target, mob, {}) == 1)
    end)

    it('uses the correct random duration range for single-target and AoE applications', function()
        local random, effectMove = math.random, xi.mobskills.mobStatusEffectMove
        local ranges, calls, messages = {}, {}, {}
        math.random = function(lower, upper) table.insert(ranges, { lower, upper }); return upper end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            table.insert(calls, { effect, power, tick, duration })
            return 77
        end
        local skill = { isAoE = function() return false end, setMsg = function(_, value) table.insert(messages, value) end }
        local terror = require('scripts/actions/mobskills/absolute_terror')
        assert(terror.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.TERROR)
        skill.isAoE = function() return true end
        assert(terror.onMobWeaponSkill({}, {}, skill, {}) == xi.effect.TERROR)
        math.random, xi.mobskills.mobStatusEffectMove = random, effectMove
        assert(ranges[1][1] == 15 and ranges[1][2] == 45 and ranges[2][1] == 15 and ranges[2][2] == 45 and ranges[3][1] == 10 and ranges[3][2] == 18)
        assert(calls[1][1] == xi.effect.TERROR and calls[1][2] == 30 and calls[1][3] == 0 and calls[1][4] == 45)
        assert(calls[2][1] == xi.effect.TERROR and calls[2][2] == 30 and calls[2][3] == 0 and calls[2][4] == 18)
        assert(messages[1] == 77 and messages[2] == 77)
    end)
end)
