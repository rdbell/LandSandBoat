require('scripts/actions/mobskills/torpefying_charge')
describe('Torpefying Charge mob skill', function()
    it('applies paralysis gaze for 120s', function()
        local skill = require('scripts/actions/mobskills/torpefying_charge')
        local gaze = xi.mobskills.mobGazeMove
        local message, gazeParams = nil, nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        xi.mobskills.mobGazeMove = function(...) gazeParams = { ... }; return 456 end
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.PARALYSIS and message == 456)
        xi.mobskills.mobGazeMove = gaze
        assert(gazeParams[3] == xi.effect.PARALYSIS and gazeParams[4] == 15 and gazeParams[6] == 120)
    end)
end)
