describe('Frog Song mob skill', function()
    it('misses non-players and charms then costumes players only after enfeebling success', function()
        local song = require('scripts/actions/mobskills/frog_song')
        local effectMove = xi.mobskills.mobStatusEffectMove
        local skill = {}; skill.setMsg = function(_, value) skill.message = value end
        local mob = { charm = function(_, target) target.charmed = true end }
        local npc = { isPC = function() return false end }
        local player = { isPC = function() return true end }
        player.addStatusEffect = function(_, effect, options) player.costume = { effect, options } end
        assert(song.onMobSkillCheck({}, mob, skill) == 0 and song.onMobWeaponSkill(mob, npc, skill, {}) == xi.effect.CHARM_I and skill.message == xi.msg.basic.SKILL_MISS)
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration) assert(effect == xi.effect.CHARM_I and power == 0 and tick == 3 and duration == 60); return xi.msg.basic.SKILL_MISS end
        assert(song.onMobWeaponSkill(mob, player, skill, {}) == xi.effect.CHARM_I and not player.charmed and player.costume == nil)
        xi.mobskills.mobStatusEffectMove = function() return xi.msg.basic.SKILL_ENFEEB_IS end
        assert(song.onMobWeaponSkill(mob, player, skill, {}) == xi.effect.CHARM_I and player.charmed and player.costume[1] == xi.effect.COSTUME and player.costume[2].power == 1812 and player.costume[2].duration == 60 and player.costume[2].origin == mob and skill.message == xi.msg.basic.SKILL_ENFEEB_IS)
        xi.mobskills.mobStatusEffectMove = effectMove
    end)
end)
