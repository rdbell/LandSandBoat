require('scripts/actions/mobskills/spring_water')
describe('Spring Water mob skill', function()
    it('heals with level/TP/HP formula and SELF_HEAL message', function()
        local water = require('scripts/actions/mobskills/spring_water')
        local heal = xi.mobskills.mobHealMove
        local healAmt, message = nil, nil
        local mob = {
            getMainLvl = function() return 50 end,
            getHP = function() return 500 end,
            getMaxHP = function() return 1000 end,
        }
        local skill = {
            getTP = function() return 1000 end,
            setMsg = function(_, value) message = value end,
        }
        xi.mobskills.mobHealMove = function(_, amount) healAmt = amount; return amount end
        assert(water.onMobSkillCheck({}, mob, skill) == 0)
        -- base = 50 + 2*50*1 = 150; mult = 1+(1-0.5)*5 = 3.5; amount = 525
        assert(water.onMobWeaponSkill(mob, {}, skill, {}) == 525)
        xi.mobskills.mobHealMove = heal
        assert(healAmt == 525 and message == xi.msg.basic.SELF_HEAL)
    end)
end)
