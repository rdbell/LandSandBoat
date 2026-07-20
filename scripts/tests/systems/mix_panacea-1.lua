require('scripts/actions/mobskills/mix_panacea-1')
describe('Mix Panacea-1 mob skill', function()
    it('erases first matching panacea effect in order', function()
        local skill = require('scripts/actions/mobskills/mix_panacea-1')
        local msg, deleted = nil, nil
        local effects = { [xi.effect.ATTACK_DOWN] = true, [xi.effect.BIND] = true }
        local target = {
            delStatusEffect = function(_, e)
                if effects[e] then
                    effects[e] = nil
                    deleted = e
                    return true
                end
                return false
            end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.BIND)
        assert(msg == xi.msg.basic.SKILL_ERASE and deleted == xi.effect.BIND)
        effects, deleted, msg = {}, nil, nil
        skill.onMobWeaponSkill({}, target, sk, {})
        assert(msg == xi.msg.basic.NO_EFFECT)
    end)
end)
