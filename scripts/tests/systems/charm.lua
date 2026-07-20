require('scripts/actions/mobskills/charm')
describe('Charm mob skill', function()
    it('misses non-PC and charms PC on SKILL_ENFEEB_IS', function()
        local skill = require('scripts/actions/mobskills/charm')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local msg, charmed, enmity = nil, false, false
        local target = { isPC = function() return false end }
        local mob = {
            getPool = function() return 0 end,
            charm = function(_, t) charmed = (t == target) end,
            resetEnmity = function(_, t) enmity = (t == target) end,
        }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.CHARM_I)
        assert(msg == xi.msg.basic.SKILL_MISS and not charmed)
        target.isPC = function() return true end
        msg, charmed, enmity = nil, false, false
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            assert(effect == xi.effect.CHARM_I and power == 0 and tick == 3 and duration == 180)
            return xi.msg.basic.SKILL_ENFEEB_IS
        end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == xi.effect.CHARM_I)
        assert(charmed and enmity and msg == xi.msg.basic.SKILL_ENFEEB_IS)
        -- OSSCHAART duration
        mob.getPool = function() return xi.mobPool.OSSCHAART end
        local dur = nil
        xi.mobskills.mobStatusEffectMove = function(_, _, _, _, _, duration) dur = duration; return xi.msg.basic.SKILL_MISS end
        skill.onMobWeaponSkill(mob, target, sk, {})
        assert(dur == 30)
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
