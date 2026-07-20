require('scripts/actions/mobskills/mix_dry_ether_concoction')
describe('Mix Dry Ether Concoction mob skill', function()
    it('restores 160 MP with SKILL_RECOVERS_MP', function()
        local skill = require('scripts/actions/mobskills/mix_dry_ether_concoction')
        local msg, mp = nil, nil
        local target = { addMP = function(_, v) mp = v end }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == 0)
        assert(msg == xi.msg.basic.SKILL_RECOVERS_MP and mp == 160)
    end)
end)
