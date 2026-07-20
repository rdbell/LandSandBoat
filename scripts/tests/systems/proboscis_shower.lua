require('scripts/actions/mobskills/proboscis_shower')

describe('Proboscis Shower mob skill', function()
    it('heals with default potency sampling and SELF_HEAL message', function()
        local shower = require('scripts/actions/mobskills/proboscis_shower')
        local heal, random = xi.mobskills.mobHealMove, math.random
        local amount, message, skillParam = nil, nil, 0
        local mob = { getMaxHP = function() return 1000 end }
        local skill = {
            getParam = function() return skillParam end,
            setMsg = function(_, value) message = value end,
        }
        xi.mobskills.mobHealMove = function(_, value)
            amount = value
            return value
        end
        local randomArgs = nil
        math.random = function(low, high)
            randomArgs = { low, high }
            -- Default potency 13: potency/4 == 3.25 in Lua; return full reduction of 3.
            return 3
        end
        assert(shower.onMobSkillCheck({}, mob, skill) == 0)
        local healed = shower.onMobWeaponSkill(mob, {}, skill, {})
        xi.mobskills.mobHealMove, math.random = heal, random
        assert(randomArgs and randomArgs[1] == 0 and randomArgs[2] == 13 / 4)
        -- (13 - 3) / 100 * 1000 = 100
        assert(healed == 100 and amount == 100 and message == xi.msg.basic.SELF_HEAL)
    end)
end)
