require('scripts/actions/mobskills/summonshadows')
describe('Summonshadows mob skill', function()
    it('always fails skill check and sets NONE when weapon skill runs', function()
        local skill = require('scripts/actions/mobskills/summonshadows')
        local message = nil
        local sk = { setMsg = function(_, v) message = v end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 1)
        -- stub GetMobByID to nil so spawn branch is skipped
        local oldGet = GetMobByID
        GetMobByID = function() return nil end
        local mob = { getID = function() return 100 end, getXPos = function() return 1 end, getYPos = function() return 2 end, getZPos = function() return 3 end }
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 0 and message == xi.msg.basic.NONE)
        GetMobByID = oldGet
    end)
end)
