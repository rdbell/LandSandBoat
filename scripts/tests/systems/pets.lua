require('scripts/globals/pets')

describe('Pet mob-skill admission', function()
    it('blocks summoning when no pet is assigned or a pet is already spawned', function()
        assert(xi.pet.mobSkillCheckResult(false, false) == 1)
        assert(xi.pet.mobSkillCheckResult(true, true) == 1)
    end)

    it('allows summoning for an assigned but unspawned pet', function()
        assert(xi.pet.mobSkillCheckResult(true, false) == 0)
    end)
end)
