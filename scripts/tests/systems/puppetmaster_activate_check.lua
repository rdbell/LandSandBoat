require('scripts/actions/abilities/activate')

describe('Puppetmaster Activate availability check', function()
    it('applies existing-pet, area, and elemental-capacity gates in order', function()
        local skill = require('scripts/actions/abilities/activate')
        local pet = {}
        local player = {
            getPet = function() return pet end,
            canUseMisc = function() return false end,
            isExceedingElementalCapacity = function() return true end,
        }

        local msg, param = skill.onAbilityCheck(player, {}, {})
        assert(msg == xi.msg.basic.ALREADY_HAS_A_PET and param == 0)

        player.getPet = function() return nil end
        msg, param = skill.onAbilityCheck(player, {}, {})
        assert(msg == xi.msg.basic.CANT_BE_USED_IN_AREA and param == 0)

        player.canUseMisc = function() return true end
        msg, param = skill.onAbilityCheck(player, {}, {})
        assert(msg == xi.msg.basic.AUTO_EXCEEDS_CAPACITY and param == 0)

        player.isExceedingElementalCapacity = function() return false end
        msg, param = skill.onAbilityCheck(player, {}, {})
        assert(msg == 0 and param == 0)
    end)
end)
