require('scripts/actions/mobskills/danse_macabre')
describe('Danse Macabre mob skill', function()
    it('charms PC on SKILL_ENFEEB_IS and misses non-PC', function()
        local skill = require('scripts/actions/mobskills/danse_macabre')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local msg, charmed = nil, false
        local target = { isPC = function() return false end }
        local mob = { charm = function(_, t) charmed = true end }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.CHARM_I)
        assert(msg == xi.msg.basic.SKILL_MISS and not charmed)
        target.isPC = function() return true end
        xi.mobskills.mobStatusEffectMove = function(_,_,e,p,t,d)
            assert(e == xi.effect.CHARM_I and t == 3 and d == 60)
            return xi.msg.basic.SKILL_ENFEEB_IS
        end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.CHARM_I)
        assert(charmed and msg == xi.msg.basic.SKILL_ENFEEB_IS)
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
