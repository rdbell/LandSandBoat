require('scripts/actions/mobskills/rejuvenation')
describe('Rejuvenation mob skill', function()
    it('fully restores HP, MP, and TP and emits SELF_HEAL', function()
        local rejuv = require('scripts/actions/mobskills/rejuvenation')
        local hpAdd, mpAdd, tpAdd, message = nil, nil, nil, nil
        local target = {
            getMaxHP = function() return 1000 end,
            getHP = function() return 900 end,
            getMaxMP = function() return 200 end,
            getMP = function() return 150 end,
            getTP = function() return 500 end,
            addHP = function(_, v) hpAdd = v end,
            addMP = function(_, v) mpAdd = v end,
            addTP = function(_, v) tpAdd = v end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        assert(rejuv.onMobSkillCheck(target, {}, skill) == 0)
        assert(rejuv.onMobWeaponSkill({}, target, skill, {}) == 100)
        assert(hpAdd == 100 and mpAdd == 50 and tpAdd == 2500 and message == xi.msg.basic.SELF_HEAL)
    end)
end)
