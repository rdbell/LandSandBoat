require('scripts/actions/mobskills/hypnic_lamp')
describe('Hypnic Lamp mob skill', function()
    it('admits intact eyestalks and gazes Sleep with TP duration', function()
        local skill = require('scripts/actions/mobskills/hypnic_lamp')
        local gazeMove = xi.mobskills.mobGazeMove
        local status, msg, anim = nil, nil, 1
        local mob = { getAnimationSub = function() return anim end }
        local sk = { setMsg = function(_, m) msg = m end, getTP = function() return 1000 end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        anim = 0
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobGazeMove = function(_,_,e,p,t,d) status={e,p,t,d}; return 242 end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.SLEEP_I)
        assert(status[1] == xi.effect.SLEEP_I and status[4] == xi.mobskills.calculateDuration(1000, 120, 180))
        assert(msg == 242)
        xi.mobskills.mobGazeMove = gazeMove
    end)
end)
