require('scripts/globals/dynamis')

describe('Dynamis entry trigger preflight', function()
    it('prioritizes Tavnazia first visit, then low level, then the default warning', function()
        local firstVisit = xi.dynamis.entryTriggerPreflightPlan(true, true, true, false, false)
        assert(firstVisit.startFirstVisit and firstVisit.markFirstVisit)

        local lowLevel = xi.dynamis.entryTriggerPreflightPlan(false, true, true, false, false)
        assert(lowLevel.showLowLevel and lowLevel.showDefaultMessage == nil)

        local defaultMessage = xi.dynamis.entryTriggerPreflightPlan(false, true, false, false, false)
        assert(defaultMessage.showDefaultMessage and defaultMessage.showLowLevel == nil)

        local noMessage = xi.dynamis.entryTriggerPreflightPlan(false, false, false, true, false)
        assert(next(noMessage) == nil)

        local beatenDynamis = xi.dynamis.entryTriggerPreflightPlan(false, false, false, false, true)
        assert(next(beatenDynamis) == nil)
    end)
end)

describe('Dynamis entry trigger actions', function()
    it('prioritizes shrouded sand, then victory, then the entry menu', function()
        local shroudedSand = xi.dynamis.entryTriggerActionPlan(true, true, false, false, false)
        assert(shroudedSand.startShroudedSand and shroudedSand.startVictory == nil)

        local victory = xi.dynamis.entryTriggerActionPlan(true, true, true, true, false)
        assert(victory.startVictory and victory.openEntryMenu == nil)

        local entryMenu = xi.dynamis.entryTriggerActionPlan(false, false, false, false, true)
        assert(entryMenu.openEntryMenu and entryMenu.startVictory == nil)

        local blocked = xi.dynamis.entryTriggerActionPlan(false, false, false, false, false)
        assert(next(blocked) == nil)
    end)
end)

describe('Dynamis entry menu', function()
    it('opens only after the strict wait boundary and otherwise reports days remaining', function()
        local entry = xi.dynamis.entryMenuPlan(0, 86401, 24, 7)
        assert(entry.openMenu and entry.sjobOption == 1)

        local lockedAtBoundary = xi.dynamis.entryMenuPlan(0, 86400, 24, 6)
        assert(lockedAtBoundary.showCooldown)

        local cooldown = xi.dynamis.entryMenuPlan(0, 0, 24, 6)
        assert(cooldown.showCooldown and cooldown.daysRemaining == 25)
    end)
end)

describe('Dynamis time-extension groups', function()
    it('normalizes single and grouped extension mobs and rejects missing IDs', function()
        local single = { mob = 100, ki = 200, minutes = 10 }
        local grouped = { mob = { 101, 102 }, ki = 201, minutes = 20 }
        local entries = { single, grouped }

        local te, group = xi.dynamis.findTimeExtensionGroup(entries, 100)
        assert(te == single and #group == 1 and group[1] == 100)

        te, group = xi.dynamis.findTimeExtensionGroup(entries, 102)
        assert(te == grouped and #group == 2 and group[1] == 101 and group[2] == 102)

        te, group = xi.dynamis.findTimeExtensionGroup(entries, 999)
        assert(te == nil and group == nil)

        te, group = xi.dynamis.findTimeExtensionGroup(nil, 100)
        assert(te == nil and group == nil)
    end)
end)

describe('Dynamis refill statue groups', function()
    it('returns the matching statue and full group', function()
        local blue = { mob = 100, eye = 1 }
        local green = { mob = 101, eye = 2 }
        local entries = { { blue, green } }

        local statue, group = xi.dynamis.findRefillStatueGroup(entries, 101)
        assert(statue == green and #group == 2 and group[1] == blue and group[2] == green)

        statue, group = xi.dynamis.findRefillStatueGroup(entries, 999)
        assert(statue == nil and group == nil)
    end)
end)

describe('Dynamis refill statue recovery', function()
    it('maps blue and green statue eyes to their recovery pools', function()
        assert(xi.dynamis.refillStatueRecovery(xi.dynamis.eye.BLUE) == 'mp')
        assert(xi.dynamis.refillStatueRecovery(xi.dynamis.eye.GREEN) == 'hp')
        assert(xi.dynamis.refillStatueRecovery(xi.dynamis.eye.NONE) == nil)
    end)
end)

describe('Dynamis refill statue range', function()
    it('includes players strictly within 30 yalms', function()
        assert(xi.dynamis.canReceiveRefill(29.9))
        assert(not xi.dynamis.canReceiveRefill(30))
        assert(not xi.dynamis.canReceiveRefill(30.1))
    end)
end)

describe('Dynamis refill statue respawns', function()
    it('plans killer group respawns with a five-minute delay', function()
        assert(xi.dynamis.refillStatueRespawnPlan(100, { 100 }, false, 1) == nil)
        local plan = xi.dynamis.refillStatueRespawnPlan(100, { 100, 101 }, true, 2)
        assert(plan.respawnMobId == 101 and plan.disableDead and plan.respawnDelay == 300)
    end)
end)

describe('Dynamis QM trade mobs', function()
    it('selects fixed and grouped mobs', function()
        assert(xi.dynamis.qmTradeMob(100, 1) == 100)
        assert(xi.dynamis.qmTradeMob({ 100, 101 }, 2) == 101)
        assert(xi.dynamis.qmTradeMob({ 100 }, 2) == nil)
        assert(xi.dynamis.qmTradeMob(nil, 1) == nil)
    end)
end)

describe('Dynamis QM trigger plans', function()
    it('prioritizes events and otherwise exposes a single ominous item', function()
        assert(xi.dynamis.qmTriggerPlan({ 1 }, { 100 }, true).event)
        assert(xi.dynamis.qmTriggerPlan(nil, { 100 }, true).ominousItem == 100)
        assert(xi.dynamis.qmTriggerPlan(nil, { 100, 101 }, true) == nil)
        assert(xi.dynamis.qmTriggerPlan(nil, { 100 }, false) == nil)
    end)
end)

describe('Dynamis QM trade confirmation', function()
    it('confirms only successful selected mob pops', function()
        assert(xi.dynamis.shouldConfirmQMTrade(100, true))
        assert(not xi.dynamis.shouldConfirmQMTrade(100, false))
        assert(not xi.dynamis.shouldConfirmQMTrade(nil, true))
    end)
end)

describe('Dynamis QM trade order', function()
    it('returns the first matching configured entry', function()
        local index, item = xi.dynamis.firstQMTradeMatch({ 100, 101, 102 }, function(value) return value >= 101 end)
        assert(index == 2 and item == 101)
    end)
end)

describe('Dynamis mob info', function()
    it('disables gil and mug while setting a 1.5x damage multiplier', function()
        local plan = xi.dynamis.mobInfoPlan()
        assert(plan.gilMax == -1 and plan.mugGil == -1 and plan.baseDamageMultiplier == 150)
    end)
end)

describe('Dynamis refill statues', function()
    it('finds a statue eye configuration across groups', function()
        local blue = { mob = 100, eye = 1 }
        local green = { mob = 101, eye = 2 }
        local entries = { { blue }, { green } }

        assert(xi.dynamis.findRefillStatue(entries, 101) == green)
        assert(xi.dynamis.findRefillStatue(entries, 999) == nil)
        assert(xi.dynamis.findRefillStatue(nil, 100) == nil)
    end)
end)

describe('Dynamis time-extension awards', function()
    it('awards time only to effect holders without the extension key item', function()
        assert(xi.dynamis.timeExtensionAwardPlan(false, false, 10) == nil)
        assert(xi.dynamis.timeExtensionAwardPlan(true, true, 10) == nil)

        local award = xi.dynamis.timeExtensionAwardPlan(true, false, 10)
        assert(award.durationAdded == 600000)
    end)
end)

describe('Dynamis time-extension respawns', function()
    it('plans group replacements only for killers', function()
        assert(xi.dynamis.timeExtensionRespawnPlan(100, { 100, 101 }, false, 2) == nil)
        assert(xi.dynamis.timeExtensionRespawnPlan(100, nil, true, 1) == nil)
        assert(xi.dynamis.timeExtensionRespawnPlan(100, { 100 }, true, 2) == nil)

        local same = xi.dynamis.timeExtensionRespawnPlan(100, { 100, 101 }, true, 1)
        assert(same.respawnMobId == 100 and not same.disableDead and same.respawnDelay == 85)

        local replacement = xi.dynamis.timeExtensionRespawnPlan(100, { 100, 101 }, true, 2)
        assert(replacement.respawnMobId == 101 and replacement.disableDead and replacement.respawnDelay == 85)
    end)
end)
