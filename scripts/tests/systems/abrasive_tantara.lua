require('scripts/actions/mobskills/abrasive_tantara')

describe('Abrasive Tantara mob skill', function()
    it('rejects hornless Imps and otherwise requests fixed Amnesia', function()
        local calls, message = {}, nil
        local mob = {
            getAnimationSub = function() return 5 end,
            getSpecies = function() return xi.mobSpecies.IMP end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        local effectMove = xi.mobskills.mobStatusEffectMove
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            table.insert(calls, { effect, power, tick, duration })
            return 77
        end
        local tantara = require('scripts/actions/mobskills/abrasive_tantara')
        assert(tantara.onMobSkillCheck({}, mob, skill) == 1)
        mob.getAnimationSub = function() return 4 end
        assert(tantara.onMobSkillCheck({}, mob, skill) == 0)
        mob.getAnimationSub = function() return 5 end
        mob.getSpecies = function() return 0 end
        assert(tantara.onMobSkillCheck({}, mob, skill) == 0)
        assert(tantara.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.AMNESIA)
        xi.mobskills.mobStatusEffectMove = effectMove
        assert(calls[1][1] == xi.effect.AMNESIA and calls[1][2] == 1 and calls[1][3] == 0 and calls[1][4] == 60)
        assert(message == 77)
    end)
end)
