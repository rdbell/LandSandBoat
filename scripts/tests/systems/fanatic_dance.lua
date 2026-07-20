require('scripts/actions/mobskills/fanatic_dance')

describe('Fanatic Dance mob skill', function()
    it('requires a Dynamis notorious mob, misses non-PCs, and charms only after enfeebling success', function()
        local dance = require('scripts/actions/mobskills/fanatic_dance')
        local effectMove = xi.mobskills.mobStatusEffectMove
        local mob = {
            isInDynamis = function() return true end,
            isMobType = function() return true end,
            charm = function(_, target) target.charmed = true end,
        }
        local nonNM = { isInDynamis = function() return true end, isMobType = function() return false end }
        local skill = {}
        skill.setMsg = function(_, value) skill.message = value end
        local npc = { isPC = function() return false end }
        local player = { isPC = function() return true end }
        assert(dance.onMobSkillCheck({}, mob, skill) == 0 and dance.onMobSkillCheck({}, nonNM, skill) == 1)
        assert(dance.onMobWeaponSkill(mob, npc, skill, {}) == xi.effect.CHARM_I and skill.message == xi.msg.basic.SKILL_MISS)
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            assert(effect == xi.effect.CHARM_I and power == 0 and tick == 3 and duration == 60)
            return xi.msg.basic.SKILL_MISS
        end
        assert(dance.onMobWeaponSkill(mob, player, skill, {}) == xi.effect.CHARM_I and not player.charmed)
        xi.mobskills.mobStatusEffectMove = function() return xi.msg.basic.SKILL_ENFEEB_IS end
        assert(dance.onMobWeaponSkill(mob, player, skill, {}) == xi.effect.CHARM_I and player.charmed and skill.message == xi.msg.basic.SKILL_ENFEEB_IS)
        xi.mobskills.mobStatusEffectMove = effectMove
    end)
end)
