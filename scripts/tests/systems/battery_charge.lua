require('scripts/actions/mobskills/battery_charge')

describe('Battery Charge mob skill', function()
    it('clamps Refresh power from TP and forwards the buff message', function()
        local batteryCharge = require('scripts/actions/mobskills/battery_charge')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local cases = {
            { tp = 0, power = 1 },
            { tp = 1000, power = 1 },
            { tp = 1500, power = 1.5 },
            { tp = 3000, power = 3 },
            { tp = 5000, power = 3 },
        }
        local mob = {}

        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            assert(target == mob and effect == xi.effect.REFRESH)
            assert(tick == 3 and duration == 198)
            return power + 100
        end

        for _, case in ipairs(cases) do
            local message = nil
            local skill = {
                getTP = function() return case.tp end,
                setMsg = function(_, value) message = value end,
            }

            assert(batteryCharge.onMobSkillCheck(nil, mob, skill) == 0)
            assert(batteryCharge.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.REFRESH)
            assert(message == case.power + 100)
        end

        xi.mobskills.mobBuffMove = originalBuffMove
    end)
end)
