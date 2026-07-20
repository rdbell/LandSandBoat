require('scripts/actions/mobskills/absorbing_kiss')
describe('Absorbing Kiss mob skill', function()
    it('drains one attribute by default and all seven as Pepper', function()
        local skill = require('scripts/actions/mobskills/absorbing_kiss')
        local drainAttr = xi.mobskills.mobDrainAttribute
        local origRandom = math.random
        local drains, message = {}, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { getPool = function() return 0 end }
        math.random = function(a, b)
            if a == xi.effect.STR_DOWN and b == xi.effect.CHR_DOWN then
                return xi.effect.STR_DOWN
            end
            return 7
        end
        xi.mobskills.mobDrainAttribute = function(_, _, effect, power, tick, duration)
            table.insert(drains, { effect, power, tick, duration })
            return xi.msg.basic.ATTR_DRAINED
        end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 1)
        assert(#drains == 1 and drains[1][1] == xi.effect.STR_DOWN and drains[1][2] == 10 and drains[1][3] == 3 and drains[1][4] == 120)
        assert(message == xi.msg.basic.ATTR_DRAINED)

        drains, message = {}, nil
        mob.getPool = function() return xi.mobPool.PEPPER end
        local call = 0
        math.random = function(a, b)
            assert(a == 7 and b == 8)
            call = call + 1
            return call % 2 == 0 and 8 or 7
        end
        local n = 0
        xi.mobskills.mobDrainAttribute = function(_, _, effect, power, tick, duration)
            table.insert(drains, { effect, power, tick, duration })
            n = n + 1
            if n <= 2 then return xi.msg.basic.ATTR_DRAINED end
            return xi.msg.basic.SKILL_MISS
        end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 2)
        xi.mobskills.mobDrainAttribute = drainAttr
        math.random = origRandom
        assert(#drains == 7 and drains[1][1] == xi.effect.STR_DOWN and drains[7][1] == xi.effect.CHR_DOWN)
        assert(drains[1][3] == 0 and drains[1][4] == 120)
        assert(message == xi.msg.basic.ATTR_DRAINED)
    end)
end)
