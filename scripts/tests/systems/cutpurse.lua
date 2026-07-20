require('scripts/actions/mobskills/cutpurse')
describe('Cutpurse mob skill', function()
    it('unequips first equipped shuffled slot for PCs', function()
        local skill = require('scripts/actions/mobskills/cutpurse')
        local msg, unequipped = nil, {}
        local shuffle = utils.shuffle
        utils.shuffle = function(t)
            -- force MAIN first for deterministic test
            return { xi.slot.MAIN, xi.slot.SUB, xi.slot.BACK }
        end
        local target = {
            isPC = function() return false end,
            hasSlotEquipped = function(_, s) return s == xi.slot.MAIN end,
            unequipItem = function(_, s) unequipped[#unequipped+1] = s end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        skill.onMobWeaponSkill({}, target, sk, {})
        assert(msg == xi.msg.basic.SKILL_NO_EFFECT)
        target.isPC = function() return true end
        msg, unequipped = nil, {}
        skill.onMobWeaponSkill({}, target, sk, {})
        assert(unequipped[1] == xi.slot.MAIN and unequipped[2] == xi.slot.SUB)
        utils.shuffle = shuffle
    end)
end)
