require('scripts/actions/mobskills/torrent')
describe('Torrent mob skill', function()
    it('unequips MAIN..BACK and sets NONE message', function()
        local skill = require('scripts/actions/mobskills/torrent')
        local message, unequipped = nil, {}
        local sk = { setMsg = function(_, v) message = v end }
        local target = { unequipItem = function(_, slot) unequipped[#unequipped + 1] = slot end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == 0)
        assert(message == xi.msg.basic.NONE)
        assert(unequipped[1] == xi.slot.MAIN and unequipped[#unequipped] == xi.slot.BACK)
    end)
end)
