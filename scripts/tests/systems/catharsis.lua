require('scripts/actions/mobskills/catharsis')
describe('Catharsis mob skill', function()
    it('self-heals 12.5 percent of max HP', function()
        local skill = require('scripts/actions/mobskills/catharsis')
        local healMove = xi.mobskills.mobHealMove
        local msg, amount = nil, nil
        local mob = { getMaxHP = function() return 800 end }
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobHealMove = function(_, v)
            amount = v
            return v
        end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 100)
        assert(msg == xi.msg.basic.SELF_HEAL and amount == 100)
        xi.mobskills.mobHealMove = healMove
    end)
end)
