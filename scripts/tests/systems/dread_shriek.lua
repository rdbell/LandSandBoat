require('scripts/actions/mobskills/dread_shriek')

describe('Dread Shriek mob skill', function()
    it('uses random Paralysis values and applies NM and Cyranuce potency overrides', function()
        local dreadShriek = require('scripts/actions/mobskills/dread_shriek')
        local statusMove, random = xi.mobskills.mobStatusEffectMove, math.random
        local request, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            request = { effect, power, tick, duration }
            return 777
        end

        assert(dreadShriek.onMobSkillCheck({}, {}, {}) == 0)

        local values = { 45, 37 }
        math.random = function(low, high)
            assert((low == 40 and high == 50) or (low == 30 and high == 60))
            local value = table.remove(values, 1)
            return value
        end
        assert(dreadShriek.onMobWeaponSkill({ isNM = function() return false end, getPool = function() return 0 end }, {}, skill, {}) == xi.effect.PARALYSIS)
        assert(request[1] == xi.effect.PARALYSIS and request[2] == 45 and request[3] == 0 and request[4] == 37 and message == 777)

        values = { 42, 59 }
        assert(dreadShriek.onMobWeaponSkill({ isNM = function() return true end, getPool = function() return 0 end }, {}, skill, {}) == xi.effect.PARALYSIS)
        assert(request[1] == xi.effect.PARALYSIS and request[2] == 90 and request[3] == 0 and request[4] == 59 and message == 777)

        values = { 40, 30 }
        assert(dreadShriek.onMobWeaponSkill({ isNM = function() return true end, getPool = function() return xi.mobPool.CYRANUCE_M_CUTAULEON end }, {}, skill, {}) == xi.effect.PARALYSIS)
        xi.mobskills.mobStatusEffectMove, math.random = statusMove, random
        assert(request[1] == xi.effect.PARALYSIS and request[2] == 100 and request[3] == 0 and request[4] == 30 and message == 777)
    end)
end)
