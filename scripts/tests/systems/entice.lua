require('scripts/actions/mobskills/entice')
describe('Entice mob skill', function()
    it('requires Soul Voice and charms PC', function()
        local skill = require('scripts/actions/mobskills/entice')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local hasSV, msg, charmed = false, nil, false
        local mob = {
            hasStatusEffect = function(_, e) return hasSV and e == xi.effect.SOUL_VOICE end,
            charm = function() charmed = true end,
        }
        local target = { isPC = function() return true end }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 1)
        hasSV = true
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d)
            assert(t == 1 and d == 30)
            return xi.msg.basic.SKILL_ENFEEB_IS
        end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.CHARM_I)
        assert(charmed)
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
