require('scripts/actions/mobskills/mix_vaccine')
describe('Mix Vaccine mob skill', function()
    it('erases Plague or reports no effect', function()
        local skill = require('scripts/actions/mobskills/mix_vaccine')
        local msg, deleted, has = nil, false, true
        local target = {
            hasStatusEffect = function(_, e) return has and e == xi.effect.PLAGUE end,
            delStatusEffect = function(_, e)
                if e == xi.effect.PLAGUE then deleted = true end
            end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.PLAGUE)
        assert(msg == xi.msg.basic.SKILL_ERASE and deleted)
        has, deleted, msg = false, false, nil
        skill.onMobWeaponSkill({}, target, sk, {})
        assert(msg == xi.msg.basic.NO_EFFECT and not deleted)
    end)
end)
