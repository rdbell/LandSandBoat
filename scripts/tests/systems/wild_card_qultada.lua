require('scripts/actions/mobskills/wild_card_qultada')
describe('Wild Card Qultada mob skill', function()
    it('sets USES message with no further effect', function()
        local skill = require('scripts/actions/mobskills/wild_card_qultada')
        local message = nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        skill.onMobWeaponSkill({}, {}, sk, {})
        assert(message == xi.msg.basic.USES)
    end)
end)
