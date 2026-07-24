require('scripts/globals/mobskills')

describe('Mob skill TP return gate', function()
    it('requires both a landed hit and damage', function()
        assert(xi.mobskills.skillTPReturnApplies(100, 1))
        assert(not xi.mobskills.skillTPReturnApplies(0, 1))
        assert(not xi.mobskills.skillTPReturnApplies(100, 0))
        assert(not xi.mobskills.skillTPReturnApplies(0, 0))
    end)
end)

describe('Mob skill TP delay selection', function()
    it('uses the ranged delay only for ranged skills', function()
        assert(xi.mobskills.skillTPUsesRangedDelay(xi.attackType.RANGED))
    end)

    -- Breath and magical skills take the melee hit TP return and melee delay,
    -- same as physical.
    it('uses the melee delay for physical, breath, and magical skills', function()
        assert(not xi.mobskills.skillTPUsesRangedDelay(xi.attackType.PHYSICAL))
        assert(not xi.mobskills.skillTPUsesRangedDelay(xi.attackType.BREATH))
        assert(not xi.mobskills.skillTPUsesRangedDelay(xi.attackType.MAGICAL))
    end)
end)

describe('Mob skill extra hit TP', function()
    it('gives ten TP per hit past the first', function()
        assert(xi.mobskills.skillTPExtraHitBonus(1) == 0)
        assert(xi.mobskills.skillTPExtraHitBonus(2) == 10)
        assert(xi.mobskills.skillTPExtraHitBonus(5) == 40)
    end)
end)

describe('Mob skill Save TP floor', function()
    it('raises TP to the modifier floor', function()
        assert(xi.mobskills.saveTPFloor(100, 1000) == 1000)
    end)

    it('never lowers TP already above the floor', function()
        assert(xi.mobskills.saveTPFloor(2000, 1000) == 2000)
        assert(xi.mobskills.saveTPFloor(1000, 1000) == 1000)
    end)

    it('does nothing without the modifier', function()
        assert(xi.mobskills.saveTPFloor(100, 0) == 100)
    end)
end)

describe('Mob skill unequip selection', function()
    it('draws without replacement', function()
        local chosen = xi.mobskills.unequipSelection({ 1, 2, 3 }, 3, function() return 1 end)
        table.sort(chosen)

        assert(#chosen == 3)
        assert(chosen[1] == 1 and chosen[2] == 2 and chosen[3] == 3)
    end)

    it('takes at most the requested count', function()
        assert(#xi.mobskills.unequipSelection({ 1, 2, 3 }, 2, function() return 1 end) == 2)
    end)

    it('is bounded by what is actually equipped', function()
        assert(#xi.mobskills.unequipSelection({ 1 }, 5, function() return 1 end) == 1)
        assert(#xi.mobskills.unequipSelection({}, 5, function() return 1 end) == 0)
    end)

    it('removes the rolled index', function()
        local chosen = xi.mobskills.unequipSelection({ 10, 20, 30 }, 1, function() return 2 end)
        assert(#chosen == 1 and chosen[1] == 20)
    end)

    it('leaves the caller list untouched', function()
        local slots = { 1, 2, 3 }
        xi.mobskills.unequipSelection(slots, 3, function() return 1 end)
        assert(#slots == 3)
    end)
end)

describe('Pet magic accuracy bonus', function()
    it('gives an avatar its master skill overcap', function()
        assert(xi.mobskills.petMagicAccuracyBonus(true, 300, 250, false, 0) == 50)
    end)

    it('floors the overcap at zero and caps it at two hundred', function()
        assert(xi.mobskills.petMagicAccuracyBonus(true, 200, 250, false, 0) == 0)
        assert(xi.mobskills.petMagicAccuracyBonus(true, 1000, 250, false, 0) == 200)
    end)

    it('ignores the overcap for a non-avatar pet', function()
        assert(xi.mobskills.petMagicAccuracyBonus(false, 1000, 250, false, 0) == 0)
    end)

    it('adds a flat bonus while a burstable skillchain is up', function()
        assert(xi.mobskills.petMagicAccuracyBonus(false, 0, 0, true, 1) == 25)
        assert(xi.mobskills.petMagicAccuracyBonus(true, 300, 250, true, 1) == 75)
    end)

    it('needs both a pet ID and a skillchain for that bonus', function()
        assert(xi.mobskills.petMagicAccuracyBonus(false, 0, 0, true, 0) == 0)
        assert(xi.mobskills.petMagicAccuracyBonus(false, 0, 0, false, 1) == 0)
    end)
end)
