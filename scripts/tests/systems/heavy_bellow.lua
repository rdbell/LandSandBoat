require('scripts/actions/mobskills/heavy_bellow')
describe('Heavy Bellow mob skill', function()
    it('applies Stun', function()
        local skill = require('scripts/actions/mobskills/heavy_bellow')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local status, msg = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d) status={e,p,t,d}; return 242 end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.STUN)
        assert(status[1] == xi.effect.STUN and status[4] == 6 and msg == 242)
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
