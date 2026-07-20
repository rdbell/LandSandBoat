require('scripts/actions/mobskills/contagion_transfer')
describe('Contagion Transfer mob skill', function()
    it('transfers flag-matching effects and reports drained count', function()
        local skill = require('scripts/actions/mobskills/contagion_transfer')
        local msg, transferred = nil, {}
        local effects = {
            { getEffectType = function() return xi.effect.POISON end, getEffectFlags = function() return xi.effectFlag.DISPELABLE end },
            { getEffectType = function() return 999 end, getEffectFlags = function() return 0 end },
        }
        local target = {
            getStatusEffects = function() return effects end,
            hasStatusEffect = function(_, e) return e == xi.effect.PETRIFICATION end,
            getStatusEffect = function(_, id)
                if id == xi.effect.POISON or id == xi.effect.PETRIFICATION then
                    return { id = id }
                end
                return nil
            end,
            delStatusEffect = function(_, id) transferred[#transferred+1] = { 'del', id } end,
        }
        local mob = {
            copyStatusEffect = function(_, effect) transferred[#transferred+1] = { 'copy', effect.id } end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 2)
        assert(msg == xi.msg.basic.EFFECT_DRAINED)
        assert(#transferred == 4)
        -- no effects
        target.getStatusEffects = function() return {} end
        target.hasStatusEffect = function() return false end
        msg = nil
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 0)
        assert(msg == xi.msg.basic.SKILL_NO_EFFECT)
    end)
end)
