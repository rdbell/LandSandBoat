require('scripts/globals/job_utils/dragoon')

describe('Dragoon Spirit Link availability check', function()
    it('requires a wyvern and permits a full wyvern only with Empathy', function()
        local hp, maxHP, empathy = 100, 100, 0
        local wyvern = {
            getPetID = function() return xi.petId.WYVERN end,
            getHP = function() return hp end,
            getMaxHP = function() return maxHP end,
        }
        local player = {
            getPet = function() return nil end,
            getMerit = function() return empathy end,
        }

        assert(xi.job_utils.dragoon.abilityCheckSpiritLink(player, {}, {}) == xi.msg.basic.REQUIRES_A_PET)

        player.getPet = function() return wyvern end
        assert(xi.job_utils.dragoon.abilityCheckSpiritLink(player, {}, {}) == xi.msg.basic.UNABLE_TO_USE_JA)

        empathy = 1
        assert(xi.job_utils.dragoon.abilityCheckSpiritLink(player, {}, {}) == 0)

        empathy = 0
        hp = 99
        assert(xi.job_utils.dragoon.abilityCheckSpiritLink(player, {}, {}) == 0)
    end)
end)
