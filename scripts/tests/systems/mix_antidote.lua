require('scripts/actions/mobskills/mix_antidote')
describe('Mix Antidote mob skill', function()
    it('erases Poison or reports no effect', function()
        local skill = require('scripts/actions/mobskills/mix_antidote')
        local msg, deleted, has = nil, false, true
        local target = {
            hasStatusEffect = function(_, e) return has and e == xi.effect.POISON end,
            delStatusEffect = function(_, e)
                if e == xi.effect.POISON then deleted = true end
            end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.POISON)
        assert(msg == xi.msg.basic.SKILL_ERASE and deleted)
        has, deleted, msg = false, false, nil
        skill.onMobWeaponSkill({}, target, sk, {})
        assert(msg == xi.msg.basic.SKILL_NO_EFFECT and not deleted)
    end)
end)
