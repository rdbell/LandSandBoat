require('scripts/actions/mobskills/winds_of_oblivion')
describe('Winds of Oblivion mob skill', function()
    it('applies Amnesia power 30 duration 75 with subType 100', function()
        local skill = require('scripts/actions/mobskills/winds_of_oblivion')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local args, message = nil, nil
        xi.mobskills.mobStatusEffectMove = function(mob, target, effect, power, tick, duration, subType)
            args = { mob, target, effect, power, tick, duration, subType }
            return 242
        end
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == xi.effect.AMNESIA)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(args[3] == xi.effect.AMNESIA and args[4] == 30 and args[5] == 0 and args[6] == 75 and args[7] == 100)
        assert(message == 242)
    end)
end)
