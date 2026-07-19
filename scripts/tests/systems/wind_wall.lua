require('scripts/actions/mobskills/wind_wall')

describe('Wind Wall mob skill', function()
    it('rejects an existing Evasion Boost and scales duration from TP', function()
        local windWall = require('scripts/actions/mobskills/wind_wall')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local cases = { { 0, 15 }, { 1000, 15 }, { 1500, 37.5 }, { 2000, 60 }, { 3000, 105 } }
        local mob = { hasStatusEffect = function(_, effect) return effect == xi.effect.EVASION_BOOST end }
        local skill = { getTP = function() return 0 end, setMsg = function() end }
        assert(windWall.onMobSkillCheck(nil, mob, skill) == 1)
        mob.hasStatusEffect = function() return false end
        assert(windWall.onMobSkillCheck(nil, mob, skill) == 0)

        xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
            assert(target == mob and effect == xi.effect.EVASION_BOOST)
            assert(power == 200 and tick == 0)
            return duration
        end
        for _, case in ipairs(cases) do
            local message = nil
            skill.getTP = function() return case[1] end
            skill.setMsg = function(_, value) message = value end
            assert(windWall.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.EVASION_BOOST)
            assert(message == case[2])
        end
        xi.mobskills.mobBuffMove = originalBuffMove
    end)
end)
