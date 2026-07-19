require('scripts/actions/mobskills/airy_shield')

describe('Airy Shield mob skill', function()
    it('makes Arrow Shield non-dispellable and removes Magic Shield after buffing', function()
        local airyShield = require('scripts/actions/mobskills/airy_shield')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message, deleted, removed = nil, nil, nil, nil
        local effect = { delEffectFlag = function(_, flag) deleted = flag end }
        local mob = {
            getStatusEffect = function(_, id) assert(id == xi.effect.ARROW_SHIELD); return effect end,
            delStatusEffect = function(_, id) removed = id end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(target, effectID, power, tick, duration)
            buff = { target, effectID, power, tick, duration }
            return 123
        end

        assert(airyShield.onMobSkillCheck(nil, mob, skill) == 0)
        assert(airyShield.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.ARROW_SHIELD)
        xi.mobskills.mobBuffMove = originalBuffMove

        assert(buff[1] == mob and buff[2] == xi.effect.ARROW_SHIELD)
        assert(buff[3] == 1 and buff[4] == 0 and buff[5] == 60)
        assert(message == 123 and deleted == xi.effectFlag.DISPELABLE and removed == xi.effect.MAGIC_SHIELD)
    end)

    it('still removes Magic Shield when Arrow Shield cannot be found', function()
        local airyShield = require('scripts/actions/mobskills/airy_shield')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local removed = nil
        local mob = { getStatusEffect = function() return nil end, delStatusEffect = function(_, id) removed = id end }
        xi.mobskills.mobBuffMove = function() return 123 end
        assert(airyShield.onMobWeaponSkill(mob, nil, { setMsg = function() end }, nil) == xi.effect.ARROW_SHIELD)
        xi.mobskills.mobBuffMove = originalBuffMove
        assert(removed == xi.effect.MAGIC_SHIELD)
    end)
end)
