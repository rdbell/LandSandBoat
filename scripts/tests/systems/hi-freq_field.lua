require('scripts/actions/mobskills/hi-freq_field')
describe('Hi-Freq Field mob skill', function()
    it('applies Evasion Down', function()
        local skill = require('scripts/actions/mobskills/hi-freq_field')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local status, msg = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) status={e,p,t,d}; return 242 end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.EVASION_DOWN)
        assert(status[1] == xi.effect.EVASION_DOWN and status[2] == 40 and status[4] == 180 and msg == 242)
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
