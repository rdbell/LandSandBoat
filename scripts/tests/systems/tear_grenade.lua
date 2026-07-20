require('scripts/actions/mobskills/tear_grenade')
describe('Tear Grenade mob skill', function()
    it('applies Blindness then Silence via gaze', function()
        local skill = require('scripts/actions/mobskills/tear_grenade')
        local msg, effects = nil, {}
        local orig = xi.mobskills.mobGazeMove
        xi.mobskills.mobGazeMove = function(_, _, e, power, tick, dur)
            effects[#effects+1] = { e, power, tick, dur }
            return 242
        end
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.SILENCE)
        assert(effects[1][1] == xi.effect.BLINDNESS and effects[2][1] == xi.effect.SILENCE and msg == 242)
        xi.mobskills.mobGazeMove = orig
    end)
end)
