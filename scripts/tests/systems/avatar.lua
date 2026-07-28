local avatar = require('scripts/mixins/families/avatar')

describe('Avatar mixin', function()
    it('selects models and Astral Flow abilities with normalized delays', function()
        local carby = xi.mix.avatar.spawnPlan(xi.petId.CARBUNCLE, 0, 250)
        assert(carby.modelId == 791 and carby.astralDelayMs == 250 and carby.cleanupDelayMs == 5250)

        local randomRamuh = xi.mix.avatar.spawnPlan(0, xi.petId.RAMUH, -10)
        assert(randomRamuh.modelId == 798 and randomRamuh.astralDelayMs == 0 and randomRamuh.cleanupDelayMs == 5000)

        local diabolos = xi.mix.avatar.engagePlan(1145, 100)
        assert(diabolos.abilityId == 1911 and diabolos.astralDelayMs == 100)
        assert(xi.mix.avatar.spawnPlan(999, xi.petId.CARBUNCLE, 0) == nil)
        assert(xi.mix.avatar.engagePlan(999, 0) == nil)
    end)
end)
