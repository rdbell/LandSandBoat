require('scripts/actions/mobskills/qn_aern_rdm')
describe("Qn'aern RDM mob skill", function()
    it('admits QNAERN_RDM at HPP<=70 and applies CHAINSPELL', function()
        local skill = require('scripts/actions/mobskills/qn_aern_rdm')
        local msg, buff = nil, nil
        local orig = xi.mobskills.mobBuffMove
        xi.mobskills.mobBuffMove = function(mob, effect, power, tick, duration)
            buff = { effect, power, tick, duration }
            return 0
        end
        local mob = { getPool = function() return xi.mobPool.QNAERN_RDM end, getHPP = function() return 70 end }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        mob.getHPP = function() return 71 end
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        mob.getHPP = function() return 50 end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.CHAINSPELL)
        assert(buff[1] == xi.effect.CHAINSPELL and buff[2] == 1 and buff[4] == 60 and msg == xi.msg.basic.USES)
        xi.mobskills.mobBuffMove = orig
    end)
end)
