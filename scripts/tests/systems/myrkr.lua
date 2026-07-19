require('scripts/actions/mobskills/myrkr')

describe('Myrkr mob skill', function()
    it('restores 20%, 40%, or 60% of maximum MP at TP boundaries', function()
        local myrkr = require('scripts/actions/mobskills/myrkr')
        local cases = { { 1000, 200 }, { 2000, 400 }, { 3000, 600 } }
        for _, case in ipairs(cases) do
            local restored, message = nil, nil
            local mob = {
                getMaxMP = function() return 1000 end,
                addMP = function(_, value) restored = value end,
            }
            local skill = {
                getTP = function() return case[1] end,
                setMsg = function(_, value) message = value end,
            }
            assert(myrkr.onMobSkillCheck(nil, mob, skill) == 0)
            assert(myrkr.onMobWeaponSkill(mob, nil, skill, nil) == case[2])
            assert(restored == case[2] and message == xi.msg.basic.SKILL_RECOVERS_MP)
        end
    end)

    it('floors fractional MP recovery', function()
        local myrkr = require('scripts/actions/mobskills/myrkr')
        local restored = nil
        local mob = { getMaxMP = function() return 999 end, addMP = function(_, value) restored = value end }
        local skill = { getTP = function() return 1000 end, setMsg = function() end }
        assert(myrkr.onMobWeaponSkill(mob, nil, skill, nil) == 199)
        assert(restored == 199)
    end)
end)
