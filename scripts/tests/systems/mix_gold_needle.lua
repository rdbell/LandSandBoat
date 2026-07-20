require('scripts/actions/mobskills/mix_gold_needle')
describe('Mix Gold Needle mob skill', function()
    it('erases Petrification or reports no effect', function()
        local skill = require('scripts/actions/mobskills/mix_gold_needle')
        local msg, deleted, has = nil, false, true
        local target = {
            hasStatusEffect = function(_, e) return has and e == xi.effect.PETRIFICATION end,
            delStatusEffect = function(_, e)
                if e == xi.effect.PETRIFICATION then deleted = true end
            end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.PETRIFICATION)
        assert(msg == xi.msg.basic.SKILL_ERASE and deleted)
        has, deleted, msg = false, false, nil
        skill.onMobWeaponSkill({}, target, sk, {})
        assert(msg == xi.msg.basic.NO_EFFECT and not deleted)
    end)
end)
