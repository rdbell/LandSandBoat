require('scripts/actions/mobskills/whispering_wind')
describe('Whispering Wind mob skill', function()
    it('heals with HP-ratio multiplier and sets SELF_HEAL', function()
        local skill = require('scripts/actions/mobskills/whispering_wind')
        local heal = xi.mobskills.mobHealMove
        local message, amount = nil, nil
        local sk = { setMsg = function(_, v) message = v end, getTP = function() return 1000 end }
        local mob = {
            getMainLvl = function() return 50 end,
            getHP = function() return 100 end,
            getMaxHP = function() return 100 end,
        }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobHealMove = function(t, base) amount = base; return base end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 150 and message == xi.msg.basic.SELF_HEAL and amount == 150)
        xi.mobskills.mobHealMove = heal
    end)
end)
