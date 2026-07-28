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

describe('Terynon event-update plan', function()
    it('returns the selected MON page mask as its first event argument', function()
        local pages =
        {
            [3] =
            {
                [0] = { monSpecies = 5 },
                [4] = { monVariant = 9 },
            },
        }
        local option = bit.lshift(3, 16)
        local plan = xi.monstrosity.eventUpdatePlan(
            7,
            option,
            pages,
            function() return 0 end,
            function(variant) return variant == 9 end,
            function() return false end,
            function() return false end
        )

        assert(plan[1] == bit.lshift(1, 0), plan[1])
        for index = 2, 8 do
            assert(plan[index] == 0, index)
        end
    end)

    it('returns purchased instincts and completed limit breaks for the instinct page', function()
        local plan = xi.monstrosity.eventUpdatePlan(
            7,
            1,
            {},
            function() return 0 end,
            function() return false end,
            function(instinct) return instinct == xi.monstrosity.purchasableInstincts.HUME_II end,
            function(job) return job == xi.job.WAR end
        )

        assert(plan[1] == 0x1, plan[1])
        assert(plan[2] == 0x1, plan[2])
        for index = 3, 8 do
            assert(plan[index] == 0, index)
        end
    end)

    it('ignores other event IDs and unsupported options', function()
        assert(xi.monstrosity.eventUpdatePlan(6, 0, {}, function() return 0 end, function() return false end, function() return false end, function() return false end) == nil)
        assert(xi.monstrosity.eventUpdatePlan(7, 2, {}, function() return 0 end, function() return false end, function() return false end, function() return false end) == nil)
    end)
end)

describe('Terynon MON event-finish plan', function()
    local offers =
    {
        [2] =
        {
            [8] = { infamyCost = 500, monSpecies = 5 },
        },
    }
    local option = bit.bor(bit.lshift(8, 16), bit.lshift(3, 8), 1)

    it('decodes the selected category and MON before planning its purchase', function()
        local plan = xi.monstrosity.monEventFinishPlan(option, offers, 500)
        assert(plan.selectedCategory == 2 and plan.selectedMon == 8)
        assert(plan.cost == 500 and plan.unlockSpecies == 5)
    end)

    it('preserves the infamy denial and rejects missing categories or offers', function()
        assert(xi.monstrosity.monEventFinishPlan(option, offers, 499).deny)
        assert(xi.monstrosity.monEventFinishPlan(option, {}, 999999).invalid)
        assert(xi.monstrosity.monEventFinishPlan(bit.bor(bit.lshift(8, 16), 1), offers, 999999).invalid)
    end)

    it('ignores other event-finish option types', function()
        assert(xi.monstrosity.monEventFinishPlan(2, offers, 999999) == nil)
    end)
end)

describe('Terynon instinct event-finish plan', function()
    it('decodes an advanced instinct and consults its paired limit break', function()
        local option = bit.bor(bit.lshift(119, 16), bit.lshift(xi.monstrosity.purchasableInstincts.WAR, 8), 2)
        local checkedJob
        local plan = xi.monstrosity.instinctEventFinishPlan(option, function(job)
            checkedJob = job
            return job == xi.job.WAR
        end, 5000)

        assert(checkedJob == xi.job.WAR, checkedJob)
        assert(plan.cost == 5000 and plan.purchaseInstinct == xi.monstrosity.purchasableInstincts.WAR)
    end)

    it('does not consult a limit break for racial instincts', function()
        local option = bit.bor(bit.lshift(119, 16), bit.lshift(xi.monstrosity.purchasableInstincts.HUME_II, 8), 2)
        local plan = xi.monstrosity.instinctEventFinishPlan(option, function()
            assert(false, 'racial instinct should not query a limit break')
        end, 500)

        assert(plan.cost == 500 and plan.purchaseInstinct == xi.monstrosity.purchasableInstincts.HUME_II)
    end)

    it('preserves invalid checks and ignores other option types', function()
        local invalid = bit.bor(bit.lshift(118, 16), bit.lshift(xi.monstrosity.purchasableInstincts.HUME_II, 8), 2)
        assert(xi.monstrosity.instinctEventFinishPlan(invalid, function() return false end, 999999).invalid)
        assert(xi.monstrosity.instinctEventFinishPlan(1, function() return false end, 999999) == nil)
    end)
end)

describe('Terynon special-effect event-finish plan', function()
    it('decodes a type-3 selection and composes its level-scaled effect', function()
        local option = bit.bor(bit.lshift(4, 8), 3)
        local plan = xi.monstrosity.specialEffectEventFinishPlan(option, 76, true, 100)

        assert(plan.effect == 'protect' and plan.cost == 100)
        assert(plan.power == 230 and plan.tier == 5 and plan.duration == 1800)
    end)

    it('keeps Haste free and preserves the catalog denial', function()
        local haste = xi.monstrosity.specialEffectEventFinishPlan(bit.bor(bit.lshift(6, 8), 3), 1, false, 0)
        assert(haste.effect == 'haste' and haste.cost == 0 and haste.power == 1000)

        local denied = xi.monstrosity.specialEffectEventFinishPlan(bit.bor(bit.lshift(1, 8), 3), 1, false, 399)
        assert(denied.deny)
    end)

    it('ignores unknown selections and other option types', function()
        assert(xi.monstrosity.specialEffectEventFinishPlan(bit.bor(bit.lshift(7, 8), 3), 1, false, 999999) == nil)
        assert(xi.monstrosity.specialEffectEventFinishPlan(2, 1, false, 999999) == nil)
    end)
end)

describe('Terynon trigger plan', function()
    it('starts event 7 with infamy followed by seven empty arguments', function()
        local plan = xi.monstrosity.teyrnonTriggerPlan(1, 1234)
        assert(plan.csid == 7 and plan.args[1] == 1234)
        for index = 2, 8 do
            assert(plan.args[index] == 0, index)
        end
    end)

    it('requires the setting to be exactly one', function()
        assert(xi.monstrosity.teyrnonTriggerPlan(0, 1234) == nil)
        assert(xi.monstrosity.teyrnonTriggerPlan(2, 1234) == nil)
    end)
end)

describe('Maccus trigger plan', function()
    it('starts event 9 with its fixed opening arguments', function()
        local plan = xi.monstrosity.maccusTriggerPlan(1)
        assert(plan.csid == 9)
        assert(plan.args[1] == 285 and plan.args[2] == 2 and plan.args[3] == 2)
        for index = 4, 8 do
            assert(plan.args[index] == 0, index)
        end
    end)

    it('requires the setting to be exactly one', function()
        assert(xi.monstrosity.maccusTriggerPlan(0) == nil)
        assert(xi.monstrosity.maccusTriggerPlan(2) == nil)
    end)
end)

describe('Aengus trigger plan', function()
    it('starts event 13 with belligerency and infamy', function()
        local belligerent = xi.monstrosity.aengusTriggerPlan(1, true, 1234)
        assert(belligerent.csid == 13 and belligerent.args[1] == 1 and belligerent.args[2] == 1234)

        local peaceful = xi.monstrosity.aengusTriggerPlan(1, false, 1234)
        assert(peaceful.args[1] == 0 and peaceful.args[2] == 1234)
        for index = 3, 8 do
            assert(peaceful.args[index] == 0, index)
        end
    end)

    it('requires the setting to be exactly one', function()
        assert(xi.monstrosity.aengusTriggerPlan(0, true, 1234) == nil)
        assert(xi.monstrosity.aengusTriggerPlan(2, true, 1234) == nil)
    end)
end)

describe('Aengus event-finish plan', function()
    it('toggles belligerency only for event 13 option 1', function()
        assert(xi.monstrosity.aengusEventFinishPlan(13, 1, false).belligerency)
        assert(not xi.monstrosity.aengusEventFinishPlan(13, 1, true).belligerency)
    end)

    it('ignores other events and options', function()
        assert(xi.monstrosity.aengusEventFinishPlan(12, 1, false) == nil)
        assert(xi.monstrosity.aengusEventFinishPlan(13, 0, false) == nil)
        assert(xi.monstrosity.aengusEventFinishPlan(13, 2, true) == nil)
    end)
end)

describe('Odyssean Passage trigger plan', function()
    it('starts event 5 with MON size and belligerency', function()
        local plan = xi.monstrosity.odysseanPassageTriggerPlan(1, 42, true, 0)
        assert(plan.csid == 5 and plan.args[1] == 0 and plan.args[2] == 42 and plan.args[3] == 1)
        for index = 4, 8 do
            assert(plan.args[index] == 0, index)
        end
    end)

    it('lets the exact PvP bypass value suppress belligerency', function()
        assert(xi.monstrosity.odysseanPassageTriggerPlan(1, 42, true, 1).args[3] == 0)
        assert(xi.monstrosity.odysseanPassageTriggerPlan(1, 42, true, 2).args[3] == 1)
        assert(xi.monstrosity.odysseanPassageTriggerPlan(1, 42, false, 0).args[3] == 0)
    end)

    it('requires Monstrosity to be exactly enabled', function()
        assert(xi.monstrosity.odysseanPassageTriggerPlan(0, 42, true, 0) == nil)
        assert(xi.monstrosity.odysseanPassageTriggerPlan(2, 42, true, 0) == nil)
    end)
end)

describe('Odyssean Passage event-update plan', function()
    it('looks up all configured belligerency caps from the selected zone', function()
        local caps =
        {
            [xi.zone.BUBURIMU_PENINSULA] = 30,
            [xi.zone.XARCABARD]          = 60,
            [xi.zone.ULEGUERAND_RANGE]   = 90,
        }

        for zone, cap in pairs(caps) do
            local plan = xi.monstrosity.odysseanPassageEventUpdatePlan(bit.lshift(zone, 4))
            assert(plan[1] == cap, zone)
            assert(plan[5] == 1, zone)
            for _, index in ipairs({ 2, 3, 4, 6, 7, 8 }) do
                assert(plan[index] == 0, index)
            end
        end
    end)

    it('preserves an absent cap for an unknown selected zone', function()
        local plan = xi.monstrosity.odysseanPassageEventUpdatePlan(bit.lshift(1, 4))
        assert(plan[1] == nil and plan[5] == 1)
    end)
end)

describe('Odyssean Passage event-finish plan', function()
    it('returns to the entrance only for option type one with zone zero', function()
        assert(xi.monstrosity.odysseanPassageEventFinishPlan(1, function()
            assert(false, 'return should not choose a teleport')
        end).returnToEntrance)
        assert(xi.monstrosity.odysseanPassageEventFinishPlan(0, function() return 1 end) == nil)
    end)

    it('uses the caller-selected position from every configured destination zone', function()
        local cases =
        {
            { xi.zone.EAST_RONFAURE,        1, { 120, 0.5, -530, 192 } },
            { xi.zone.EAST_RONFAURE,        2, { 115, -59.684, 247, 16 } },
            { xi.zone.QUFIM_ISLAND,         1, { -2, -20.001, 324, 64 } },
            { xi.zone.QUFIM_ISLAND,         2, { 161, -20, 37, 192 } },
            { xi.zone.SOUTH_GUSTABERG,      1, { -115, -0.136, -165, 64 } },
            { xi.zone.VALKURM_DUNES,        1, { 838, 0, -162, 64 } },
            { xi.zone.WESTERN_ALTEPA_DESERT, 1, { 685.548, -1.744, -50.395, 128 } },
        }

        for _, test in ipairs(cases) do
            local zone, choice, expected = test[1], test[2], test[3]
            local plan = xi.monstrosity.odysseanPassageEventFinishPlan(bit.bor(bit.lshift(zone, 4), 1), function(count)
                assert(count >= choice)
                return choice
            end)
            assert(plan.zone == zone and plan.position[1] == expected[1] and plan.position[2] == expected[2])
            assert(plan.position[3] == expected[3] and plan.position[4] == expected[4])
        end
    end)

    it('uses the source zero-position fallback for an unknown selected zone', function()
        local plan = xi.monstrosity.odysseanPassageEventFinishPlan(bit.bor(bit.lshift(1, 4), 1), function()
            assert(false, 'fallback should not choose a teleport')
        end)
        assert(plan.fallback and plan.zone == 1)
        assert(plan.position[1] == 0 and plan.position[2] == 0 and plan.position[3] == 0 and plan.position[4] == 0)
    end)
end)

describe('Monstrosity return-to-entrance plan', function()
    local entry =
    {
        entry_x       = 12.5,
        entry_y       = -3,
        entry_z       = 42,
        entry_rot     = 16,
        entry_zone_id = 101,
        entry_mjob    = xi.job.WAR,
        entry_sjob    = xi.job.WHM,
    }

    it('uses the exact-one Feretory override only outside Feretory', function()
        local plan = xi.monstrosity.returnToEntrancePlan(1, xi.zone.EAST_RONFAURE, entry)
        assert(plan.clearEffects and not plan.restoreJobs and plan.zone == xi.zone.FERETORY)
        assert(plan.position[1] == -358 and plan.position[2] == -3.4 and plan.position[3] == -440 and plan.position[4] == 64)

        local fallthrough = xi.monstrosity.returnToEntrancePlan(1, xi.zone.FERETORY, entry)
        assert(fallthrough.restoreJobs and fallthrough.zone == entry.entry_zone_id)
    end)

    it('restores jobs and the entry position when the override is disabled', function()
        local plan = xi.monstrosity.returnToEntrancePlan(0, xi.zone.EAST_RONFAURE, entry)
        assert(plan.clearEffects and plan.restoreJobs)
        assert(plan.mainJob == entry.entry_mjob and plan.subJob == entry.entry_sjob and plan.zone == entry.entry_zone_id)
        assert(plan.position[1] == entry.entry_x and plan.position[2] == entry.entry_y and plan.position[3] == entry.entry_z and plan.position[4] == entry.entry_rot)

        assert(xi.monstrosity.returnToEntrancePlan(2, xi.zone.EAST_RONFAURE, entry).restoreJobs)
    end)
end)

describe('Feretory zone-in plan', function()
    it('corrects the zero-position entry before applying enabled Monstrosity actions', function()
        local plan = xi.monstrosity.feretoryZoneInPlan(1, 0, 0, 0, xi.job.WAR)
        assert(plan.position[1] == -358 and plan.position[2] == -3.4 and plan.position[3] == -440 and plan.position[4] == 63)
        assert(plan.changeJob and plan.mainJob == xi.job.MON and plan.clearEffects)
    end)

    it('does not correct nonzero positions, and skips changing an existing MON job', function()
        local plan = xi.monstrosity.feretoryZoneInPlan(1, 0, 0, 1, xi.job.MON)
        assert(plan.position == nil and not plan.changeJob and plan.clearEffects)
    end)

    it('uses exact-one enable gating after the zero-position correction', function()
        for _, enabled in ipairs({ 0, 2 }) do
            local plan = xi.monstrosity.feretoryZoneInPlan(enabled, 0, 0, 0, xi.job.WAR)
            assert(plan.position[1] == -358 and plan.position[2] == -3.4 and plan.position[3] == -440 and plan.position[4] == 63)
            assert(not plan.changeJob and not plan.clearEffects)
        end
    end)
end)

describe('Feretory zone-out plan', function()
    it('removes both zoning-survival flags when Monstrosity is exactly enabled', function()
        local plan = xi.monstrosity.feretoryZoneOutPlan(1)
        assert(plan.effectFlags[1] == xi.effectFlag.ON_ZONE)
        assert(plan.effectFlags[2] == xi.effectFlag.LOGOUT)
    end)

    it('does nothing unless Monstrosity is exactly enabled', function()
        assert(xi.monstrosity.feretoryZoneOutPlan(0) == nil)
        assert(xi.monstrosity.feretoryZoneOutPlan(2) == nil)
    end)
end)

describe('Relinquish countdown tick plan', function()
    it('clamps low counters and emits the four-to-one countdown', function()
        local initial = xi.monstrosity.relinquishTickPlan(-1)
        assert(initial.countdown == 4 and initial.nextCountdown == 1)

        local second = xi.monstrosity.relinquishTickPlan(1)
        assert(second.countdown == 3 and second.nextCountdown == 2)

        local middle = xi.monstrosity.relinquishTickPlan(2)
        assert(middle.countdown == 2 and middle.nextCountdown == 3)

        local last = xi.monstrosity.relinquishTickPlan(3)
        assert(last.countdown == 1 and last.nextCountdown == 4)
    end)

    it('returns to the entrance from step four and on clamped higher counters', function()
        local stepFour = xi.monstrosity.relinquishTickPlan(4)
        assert(stepFour.returnToEntrance and stepFour.nextCountdown == 5)

        local clamped = xi.monstrosity.relinquishTickPlan(99)
        assert(clamped.returnToEntrance and clamped.nextCountdown == 5)
    end)
end)

describe('Level-based instinct update', function()
    it('packs 30/60/90 tiers by species and preserves existing bits', function()
        local levels = setmetatable(
        {
            [xi.monstrosity.species.RABBIT] = 30,
            [xi.monstrosity.species.TIGER]  = 60,
            [xi.monstrosity.species.SHEEP]  = 90,
        }, { __index = function() return 0 end })
        local instincts = { [0] = 0x01 }

        xi.monstrosity.updateLevelBasedInstincts(levels, instincts)

        assert(instincts[0] == 0x85)
        assert(instincts[1] == 0x03)
    end)

    it('writes Astoltian Slime and Eorzean Spriggan into the final instinct byte', function()
        local levels = setmetatable(
        {
            [xi.monstrosity.species.ASTOLTIAN_SLIME]  = 60,
            [xi.monstrosity.species.EORZEAN_SPRIGGAN] = 90,
        }, { __index = function() return 0 end })
        local instincts = {}

        xi.monstrosity.updateLevelBasedInstincts(levels, instincts)

        assert(instincts[63] == 0xE0 and instincts[31] == nil)
    end)

    it('does not update species IDs outside the source catalog', function()
        local levels = setmetatable({ [14] = 99 }, { __index = function() return 0 end })
        local instincts = { [3] = 0x5A }

        xi.monstrosity.updateLevelBasedInstincts(levels, instincts)

        assert(instincts[3] == 0x5A)
    end)
end)

describe('Monstrosity unlock-all data', function()
    it('unlocks every source species, purchasable instinct, and variant without clearing other data', function()
        local data = {
            levels = { [14] = 47 },
            instincts = { [0] = 0x01, [63] = 0x01 },
            variants = { [31] = 0x01 },
        }

        xi.monstrosity.applyUnlockAllData(data)

        for _, species in pairs(xi.monstrosity.species) do
            assert(data.levels[species] == 99)
        end
        assert(data.levels[14] == 47)

        assert(data.instincts[0] == 0xFD and data.instincts[63] == 0xF1)
        for byteOffset = 20, 23 do
            assert(data.instincts[byteOffset] == 0xFF)
        end

        for _, variant in pairs(xi.monstrosity.variants) do
            local byteOffset = math.floor(variant / 8)
            local shiftAmount = variant % 8
            assert(bit.band(data.variants[byteOffset], bit.lshift(0x01, shiftAmount)) ~= 0)
        end
        assert(data.variants[31] == 0xF1)
    end)
end)

describe('Monstrosity species and variant unlock data', function()
    it('unlocks a locked species once without lowering existing levels', function()
        local levels = { [xi.monstrosity.species.RABBIT] = 0, [xi.monstrosity.species.TIGER] = 99 }

        assert(xi.monstrosity.unlockSpeciesData(levels, xi.monstrosity.species.RABBIT))
        assert(levels[xi.monstrosity.species.RABBIT] == 1)
        assert(not xi.monstrosity.unlockSpeciesData(levels, xi.monstrosity.species.RABBIT))
        assert(not xi.monstrosity.unlockSpeciesData(levels, xi.monstrosity.species.TIGER))
        assert(levels[xi.monstrosity.species.TIGER] == 99)
    end)

    it('sets a valid variant bit once and rejects an out-of-range ID', function()
        local variants = {}

        assert(xi.monstrosity.unlockVariantData(variants, xi.monstrosity.variants.LAPINION))
        assert(variants[0] == 0x04)
        assert(not xi.monstrosity.unlockVariantData(variants, xi.monstrosity.variants.LAPINION))
        assert(not xi.monstrosity.unlockVariantData(variants, 256))
        assert(variants[32] == nil)
    end)
end)

describe('Monstrosity starting data', function()
    it('copies the selected species into both starter identifiers', function()
        local rabbit = xi.monstrosity.startingMONData(xi.monstrosity.species.RABBIT)
        local spriggan = xi.monstrosity.startingMONData(xi.monstrosity.species.EORZEAN_SPRIGGAN)

        assert(rabbit.monstrosityId == 1 and rabbit.species == 1)
        assert(spriggan.monstrosityId == 127 and spriggan.species == 127)
    end)
end)
