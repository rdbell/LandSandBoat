require('scripts/actions/mobskills/diamondhide')

describe('Diamondhide mob skill', function()
    it('applies Stoneskin, forwards its message, and makes a present effect non-dispellable', function()
        local diamondhide = require('scripts/actions/mobskills/diamondhide')
        local originalBuffMove = xi.mobskills.mobBuffMove
        local buff, message, deleted = nil, nil, nil
        local effect = { delEffectFlag = function(_, flag) deleted = flag end }
        local target = { getStatusEffect = function(_, id) assert(id == xi.effect.STONESKIN); return effect end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            buff = { ... }
            return 123
        end

        assert(diamondhide.onMobSkillCheck(target, {}, skill) == 0)
        assert(diamondhide.onMobWeaponSkill({}, target, skill, {}) == xi.effect.STONESKIN)
        xi.mobskills.mobBuffMove = originalBuffMove

        assert(buff[1] == target and buff[2] == xi.effect.STONESKIN)
        assert(buff[3] == 800 and buff[4] == 0 and buff[5] == 300)
        assert(message == 123 and deleted == xi.effectFlag.DISPELABLE)
    end)

    it('does not require Stoneskin lookup to return an effect', function()
        local diamondhide = require('scripts/actions/mobskills/diamondhide')
        local originalBuffMove = xi.mobskills.mobBuffMove
        xi.mobskills.mobBuffMove = function() return 123 end
        assert(diamondhide.onMobWeaponSkill({}, { getStatusEffect = function() return nil end }, { setMsg = function() end }, {}) == xi.effect.STONESKIN)
        xi.mobskills.mobBuffMove = originalBuffMove
    end)
end)
