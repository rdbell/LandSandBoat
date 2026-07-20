require('scripts/actions/mobskills/booming_bombination')
describe('Booming Bombination mob skill', function()
    it('applies Plague, Defense Down, and Magic Def Down with shared duration', function()
        local skill = require('scripts/actions/mobskills/booming_bombination')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local statuses, msg = {}, nil
        local origRandom = math.random
        math.random = function(a, b)
            if a == 60 and b == 180 then return 90 end
            return origRandom(a, b)
        end
        local skillObj = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            statuses[#statuses + 1] = { effect, power, tick, duration }
            return 242
        end
        assert(skill.onMobSkillCheck({}, {}, {}) == 0)
        assert(skill.onMobWeaponSkill({}, {}, skillObj, {}) == xi.effect.MAGIC_DEF_DOWN)
        assert(#statuses == 3)
        assert(statuses[1][1] == xi.effect.PLAGUE and statuses[1][2] == 10 and statuses[1][4] == 90)
        assert(statuses[2][1] == xi.effect.DEFENSE_DOWN and statuses[2][4] == 90)
        assert(statuses[3][1] == xi.effect.MAGIC_DEF_DOWN and statuses[3][4] == 90)
        assert(msg == 242)
        math.random = origRandom
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
