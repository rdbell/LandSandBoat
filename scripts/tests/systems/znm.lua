require('scripts/globals/znm')

describe('ZNM Sanraku rotation plan', function()
    it('retains each newly rolled server value', function()
        local plan = xi.znm.sanrakuRotationPlan(4, 9, 123)
        assert(plan.interest == 4 and plan.fauna == 9 and plan.trades == 123)
    end)
end)

describe('ZNM Sanraku trade rotation', function()
    it('rotates after 500 completed trades', function()
        assert(not xi.znm.shouldRotateSanrakuTrades(499))
        assert(xi.znm.shouldRotateSanrakuTrades(500))
        assert(xi.znm.shouldRotateSanrakuTrades(501))
    end)
end)

describe('ZNM Sanraku trade increment', function()
    it('increments the counter between rotations', function()
        assert(xi.znm.nextSanrakuTradeCount(0) == 1)
        assert(xi.znm.nextSanrakuTradeCount(499) == 500)
    end)
end)

describe('ZNM Sanraku trade plan', function()
    it('rotates at the limit and otherwise increments the counter', function()
        local rotate = xi.znm.serverPlateTradesPlan(500)
        assert(rotate.rotate and rotate.trades == nil)

        local increment = xi.znm.serverPlateTradesPlan(499)
        assert(increment.rotate == nil and increment.trades == 500)
    end)
end)

describe('ZNM Sanraku selection initialization', function()
    it('initializes missing and zero selections only', function()
        assert(xi.znm.shouldInitializeSanrakuSelection(nil))
        assert(xi.znm.shouldInitializeSanrakuSelection(0))
        assert(not xi.znm.shouldInitializeSanrakuSelection(1))
    end)
end)

describe('ZNM Sanraku selection value', function()
    it('uses an initial roll only when the selection is uninitialized', function()
        assert(xi.znm.selectSanrakuSelection(nil, 17) == 17)
        assert(xi.znm.selectSanrakuSelection(0, 17) == 17)
        assert(xi.znm.selectSanrakuSelection(3, 17) == 3)
    end)
end)

describe('ZNM Sanraku daily tracking reset', function()
    it('clears the trade day and plate count', function()
        local plan = xi.znm.dailyTrackingResetPlan()
        assert(plan.tradingDay == 0 and plan.tradedPlates == 0)
    end)
end)

describe('ZNM Sanraku plate trade day', function()
    it('clears stale-day trade counts but retains current-day counts', function()
        local stale = xi.znm.plateTradeDayPlan(100, 99, 7)
        assert(stale.reset and stale.tradedPlates == 0)

        local current = xi.znm.plateTradeDayPlan(100, 100, 7)
        assert(current.reset == nil and current.tradedPlates == 7)
    end)
end)

describe('ZNM Sanraku plate trade admission', function()
    it('enforces the daily limit unless Rhapsody resets tracking', function()
        local allowed = xi.znm.sanraku.plateTradeAdmissionPlan(false, 9, 10)
        assert(allowed.accept and allowed.resetTracking == nil)

        local denied = xi.znm.sanraku.plateTradeAdmissionPlan(false, 10, 10)
        assert(not denied.accept and denied.resetTracking == nil)

        local rhapsody = xi.znm.sanraku.plateTradeAdmissionPlan(true, 10, 10)
        assert(rhapsody.accept and rhapsody.resetTracking)
    end)
end)

describe('ZNM Sanraku completed plate trade', function()
    it('records the day, awards staged zeni, and clears the staged value', function()
        local plan = xi.znm.sanraku.completedPlateTradePlan(123, 45)
        assert(plan.day == 123 and plan.zeniValue == 45 and plan.clearTradedPlateValue)
    end)
end)

describe('ZNM Sanraku trophy trade', function()
    it('ignores unknown trophies, rejects owned seals, and stages new seals', function()
        assert(xi.znm.sanraku.trophyTradePlan(nil, false).kind == 'ignore')
        assert(xi.znm.sanraku.trophyTradePlan(1000, true).kind == 'already_owned')

        local staged = xi.znm.sanraku.trophyTradePlan(1000, false)
        assert(staged.kind == 'stage' and staged.seal == 1000)
    end)
end)

describe('ZNM Sanraku trigger', function()
    it('selects introduction, no-zeni, or menu interaction', function()
        assert(xi.znm.sanraku.triggerOutcome(false, false) == 'introduction')
        assert(xi.znm.sanraku.triggerOutcome(true, false) == 'no_zeni')
        assert(xi.znm.sanraku.triggerOutcome(true, true) == 'menu')
    end)
end)

describe('ZNM Sanraku islet access', function()
    it('derives salt key items and applies the Rhapsody cost', function()
        local normal = xi.znm.sanraku.isletAccessPlan(false, 300)
        assert(normal.zeniCost == 500 and normal.keyItem == xi.ki.SICKLEMOON_SALT)

        local rhapsody = xi.znm.sanraku.isletAccessPlan(true, 302)
        assert(rhapsody.zeniCost == 50 and rhapsody.keyItem == xi.ki.SICKLEMOON_SALT + 2)
    end)
end)

describe('ZNM Sanraku islet access outcome', function()
    it('prioritizes zeni, then salt ownership, then purchase', function()
        assert(xi.znm.sanraku.isletAccessOutcome(false, false) == 'no_zeni')
        assert(xi.znm.sanraku.isletAccessOutcome(true, true) == 'already_owned')
        assert(xi.znm.sanraku.isletAccessOutcome(true, false) == 'purchase')
    end)
end)

describe('ZNM Sanraku confirmed pop index', function()
    it('maps options to catalog indices and clamps Warden to the final entry', function()
        assert(xi.znm.sanraku.confirmedZNMPopIndex(400) == 1)
        assert(xi.znm.sanraku.confirmedZNMPopIndex(430) == 31)
        assert(xi.znm.sanraku.confirmedZNMPopIndex(440) == 31)
    end)
end)

describe('ZNM Sanraku confirmed pop purchase', function()
    it('prioritizes zeni, then availability, then purchase', function()
        assert(xi.znm.sanraku.confirmedZNMPopPurchaseOutcome(false, true, false) == 'no_zeni')
        assert(xi.znm.sanraku.confirmedZNMPopPurchaseOutcome(true, false, false) == 'unavailable')
        assert(xi.znm.sanraku.confirmedZNMPopPurchaseOutcome(true, true, true) == 'unavailable')
        assert(xi.znm.sanraku.confirmedZNMPopPurchaseOutcome(true, true, false) == 'purchase')
    end)
end)

describe('ZNM Sanraku event finish', function()
    it('routes each recognized event and ignores others', function()
        assert(xi.znm.sanraku.eventFinishOutcome(910) == 'complete_plate_trade')
        assert(xi.znm.sanraku.eventFinishOutcome(908) == 'mark_introduction_seen')
        assert(xi.znm.sanraku.eventFinishOutcome(912) == 'complete_trophy_trade')
        assert(xi.znm.sanraku.eventFinishOutcome(909) == nil)
    end)
end)

describe('ZNM Sanraku completed trophy trade', function()
    it('awards the staged seal and clears it afterward', function()
        local plan = xi.znm.sanraku.completedTrophyTradePlan(1000)
        assert(plan.seal == 1000 and plan.clearTradedTrophySeal)
    end)
end)

describe('ZNM Sanraku event update', function()
    it('routes supported menu options only for event 909', function()
        assert(xi.znm.sanraku.eventUpdateOutcome(909, 1) == 'islets_menu')
        assert(xi.znm.sanraku.eventUpdateOutcome(909, 500) == 'islets_menu')
        assert(xi.znm.sanraku.eventUpdateOutcome(909, 300) == 'islets_access')
        assert(xi.znm.sanraku.eventUpdateOutcome(909, 100) == 'confirm_info')
        assert(xi.znm.sanraku.eventUpdateOutcome(909, 400) == 'confirmed_info')
        assert(xi.znm.sanraku.eventUpdateOutcome(908, 400) == nil)
        assert(xi.znm.sanraku.eventUpdateOutcome(909, 200) == nil)
    end)
end)

describe('ZNM Sanraku Islets menu', function()
    it('unlocks the menu at Zeni status 3', function()
        assert(xi.znm.sanraku.isletsMenuParam(2) == 0)
        assert(xi.znm.sanraku.isletsMenuParam(3) == 1)
    end)
end)

describe('ZNM Sanraku confirming pop index', function()
    it('maps confirmation options to one-based catalog indices', function()
        assert(xi.znm.sanraku.confirmingZNMPopIndex(100) == 1)
        assert(xi.znm.sanraku.confirmingZNMPopIndex(130) == 31)
    end)
end)

describe('ZNM Ryo menu parameter', function()
    it('unlocks dialogue choices from zeni status and balance', function()
        assert(xi.znm.ryo.menuParam(1, 0) == 175)
        assert(xi.znm.ryo.menuParam(2, 0) == 163)
        assert(xi.znm.ryo.menuParam(1, 1) == 44)
        assert(xi.znm.ryo.menuParam(2, 1000) == 0)
    end)
end)

describe('ZNM Ryo Zeni status', function()
    it('advances only through the matching dialogue options', function()
        assert(xi.znm.ryo.nextZeniStatus(401, 1) == 2)
        assert(xi.znm.ryo.nextZeniStatus(402, 2) == 3)
        assert(xi.znm.ryo.nextZeniStatus(401, 2) == nil)
        assert(xi.znm.ryo.nextZeniStatus(402, 1) == nil)
        assert(xi.znm.ryo.nextZeniStatus(404, 2) == nil)
    end)
end)

describe('ZNM Ryo event update', function()
    it('routes plate and dialogue updates, preserving the menu default', function()
        assert(xi.znm.ryo.eventUpdateOutcome(914, 0) == 'plate_value')
        assert(xi.znm.ryo.eventUpdateOutcome(913, 200) == 'interest')
        assert(xi.znm.ryo.eventUpdateOutcome(913, 201) == 'fauna')
        assert(xi.znm.ryo.eventUpdateOutcome(913, 300) == 'zeni_balance')
        assert(xi.znm.ryo.eventUpdateOutcome(913, 401) == 'zeni_status')
        assert(xi.znm.ryo.eventUpdateOutcome(913, 402) == 'zeni_status')
        assert(xi.znm.ryo.eventUpdateOutcome(913, 404) == 'menu')
        assert(xi.znm.ryo.eventUpdateOutcome(913, 999) == 'default')
        assert(xi.znm.ryo.eventUpdateOutcome(912, 200) == nil)
    end)
end)

describe('ZNM Ryo trigger', function()
    it('opens the menu only after speaking to Sanraku', function()
        assert(xi.znm.ryo.triggerOutcome(false) == 'forbidden')
        assert(xi.znm.ryo.triggerOutcome(true) == 'menu')
    end)
end)

describe('ZNM Ryo plate value update', function()
    it('returns staged zeni and clears the staging value', function()
        local plan = xi.znm.ryo.plateValueUpdatePlan(45)
        assert(plan.zeniValue == 45 and plan.clearTradedPlateValue)
    end)
end)

describe('ZNM Ryo event finish', function()
    it('cleans only the plate-value trade event', function()
        assert(xi.znm.ryo.eventFinishOutcome(914) == 'clean_trade')
        assert(xi.znm.ryo.eventFinishOutcome(913) == nil)
    end)

    it('clears a reserved item when present and always cleans the trade', function()
        local withItem = xi.znm.ryo.tradeCleanupPlan(true)
        assert(withItem.clearReservedValue and withItem.cleanTrade)

        local withoutItem = xi.znm.ryo.tradeCleanupPlan(false)
        assert(not withoutItem.clearReservedValue and withoutItem.cleanTrade)
    end)
end)

describe('ZNM pop price initialization', function()
    it('initializes absent and zero prices but preserves positive prices', function()
        local absent = xi.znm.popPricePlan(nil, 1000)
        assert(absent.price == 1000 and absent.initialize)

        local zero = xi.znm.popPricePlan(0, 1000)
        assert(zero.price == 1000 and zero.initialize)

        local existing = xi.znm.popPricePlan(1200, 1000)
        assert(existing.price == 1200 and existing.initialize == nil)
    end)
end)

describe('ZNM Ryo trade', function()
    it('accepts only an exact soul-plate trade', function()
        assert(xi.znm.ryo.tradeOutcome(true) == 'plate_trade')
        assert(xi.znm.ryo.tradeOutcome(false) == nil)
    end)
end)

describe('ZNM soul plate fauna bonus', function()
    it('matches a single recommended fauna name in its zone', function()
        local row = { zone = 100, name = 'Bugbear' }
        assert(xi.znm.isCurrentFaunaRow({ zoneId = 100, signature = 'Bugbear' }, row))
        assert(not xi.znm.isCurrentFaunaRow({ zoneId = 100, signature = 'Goblin' }, row))
    end)

    it('matches any name when the row lists several', function()
        local row = { zone = 100, name = { 'Bugbear', 'Goblin' } }
        assert(xi.znm.isCurrentFaunaRow({ zoneId = 100, signature = 'Bugbear' }, row))
        assert(xi.znm.isCurrentFaunaRow({ zoneId = 100, signature = 'Goblin' }, row))
        assert(not xi.znm.isCurrentFaunaRow({ zoneId = 100, signature = 'Orc' }, row))
    end)

    it('requires the plate to come from the recommended zone', function()
        local row = { zone = 100, name = 'Bugbear' }
        assert(not xi.znm.isCurrentFaunaRow({ zoneId = 101, signature = 'Bugbear' }, row))
    end)
end)

describe('ZNM soul plate family bonus', function()
    it('matches the subject family', function()
        local row = { family = 5 }
        assert(xi.znm.isCurrentFamilyRow({ familyId = 5 }, row, 1))
        assert(not xi.znm.isCurrentFamilyRow({ familyId = 6 }, row, 1))
    end)

    it('disambiguates elementals by signature for interests 45 to 51', function()
        local row = { family = 5, name = 'Fire Elemental' }

        -- Inside the elemental band the signature must match too.
        assert(xi.znm.isCurrentFamilyRow({ familyId = 5, signature = 'Fire Elemental' }, row, 45))
        assert(xi.znm.isCurrentFamilyRow({ familyId = 5, signature = 'Fire Elemental' }, row, 51))
        assert(not xi.znm.isCurrentFamilyRow({ familyId = 5, signature = 'Ice Elemental' }, row, 45))

        -- Outside it the signature is ignored.
        assert(xi.znm.isCurrentFamilyRow({ familyId = 5, signature = 'Ice Elemental' }, row, 44))
        assert(xi.znm.isCurrentFamilyRow({ familyId = 5, signature = 'Ice Elemental' }, row, 52))
    end)
end)

describe('ZNM soul plate ecosystem bonus', function()
    it('matches any family in the interest ecosystem', function()
        local row = { ecoSystem = { 5, 6, 7 } }
        assert(xi.znm.isCurrentEcosystemRow({ familyId = 6 }, row))
        assert(not xi.znm.isCurrentEcosystemRow({ familyId = 8 }, row))
    end)
end)

describe('ZNM soul plate bonus selection', function()
    local faunaRow    = { zone = 100, name = 'Bugbear' }
    local interestRow = { family = 5, name = 'Bugbear', ecoSystem = { 5, 6 } }

    it('prefers fauna over family and ecosystem', function()
        local plate = { zoneId = 100, signature = 'Bugbear', familyId = 5 }
        assert(xi.znm.plateBonusKind(plate, interestRow, 1, faunaRow) == 'Fauna')
    end)

    it('prefers family over ecosystem', function()
        local plate = { zoneId = 999, signature = 'Bugbear', familyId = 5 }
        assert(xi.znm.plateBonusKind(plate, interestRow, 1, faunaRow) == 'family')
    end)

    it('falls back to ecosystem', function()
        local plate = { zoneId = 999, signature = 'Other', familyId = 6 }
        assert(xi.znm.plateBonusKind(plate, interestRow, 1, faunaRow) == 'ecoSystem')
    end)

    it('reports no bonus when nothing matches', function()
        local plate = { zoneId = 999, signature = 'Other', familyId = 9 }
        assert(xi.znm.plateBonusKind(plate, interestRow, 1, faunaRow) == 'none')
    end)

    it('pins each bonus value', function()
        assert(xi.znm.plateBonusZeni('Fauna') == xi.znm.SOULPLATE_FAUNA)
        assert(xi.znm.plateBonusZeni('family') == xi.znm.SOULPLATE_INTEREST)
        assert(xi.znm.plateBonusZeni('ecoSystem') == xi.znm.SOULPLATE_ECOSYSTEM)
        assert(xi.znm.plateBonusZeni('none') == 0)

        -- Fauna is the richest bonus, ecosystem the leanest.
        assert(xi.znm.SOULPLATE_FAUNA > xi.znm.SOULPLATE_INTEREST)
        assert(xi.znm.SOULPLATE_INTEREST > xi.znm.SOULPLATE_ECOSYSTEM)
    end)
end)

describe('ZNM soul plate zeni value', function()
    it('adds the bonus to the plate quality', function()
        assert(xi.znm.plateZeniValue(50, 'none', 75) == 50)
        assert(xi.znm.plateZeniValue(50, 'ecoSystem', 75) == 50 + xi.znm.SOULPLATE_ECOSYSTEM)
        assert(xi.znm.plateZeniValue(50, 'Fauna', 75) == 50 + xi.znm.SOULPLATE_FAUNA)
    end)

    it('thirds the value for characters at or below level ten', function()
        assert(xi.znm.plateZeniValue(60, 'none', 10) == 20)
        assert(xi.znm.plateZeniValue(60, 'none', 11) == 60)
    end)

    it('clamps to the plate value bounds', function()
        assert(xi.znm.plateZeniValue(0, 'none', 75) == xi.znm.SOULPLATE_MIN_VALUE)
        assert(xi.znm.plateZeniValue(1000, 'Fauna', 75) == xi.znm.SOULPLATE_MAX_VALUE)

        -- The low-level third applies before the clamp, so the floor still holds.
        assert(xi.znm.plateZeniValue(3, 'none', 5) == xi.znm.SOULPLATE_MIN_VALUE)
    end)
end)
