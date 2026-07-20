require('scripts/actions/mobskills/lunar_cry')

describe('Lunar Cry mob skill', function()
    it('uses the moon-cycle accuracy and complementary evasion penalties', function()
        local lunarCry = require('scripts/actions/mobskills/lunar_cry')
        local status = xi.mobskills.mobStatusEffectMove
        local calls, message = {}, nil
        local mob, target = {}, {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(...) table.insert(calls, { ... }) end
        local cycleBuffs =
        {
            [xi.moonCycle.NEW_MOON] = 1,
            [xi.moonCycle.LESSER_WAXING_CRESCENT] = 6,
            [xi.moonCycle.GREATER_WAXING_CRESCENT] = 11,
            [xi.moonCycle.FIRST_QUARTER] = 16,
            [xi.moonCycle.LESSER_WAXING_GIBBOUS] = 21,
            [xi.moonCycle.GREATER_WAXING_GIBBOUS] = 26,
            [xi.moonCycle.FULL_MOON] = 31,
            [xi.moonCycle.GREATER_WANING_GIBBOUS] = 26,
            [xi.moonCycle.LESSER_WANING_GIBBOUS] = 21,
            [xi.moonCycle.THIRD_QUARTER] = 16,
            [xi.moonCycle.GREATER_WANING_CRESCENT] = 11,
            [xi.moonCycle.LESSER_WANING_CRESCENT] = 6,
        }
        local currentCycle = getVanadielMoonCycle()
        local accuracyDown = cycleBuffs[currentCycle]

        assert(lunarCry.onMobSkillCheck(target, mob, skill) == 0)
        assert(lunarCry.onMobWeaponSkill(mob, target, skill, {}) == 0)
        assert(calls[1][1] == mob and calls[1][2] == target and calls[1][3] == xi.effect.ACCURACY_DOWN and calls[1][4] == accuracyDown and calls[1][5] == 0 and calls[1][6] == 180)
        assert(calls[2][1] == mob and calls[2][2] == target and calls[2][3] == xi.effect.EVASION_DOWN and calls[2][4] == 32 - accuracyDown and calls[2][5] == 0 and calls[2][6] == 180)
        assert(message == xi.msg.basic.SKILL_ENFEEB_2)

        xi.mobskills.mobStatusEffectMove = status
    end)
end)
