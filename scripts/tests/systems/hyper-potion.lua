require('scripts/actions/mobskills/hyper-potion')
describe('Hyper Potion mob skill', function()
    it('heals 250 HP with SELF_HEAL', function()
        local skill = require('scripts/actions/mobskills/hyper-potion')
        local healMove = xi.mobskills.mobHealMove
        local msg, amount = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobHealMove = function(_, v) amount = v; return v end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 250)
        assert(msg == xi.msg.basic.SELF_HEAL and amount == 250)
        xi.mobskills.mobHealMove = healMove
    end)
end)
