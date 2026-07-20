require('scripts/actions/mobskills/attractant')
describe('Attractant mob skill', function()
    it('misses non-PC and charms PC on ENFEEB_IS', function()
        local skill = require('scripts/actions/mobskills/attractant')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local message, charmed = nil, false
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { charm = function(_, t) charmed = t end }
        local npc = { isPC = function() return false end }
        assert(skill.onMobSkillCheck(npc, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, npc, sk, {}) == xi.effect.CHARM_I)
        assert(message == xi.msg.basic.SKILL_MISS and not charmed)
        local pc = { isPC = function() return true end }
        xi.mobskills.mobStatusEffectMove = function() return xi.msg.basic.SKILL_ENFEEB_IS end
        assert(skill.onMobWeaponSkill(mob, pc, sk, {}) == xi.effect.CHARM_I)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(charmed == pc and message == xi.msg.basic.SKILL_ENFEEB_IS)
    end)
end)
