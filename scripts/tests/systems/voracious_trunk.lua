require('scripts/actions/mobskills/voracious_trunk')
describe('Voracious Trunk mob skill', function()
    it('drains one status effect and returns 1', function()
        local skill = require('scripts/actions/mobskills/voracious_trunk')
        local drain = xi.mobskills.mobDrainStatusEffectMove
        local message = nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        xi.mobskills.mobDrainStatusEffectMove = function() return 456 end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 1 and message == 456)
        xi.mobskills.mobDrainStatusEffectMove = drain
    end)
end)
