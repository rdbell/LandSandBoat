require('scripts/actions/mobskills/temporal_shift')
describe('Temporal Shift mob skill', function()
    it('applies Stun for 5s', function()
        local skill = require('scripts/actions/mobskills/temporal_shift')
        local msg = nil
        local orig = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(_, _, e, power, tick, dur)
            assert(e == xi.effect.STUN and power == 1 and dur == 5)
            return 242
        end
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.STUN)
        assert(msg == 242)
        xi.mobskills.mobStatusEffectMove = orig
    end)
end)
