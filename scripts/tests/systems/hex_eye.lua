require('scripts/actions/mobskills/hex_eye')
describe('Hex Eye mob skill', function()
    it('applies gaze Paralysis', function()
        local skill = require('scripts/actions/mobskills/hex_eye')
        local gazeMove = xi.mobskills.mobGazeMove
        local status, msg = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobGazeMove = function(_,_,e,p,t,d) status={e,p,t,d}; return 242 end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.PARALYSIS)
        assert(status[1] == xi.effect.PARALYSIS and status[2] == 60 and status[4] == 120 and msg == 242)
        xi.mobskills.mobGazeMove = gazeMove
    end)
end)
