require('scripts/actions/mobskills/dukkeripen_heal')

describe('Dukkeripen Heal mob skill', function()
    it('allows Corsair only and forwards its random self-heal result as message', function()
        local heal = require('scripts/actions/mobskills/dukkeripen_heal')
        local healMove, random = xi.mobskills.mobHealMove, math.random
        local amount, message = nil, nil
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobHealMove = function(_, value) amount = value; return 123 end
        math.random = function(low, high) assert(low == 350 and high == 500); return 417 end
        assert(heal.onMobSkillCheck({}, { getMainJob = function() return xi.job.COR end }, {}) == 0)
        assert(heal.onMobSkillCheck({}, { getMainJob = function() return xi.job.WAR end }, {}) == 1)
        assert(heal.onMobWeaponSkill({}, {}, skill, {}) == 123)
        xi.mobskills.mobHealMove, math.random = healMove, random
        assert(amount == 417 and message == xi.msg.basic.SELF_HEAL)
    end)
end)
