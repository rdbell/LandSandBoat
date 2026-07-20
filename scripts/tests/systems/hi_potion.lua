require('scripts/actions/mobskills/hi_potion')
describe('Hi Potion mob skill', function()
    it('heals 100 HP with SELF_HEAL message', function()
        local skill = require('scripts/actions/mobskills/hi_potion')
        local healMove = xi.mobskills.mobHealMove
        local msg, amount = nil, nil
        local sk = { setMsg = function(_, m) msg = m end }
        xi.mobskills.mobHealMove = function(_, v) amount = v; return v end
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 100)
        assert(msg == xi.msg.basic.SELF_HEAL and amount == 100)
        xi.mobskills.mobHealMove = healMove
    end)
end)
