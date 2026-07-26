require('scripts/globals/job_utils/dragoon')

describe('Dragoon Deep Breathing availability check', function()
    it('requires a pet and rejects non-wyvern pets', function()
        local petID
        local player = {
            getPet = function()
                if not petID then
                    return nil
                end
                return { getPetID = function() return petID end }
            end,
        }

        assert(xi.job_utils.dragoon.abilityCheckDeepBreathing(player, {}, {}) == xi.msg.basic.REQUIRES_A_PET)

        petID = xi.petId.LUOPAN
        assert(xi.job_utils.dragoon.abilityCheckDeepBreathing(player, {}, {}) == xi.msg.basic.NO_EFFECT_ON_PET)

        petID = xi.petId.WYVERN
        assert(xi.job_utils.dragoon.abilityCheckDeepBreathing(player, {}, {}) == 0)
    end)
end)
