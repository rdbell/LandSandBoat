require('scripts/globals/job_utils/dragoon')

describe('Dragoon Call Wyvern availability check', function()
    it('rejects an existing pet, Spirit Surge, and pet-restricted areas in order', function()
        local pet = {}
        local player = {
            getPet = function() return pet end,
            hasStatusEffect = function() return true end,
            canUseMisc = function() return false end,
        }

        assert(xi.job_utils.dragoon.abilityCheckCallWyvern(player, {}, {}) == xi.msg.basic.ALREADY_HAS_A_PET)

        player.getPet = function() return nil end
        assert(xi.job_utils.dragoon.abilityCheckCallWyvern(player, {}, {}) == xi.msg.basic.UNABLE_TO_USE_JA)

        player.hasStatusEffect = function() return false end
        assert(xi.job_utils.dragoon.abilityCheckCallWyvern(player, {}, {}) == xi.msg.basic.CANT_BE_USED_IN_AREA)

        player.canUseMisc = function() return true end
        assert(xi.job_utils.dragoon.abilityCheckCallWyvern(player, {}, {}) == 0)
    end)
end)
