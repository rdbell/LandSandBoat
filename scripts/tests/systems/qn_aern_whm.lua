require('scripts/actions/mobskills/qn_aern_whm')
describe("Qn'aern WHM mob skill", function()
    it('admits QNAERN_WHM at HPP<=50 and full heals after erase', function()
        local skill = require('scripts/actions/mobskills/qn_aern_whm')
        local msg, erased, added, woke = nil, false, 0, false
        local mob = { getPool = function() return xi.mobPool.QNAERN_WHM end, getHPP = function() return 50 end }
        local target = {
            getMaxHP = function() return 1000 end,
            getHP = function() return 400 end,
            eraseAllStatusEffect = function() erased = true end,
            addHP = function(_, v) added = v end,
            wakeUp = function() woke = true end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        mob.getHPP = function() return 51 end
        assert(skill.onMobSkillCheck(target, mob, sk) == 1)
        mob.getHPP = function() return 40 end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 600)
        assert(erased and added == 600 and woke and msg == xi.msg.basic.SELF_HEAL)
    end)
end)
