require('scripts/actions/mobskills/wisecrack')
describe('Wisecrack mob skill', function()
    it('misses non-PC and charms PC on ENFEEB_IS', function()
        local skill = require('scripts/actions/mobskills/wisecrack')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local message, charmed, statusArgs = nil, false, nil
        local sk = { setMsg = function(_, v) message = v end }
        local mob = { charm = function(_, target) charmed = target end }
        local npc = { isPC = function() return false end }
        assert(skill.onMobSkillCheck(npc, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, npc, sk, {}) == xi.effect.CHARM_I)
        assert(message == xi.msg.basic.SKILL_MISS and not charmed)
        local pc = { isPC = function() return true end }
        xi.mobskills.mobStatusEffectMove = function(m, t, effect, power, tick, duration)
            statusArgs = { m, t, effect, power, tick, duration }
            return xi.msg.basic.SKILL_ENFEEB_IS
        end
        assert(skill.onMobWeaponSkill(mob, pc, sk, {}) == xi.effect.CHARM_I)
        xi.mobskills.mobStatusEffectMove = statusMove
        assert(statusArgs[3] == xi.effect.CHARM_I and statusArgs[4] == 0 and statusArgs[5] == 3 and statusArgs[6] == 30)
        assert(charmed == pc and message == xi.msg.basic.SKILL_ENFEEB_IS)
    end)
end)
