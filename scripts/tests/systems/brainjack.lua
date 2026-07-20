require('scripts/actions/mobskills/brainjack')
describe('Brainjack mob skill', function()
    it('charms on SKILL_ENFEEB_IS and adds REGEN_DOWN', function()
        local skill = require('scripts/actions/mobskills/brainjack')
        local statusMove = xi.mobskills.mobStatusEffectMove
        local status, msg, charmed, enmityReset, regen = nil, nil, false, false, nil
        local effect = {
            addMod = function(_, mod, power) regen = { mod, power } end,
        }
        local target = {
            getStatusEffect = function(_, e)
                if e == xi.effect.CHARM_I then return effect end
                return nil
            end,
        }
        local mob = {
            charm = function(_, t) charmed = (t == target) end,
            resetEnmity = function(_, t) enmityReset = (t == target) end,
        }
        local skillObj = {
            setMsg = function(_, m) msg = m end,
            getMsg = function() return msg end,
        }
        xi.mobskills.mobStatusEffectMove = function(_, _, effectId, power, tick, duration)
            status = { effectId, power, tick, duration }
            return xi.msg.basic.SKILL_ENFEEB_IS
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, skillObj, {}) == xi.effect.CHARM_I)
        assert(status[1] == xi.effect.CHARM_I and status[3] == 3 and status[4] == 60)
        assert(charmed and enmityReset)
        assert(regen[1] == xi.mod.REGEN_DOWN and regen[2] == 25)
        charmed, enmityReset, regen = false, false, nil
        xi.mobskills.mobStatusEffectMove = function() return xi.msg.basic.SKILL_MISS end
        assert(skill.onMobWeaponSkill(mob, target, skillObj, {}) == xi.effect.CHARM_I)
        assert(not charmed and not enmityReset and regen == nil)
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
