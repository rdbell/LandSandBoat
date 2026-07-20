describe('Inspirit mob skill', function()
    it('sets SELF_HEAL and heals floor(current HP / 7) times two', function()
        local inspirit = require('scripts/actions/mobskills/inspirit')
        local heal = xi.mobskills.mobHealMove
        local message, args
        local mob = { getHP = function() return 50 end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobHealMove = function(_, amount) args = { amount }; return 777 end
        assert(inspirit.onMobSkillCheck({}, mob, skill) == 0 and inspirit.onMobWeaponSkill(mob, {}, skill, {}) == 777)
        xi.mobskills.mobHealMove = heal
        assert(message == xi.msg.basic.SELF_HEAL and args[1] == 14)
    end)
end)
