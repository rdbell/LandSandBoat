require('scripts/globals/monstrosity')

describe('Monstrosity instinct addressing', function()
    it('maps the first byte to instincts 0..7', function()
        assert(xi.monstrosity.instinctByteOffset(0) == 20)
        assert(xi.monstrosity.instinctBitShift(0) == 0)

        assert(xi.monstrosity.instinctByteOffset(7) == 20)
        assert(xi.monstrosity.instinctBitShift(7) == 7)
    end)

    it('advances a byte every eight instincts', function()
        assert(xi.monstrosity.instinctByteOffset(8) == 21)
        assert(xi.monstrosity.instinctBitShift(8) == 0)

        assert(xi.monstrosity.instinctByteOffset(31) == 23)
        assert(xi.monstrosity.instinctBitShift(31) == 7)
    end)

    it('accepts only the four instinct bytes', function()
        for offset = 20, 23 do
            assert(xi.monstrosity.instinctByteOffsetValid(offset), offset)
        end

        assert(not xi.monstrosity.instinctByteOffsetValid(19))
        assert(not xi.monstrosity.instinctByteOffsetValid(24))
    end)

    it('covers every purchasable instinct', function()
        for name, id in pairs(xi.monstrosity.purchasableInstincts) do
            assert(xi.monstrosity.instinctByteOffsetValid(xi.monstrosity.instinctByteOffset(id)), name)
        end
    end)
end)

describe('Monstrosity instinct ownership', function()
    local function emptyBytes()
        return { [20] = 0, [21] = 0, [22] = 0, [23] = 0 }
    end

    it('reports an unowned instinct', function()
        assert(xi.monstrosity.instinctPurchased(emptyBytes(), xi.monstrosity.purchasableInstincts.HUME_II) == false)
    end)

    it('round-trips a set instinct', function()
        local bytes = emptyBytes()

        assert(xi.monstrosity.instinctSetPurchased(bytes, xi.monstrosity.purchasableInstincts.RUN))
        assert(xi.monstrosity.instinctPurchased(bytes, xi.monstrosity.purchasableInstincts.RUN))
        assert(bytes[23] == 0x80)
    end)

    it('sets bits without disturbing neighbours', function()
        local bytes = emptyBytes()

        xi.monstrosity.instinctSetPurchased(bytes, 8)
        xi.monstrosity.instinctSetPurchased(bytes, 9)

        assert(bytes[21] == 0x03)
        assert(not xi.monstrosity.instinctPurchased(bytes, 10))
    end)

    it('treats an absent byte as empty', function()
        assert(xi.monstrosity.instinctPurchased({}, 0) == false)
    end)

    it('refuses an out-of-range instinct', function()
        local bytes = emptyBytes()

        assert(xi.monstrosity.instinctPurchased(bytes, 32) == nil)
        assert(not xi.monstrosity.instinctSetPurchased(bytes, 32))
    end)
end)

describe('Monstrosity purchased instinct mask', function()
    it('excludes the default racial instincts below HUME_II', function()
        local mask = xi.monstrosity.purchasedInstinctsMask(function() return true end)

        -- HUME_II..RUN is 27 instincts, rebased to bits 0..26.
        assert(mask == 0x7FFFFFF, mask)
    end)

    it('is empty when nothing is purchased', function()
        assert(xi.monstrosity.purchasedInstinctsMask(function() return false end) == 0)
    end)

    it('rebases HUME_II to bit zero', function()
        local mask = xi.monstrosity.purchasedInstinctsMask(function(id)
            return id == xi.monstrosity.purchasableInstincts.HUME_II
        end)

        assert(mask == 0x1, mask)
    end)

    it('rebases RUN to its offset bit', function()
        local mask = xi.monstrosity.purchasedInstinctsMask(function(id)
            return id == xi.monstrosity.purchasableInstincts.RUN
        end)

        assert(mask == bit.lshift(1, 26), mask)
    end)

    it('ignores a purchased default racial instinct', function()
        local mask = xi.monstrosity.purchasedInstinctsMask(function(id)
            return id <= xi.monstrosity.purchasableInstincts.GALKA_I
        end)

        assert(mask == 0, mask)
    end)
end)

describe('Monstrosity limit break mask', function()
    it('covers WAR through RUN at bit jobId minus one', function()
        local mask = xi.monstrosity.limitBreakMask(function() return true end)

        for jobId = xi.job.WAR, xi.job.RUN do
            assert(utils.mask.getBit(mask, jobId - 1), jobId)
        end
    end)

    it('is empty with no completed limit breaks', function()
        assert(xi.monstrosity.limitBreakMask(function() return false end) == 0)
    end)

    it('places WAR at bit zero and RUN at bit twenty-one', function()
        local war = xi.monstrosity.limitBreakMask(function(jobId) return jobId == xi.job.WAR end)
        local run = xi.monstrosity.limitBreakMask(function(jobId) return jobId == xi.job.RUN end)

        assert(war == 0x1, war)
        assert(run == bit.lshift(1, 21), run)
    end)

    it('ignores jobs outside the WAR..RUN range', function()
        local mask = xi.monstrosity.limitBreakMask(function(jobId) return jobId > xi.job.RUN end)
        assert(mask == 0, mask)
    end)
end)

describe('Terynon MON purchase requirements', function()
    local function speciesLevel(levels)
        return function(species) return levels[species] or 0 end
    end

    it('admits a locked species with satisfied prerequisites', function()
        local offer = { monSpecies = 5, requirements = { { 1, 2 }, { 2, 1 } } }
        assert(xi.monstrosity.purchaseRequirementsMet(offer, speciesLevel({ [1] = 2, [2] = 1 }), function() return false end))
    end)

    it('rejects an unlocked species, locked prerequisite, or unlocked variant', function()
        local species = { monSpecies = 5 }
        assert(not xi.monstrosity.purchaseRequirementsMet(species, speciesLevel({ [5] = 1 }), function() return false end))

        local gated = { monSpecies = 5, requirements = { { 1, 2 } } }
        assert(not xi.monstrosity.purchaseRequirementsMet(gated, speciesLevel({ [1] = 1 }), function() return false end))

        local variant = { monVariant = 9 }
        assert(not xi.monstrosity.purchaseRequirementsMet(variant, speciesLevel({}), function() return true end))
    end)

    it('admits a locked variant and accepts either new unlock type', function()
        local variant = { monVariant = 9 }
        assert(xi.monstrosity.purchaseRequirementsMet(variant, speciesLevel({}), function() return false end))

        local both = { monSpecies = 5, monVariant = 9 }
        assert(xi.monstrosity.purchaseRequirementsMet(both, speciesLevel({ [5] = 1 }), function() return false end))
    end)
end)

describe('Terynon MON purchase page mask', function()
    local function speciesLevel(levels)
        return function(species) return levels[species] or 0 end
    end

    it('sets bits only for currently purchasable offers', function()
        local offers =
        {
            [0] = { monSpecies = 5 },
            [1] = { monVariant = 9 },
            [5] = { monSpecies = 6, requirements = { { 2, 3 } } },
        }

        local mask = xi.monstrosity.purchasePageMask(offers, speciesLevel({ [2] = 3 }), function(variant) return variant == 9 end)
        assert(mask == bit.lshift(1, 0) + bit.lshift(1, 5), mask)
    end)

    it('returns an empty mask for a missing category or no eligible offers', function()
        assert(xi.monstrosity.purchasePageMask(nil, speciesLevel({}), function() return false end) == 0)

        local offers = { [3] = { monSpecies = 5 } }
        assert(xi.monstrosity.purchasePageMask(offers, speciesLevel({ [5] = 1 }), function() return false end) == 0)
    end)
end)

describe('Terynon MON purchase disposition', function()
    it('charges the exact offer cost and unlocks a species at the cost boundary', function()
        local plan = xi.monstrosity.monPurchasePlan({ infamyCost = 500, monSpecies = 5 }, 500)
        assert(plan.cost == 500 and plan.unlockSpecies == 5 and plan.unlockVariant == nil)
    end)

    it('denies an insufficient-infamy purchase', function()
        assert(xi.monstrosity.monPurchasePlan({ infamyCost = 500, monSpecies = 5 }, 499).deny)
    end)

    it('unlocks a variant when no species is present and preserves species precedence', function()
        local variant = xi.monstrosity.monPurchasePlan({ infamyCost = 750, monVariant = 9 }, 1000)
        assert(variant.cost == 750 and variant.unlockVariant == 9 and variant.unlockSpecies == nil)

        local both = xi.monstrosity.monPurchasePlan({ infamyCost = 1, monSpecies = 5, monVariant = 9 }, 1)
        assert(both.unlockSpecies == 5 and both.unlockVariant == nil)
    end)
end)

describe('Terynon instinct purchase disposition', function()
    it('requires the fixed check value', function()
        assert(xi.monstrosity.instinctPurchasePlan(9, 118, false, 999999).invalid)
    end)

    it('uses the racial price and exact-funds gate', function()
        local plan = xi.monstrosity.instinctPurchasePlan(9, 119, false, 500)
        assert(plan.cost == 500 and plan.purchaseInstinct == 9)
        assert(xi.monstrosity.instinctPurchasePlan(9, 119, false, 499).deny)
    end)

    it('uses the advanced price and only halves it for a completed limit break', function()
        local full = xi.monstrosity.instinctPurchasePlan(10, 119, false, 10000)
        assert(full.cost == 10000 and full.purchaseInstinct == 10)
        assert(xi.monstrosity.instinctPurchasePlan(10, 119, false, 9999).deny)

        local discounted = xi.monstrosity.instinctPurchasePlan(10, 119, true, 5000)
        assert(discounted.cost == 5000 and discounted.purchaseInstinct == 10)
    end)
end)

describe('Terynon Protect and Shell plans', function()
    it('uses the inclusive Protect tier boundaries and tier-scaled enhancement', function()
        assert(xi.monstrosity.protectPlan(26, false).power == 20)
        assert(xi.monstrosity.protectPlan(27, false).power == 50)
        assert(xi.monstrosity.protectPlan(63, false).power == 140)
        local plan = xi.monstrosity.protectPlan(76, true)
        assert(plan.power == 230 and plan.tier == 5 and plan.duration == 1800)
    end)

    it('uses the inclusive Shell tier boundaries and tier-scaled enhancement', function()
        assert(xi.monstrosity.shellPlan(36, false).power == 1055)
        assert(xi.monstrosity.shellPlan(37, false).power == 1641)
        assert(xi.monstrosity.shellPlan(68, false).power == 2617)
        local plan = xi.monstrosity.shellPlan(76, true)
        assert(plan.power == 3125 and plan.tier == 5 and plan.duration == 1800)
    end)
end)

describe('Terynon special-effect catalog', function()
    it('plans fixed Dedication, Regen, and Refresh purchases', function()
        local dedication = xi.monstrosity.specialEffectPlan(0, 1, false, 3000)
        assert(dedication.cost == 3000 and dedication.effect == 'dedication' and dedication.power == 50 and dedication.duration == 3600 and dedication.subpower == 10000)
        local refresh = xi.monstrosity.specialEffectPlan(3, 1, false, 10)
        assert(refresh.effect == 'refresh' and refresh.power == 1 and refresh.tick == 3)
    end)

    it('composes Protect and Shell tiers and denies insufficient infamy', function()
        local protect = xi.monstrosity.specialEffectPlan(4, 76, true, 100)
        assert(protect.effect == 'protect' and protect.power == 230 and protect.tier == 5)
        local shell = xi.monstrosity.specialEffectPlan(5, 76, true, 100)
        assert(shell.effect == 'shell' and shell.power == 3125 and shell.tier == 5)
        assert(xi.monstrosity.specialEffectPlan(1, 1, false, 399).deny)
    end)

    it('keeps Haste free and ignores unknown selections', function()
        local haste = xi.monstrosity.specialEffectPlan(6, 1, false, 0)
        assert(haste.cost == 0 and haste.effect == 'haste' and haste.power == 1000 and haste.duration == 600)
        assert(xi.monstrosity.specialEffectPlan(7, 1, false, 999999) == nil)
    end)
end)
