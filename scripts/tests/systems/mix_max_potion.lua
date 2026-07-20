require('scripts/actions/mobskills/mix_max_potion')
describe('Mix Max Potion mob skill', function()
    it('heals 700 HP with SELF_HEAL', function()
        local skill = require('scripts/actions/mobskills/mix_max_potion')
        local msg = nil
        local orig = xi.mobskills.mobHealMove
        xi.mobskills.mobHealMove = function(target, amount)
            assert(amount == 700)
            return amount
        end
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, {}, sk, {}) == 700)
        assert(msg == xi.msg.basic.SELF_HEAL)
        xi.mobskills.mobHealMove = orig
    end)
end)
