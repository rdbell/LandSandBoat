require('scripts/actions/mobskills/rising_swell')
describe('Rising Swell mob skill', function()
    it('unequips MAIN through BACK on PCs only and emits NONE', function()
        local swell = require('scripts/actions/mobskills/rising_swell')
        local slots, message = {}, nil
        local isPC = true
        local target = {
            isPC = function() return isPC end,
            unequipItem = function(_, slot) slots[#slots+1] = slot end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        assert(swell.onMobSkillCheck(target, {}, skill) == 0)
        assert(swell.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(#slots == 16 and slots[1] == xi.slot.MAIN and slots[#slots] == xi.slot.BACK)
        assert(message == xi.msg.basic.NONE)
        isPC, slots = false, {}
        swell.onMobWeaponSkill({}, target, skill, {})
        assert(#slots == 0)
    end)
end)
