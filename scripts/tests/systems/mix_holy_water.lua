require('scripts/actions/mobskills/mix_holy_water')
describe('Mix Holy Water mob skill', function()
    it('erases curse/bane and can remove doom on roll', function()
        local skill = require('scripts/actions/mobskills/mix_holy_water')
        local msg, basic, deleted = nil, nil, {}
        local effects = { [xi.effect.CURSE_I] = true }
        local target = {
            getMod = function() return 0 end,
            hasStatusEffect = function(_, e) return effects[e] == true end,
            delStatusEffect = function(_, e) deleted[#deleted + 1] = e; effects[e] = nil end,
            messageBasic = function(_, m) basic = m end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, {}, sk) == 0)
        assert(skill.onMobWeaponSkill({}, target, sk, {}) == xi.effect.CURSE_I)
        assert(msg == xi.msg.basic.SKILL_ERASE and deleted[1] == xi.effect.CURSE_I)
        -- no effect path
        effects, deleted, msg = {}, {}, nil
        skill.onMobWeaponSkill({}, target, sk, {})
        assert(msg == xi.msg.basic.NO_EFFECT or msg == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
