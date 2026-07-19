require('scripts/actions/mobskills/dagan')

describe('Dagan mob skill', function()
    it('recovers floored TP-scaled HP and MP and returns recovered HP', function()
        local factor = xi.combat.physical.calculateTPfactor
        local message = nil
        local hp, mp = nil, nil
        local mob = {
            getMaxHP = function() return 1000 end,
            getMaxMP = function() return 200 end,
            addHP = function(_, value) hp = value end,
            addMP = function(_, value) mp = value end,
        }
        local skill = {
            getTP = function() return 1500 end,
            setMsg = function(_, value) message = value end,
        }
        xi.combat.physical.calculateTPfactor = function(tp, values)
            assert(tp == 1500)
            if values[1] == 22 then return 27.5 end
            return 18.5
        end

        local dagan = require('scripts/actions/mobskills/dagan')
        assert(dagan.onMobSkillCheck({}, mob, skill) == 0)
        assert(dagan.onMobWeaponSkill(mob, {}, skill, {}) == 275)
        xi.combat.physical.calculateTPfactor = factor

        assert(hp == 275 and mp == 37 and message == xi.msg.basic.SKILL_RECOVERS_HP)
    end)
end)
