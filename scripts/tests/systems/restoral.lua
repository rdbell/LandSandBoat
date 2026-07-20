require('scripts/actions/mobskills/restoral')
describe('Restoral mob skill', function()
    it('heals with sampled base amount and multiplies for Armed Gears', function()
        local restoral = require('scripts/actions/mobskills/restoral')
        local heal, random = xi.mobskills.mobHealMove, math.random
        local amount, message, pool = nil, nil, 0
        local mob = { getPool = function() return pool end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobHealMove = function(_, value) amount = value; return value end
        math.random = function(low, high) assert(low==900 and high==1400); return 1000 end
        assert(restoral.onMobSkillCheck({}, mob, skill) == 0)
        assert(restoral.onMobWeaponSkill(mob, {}, skill, {}) == 1000)
        assert(amount == 1000 and message == xi.msg.basic.SELF_HEAL)
        pool = xi.mobPool.ARMED_GEARS
        assert(restoral.onMobWeaponSkill(mob, {}, skill, {}) == 2500)
        xi.mobskills.mobHealMove, math.random = heal, random
        assert(amount == 2500)
    end)
end)
