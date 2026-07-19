require('scripts/actions/mobskills/warm-up')

describe('Warm-up mob skill', function()
    it('only reapplies effects already present, preserving Accuracy then Evasion order', function()
        local warmUp = require('scripts/actions/mobskills/warm-up')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local cases = {
            { accuracy = false, evasion = false, effects = {}, result = nil, message = nil },
            { accuracy = true, evasion = false, effects = { xi.effect.ACCURACY_BOOST }, result = xi.effect.ACCURACY_BOOST, message = 101 },
            { accuracy = false, evasion = true, effects = { xi.effect.EVASION_BOOST }, result = xi.effect.EVASION_BOOST, message = 101 },
            { accuracy = true, evasion = true, effects = { xi.effect.ACCURACY_BOOST, xi.effect.EVASION_BOOST }, result = xi.effect.EVASION_BOOST, message = 102 },
        }
        local mob = {}
        local skill = {}
        assert(warmUp.onMobSkillCheck(nil, mob, skill) == 0)
        for _, case in ipairs(cases) do
            local buffs, message = {}, nil
            mob.hasStatusEffect = function(_, effect)
                return (effect == xi.effect.ACCURACY_BOOST and case.accuracy) or (effect == xi.effect.EVASION_BOOST and case.evasion)
            end
            skill.setMsg = function(_, value) message = value end
            xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
                table.insert(buffs, { target, effect, power, tick, duration })
                return #buffs + 100
            end
            assert(warmUp.onMobWeaponSkill(mob, nil, skill, nil) == case.result)
            assert(#buffs == #case.effects and message == case.message)
            for index, effect in ipairs(case.effects) do
                assert(buffs[index][1] == mob and buffs[index][2] == effect)
                assert(buffs[index][3] == 40 and buffs[index][4] == 0 and buffs[index][5] == 60)
            end
        end
        xi.mobskills.mobBuffMove = originalBuffMove
    end)
end)
