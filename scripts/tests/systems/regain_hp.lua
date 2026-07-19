require('scripts/actions/mobskills/regain_hp')

describe('Regain HP mob skill', function()
    it('recovers exactly missing HP, wakes the target, and returns the amount', function()
        local regainHP = require('scripts/actions/mobskills/regain_hp')
        local hp, woke, message = nil, false, nil
        local target = {
            getMaxHP = function() return 1000 end,
            getHP = function() return 725 end,
            addHP = function(_, value) hp = value end,
            wakeUp = function() woke = true end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(regainHP.onMobSkillCheck(target, {}, skill) == 0)
        assert(regainHP.onMobWeaponSkill({}, target, skill, {}) == 275)
        assert(hp == 275 and woke and message == xi.msg.basic.AOE_REGAIN_HP)
    end)
end)
