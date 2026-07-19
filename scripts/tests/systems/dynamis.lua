require('scripts/globals/dynamis')

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
