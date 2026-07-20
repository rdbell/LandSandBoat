require('scripts/actions/mobskills/regain_mp')
describe('Regain MP mob skill', function()
    it('restores missing MP with AOE_REGAIN_MP', function()
        local skill = require('scripts/actions/mobskills/regain_mp')
        local msg, added = nil, 0
        local target = {
            getMaxMP = function() return 500 end,
            getMP = function() return 200 end,
            addMP = function(_, v) added = v end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == 300)
        assert(added == 300 and msg == xi.msg.basic.AOE_REGAIN_MP)
    end)
end)
