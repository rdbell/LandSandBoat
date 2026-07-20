describe('Fragrant Breath mob skill', function()
    it('rejects admission, misses non-players, and charms PCs only after enfeebling success', function()
        local breath = require('scripts/actions/mobskills/fragrant_breath')
        local effectMove = xi.mobskills.mobStatusEffectMove
        local skill = { getID = function() return 0 end }
        skill.setMsg = function(_, value) skill.message = value end
        local mob = { charm = function(_, target) target.charmed = true end }
        local npc = { isPC = function() return false end }
        local player = { isPC = function() return true end }
        assert(breath.onMobSkillCheck({}, mob, skill) == 1 and breath.onMobWeaponSkill(mob, npc, skill, {}) == xi.effect.CHARM_I and skill.message == xi.msg.basic.SKILL_MISS)
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            assert(effect == xi.effect.CHARM_I and power == 0 and tick == 3 and duration == 30)
            return xi.msg.basic.SKILL_MISS
        end
        assert(breath.onMobWeaponSkill(mob, player, skill, {}) == xi.effect.CHARM_I and not player.charmed)
        skill.getID = function() return 1606 end
        xi.mobskills.mobStatusEffectMove = function(_, _, _, _, _, duration) assert(duration == 15); return xi.msg.basic.SKILL_ENFEEB_IS end
        assert(breath.onMobWeaponSkill(mob, player, skill, {}) == xi.effect.CHARM_I and player.charmed and skill.message == xi.msg.basic.SKILL_ENFEEB_IS)
        xi.mobskills.mobStatusEffectMove = effectMove
    end)
end)
