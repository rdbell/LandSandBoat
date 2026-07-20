require('scripts/actions/mobskills/magic_barrier')

describe('Magic Barrier mob skill', function()
    it('makes Magic Shield non-dispellable and removes Arrow Shield after buffing', function()
        local magicBarrier = require('scripts/actions/mobskills/magic_barrier')
        local buffMove = xi.mobskills.mobBuffMove
        local buff, message, deleted, removed = nil, nil, nil, nil
        local effect = { delEffectFlag = function(_, flag) deleted = flag end }
        local mob = {
            getStatusEffect = function(_, id) assert(id == xi.effect.MAGIC_SHIELD); return effect end,
            delStatusEffect = function(_, id) removed = id end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(target, effectID, power, tick, duration)
            buff = { target, effectID, power, tick, duration }
            return 123
        end

        assert(magicBarrier.onMobSkillCheck(nil, mob, skill) == 0)
        assert(magicBarrier.onMobWeaponSkill(mob, nil, skill, nil) == xi.effect.MAGIC_SHIELD)
        xi.mobskills.mobBuffMove = buffMove

        assert(buff[1] == mob and buff[2] == xi.effect.MAGIC_SHIELD and buff[3] == 1 and buff[4] == 0 and buff[5] == 60)
        assert(message == 123 and deleted == xi.effectFlag.DISPELABLE and removed == xi.effect.ARROW_SHIELD)
    end)

    it('still removes Arrow Shield when Magic Shield cannot be found', function()
        local magicBarrier = require('scripts/actions/mobskills/magic_barrier')
        local buffMove = xi.mobskills.mobBuffMove
        local removed = nil
        local mob = { getStatusEffect = function() return nil end, delStatusEffect = function(_, id) removed = id end }
        xi.mobskills.mobBuffMove = function() return 123 end
        assert(magicBarrier.onMobWeaponSkill(mob, nil, { setMsg = function() end }, nil) == xi.effect.MAGIC_SHIELD)
        xi.mobskills.mobBuffMove = buffMove
        assert(removed == xi.effect.ARROW_SHIELD)
    end)
end)
