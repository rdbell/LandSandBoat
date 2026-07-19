require('scripts/actions/mobskills/wz_recover_all')

describe('WZ Recover All mob skill', function()
    it('restores exactly missing HP and MP, resets recasts, and reports recovery', function()
        local recoverAll = require('scripts/actions/mobskills/wz_recover_all')
        local hp, mp, reset, message = nil, nil, false, nil
        local target = {
            getMaxHP = function() return 1000 end,
            getHP = function() return 625 end,
            getMaxMP = function() return 500 end,
            getMP = function() return 125 end,
            addHP = function(_, value) hp = value end,
            addMP = function(_, value) mp = value end,
            resetRecasts = function() reset = true end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        assert(recoverAll.onMobSkillCheck(target, {}, skill) == 0)
        assert(recoverAll.onMobWeaponSkill({}, target, skill, {}) == 0)
        assert(hp == 375 and mp == 375 and reset)
        assert(message == xi.msg.basic.RECOVERS_HP_AND_MP)
    end)
end)
