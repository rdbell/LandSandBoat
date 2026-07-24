require('scripts/globals/pirates')

describe('Pirates encounter schedule', function()
    it('runs seven stages on an eight-hour cycle', function()
        assert(#xi.pirates.schedule() == 7)
        assert(xi.pirates.cycleMinutes == 480)
    end)

    it('orders the stages by their end time', function()
        local previous = -1

        for _, entry in ipairs(xi.pirates.schedule()) do
            assert(entry.endTime > previous, entry.endTime)
            previous = entry.endTime
        end
    end)

    it('maps each stage to its action in cycle order', function()
        local expected =
        {
            xi.pirates.actions.ARRIVING,
            xi.pirates.actions.ARRIVE,
            xi.pirates.actions.PIRATES_ARRIVE,
            xi.pirates.actions.MOBS_SPAWN,
            xi.pirates.actions.PIRATES_RETREAT,
            xi.pirates.actions.DEPART,
            xi.pirates.actions.DEPARTING,
        }

        for i, entry in ipairs(xi.pirates.schedule()) do
            assert(entry.action == expected[i], i)
        end
    end)

    it('pins the stage end times in minutes after midnight', function()
        local expected = { 70, 90, 92, 94, 260, 267, 288 }

        for i, entry in ipairs(xi.pirates.schedule()) do
            assert(entry.endTime == expected[i], i .. ':' .. entry.endTime)
        end
    end)

    it('finishes every stage within the cycle', function()
        for _, entry in ipairs(xi.pirates.schedule()) do
            assert(entry.endTime < xi.pirates.cycleMinutes, entry.endTime)
        end
    end)
end)

describe('Pirates verm cloak', function()
    it('dresses only the middle pirate', function()
        assert(xi.pirates.vermCloakPirateIndex == 2)
    end)

    it('wears the cloak on a roll of ten or under', function()
        assert(xi.pirates.vermCloakRollPassed(1))
        assert(xi.pirates.vermCloakRollPassed(10))
        assert(not xi.pirates.vermCloakRollPassed(11))
        assert(not xi.pirates.vermCloakRollPassed(100))
    end)

    it('swaps the body model for the cloak', function()
        assert(xi.pirates.bodyModelId(true) == xi.pirates.vermCloakModelId)
        assert(xi.pirates.bodyModelId(false) == xi.pirates.defaultBodyModelId)
        assert(xi.pirates.vermCloakModelId == 47)
        assert(xi.pirates.defaultBodyModelId == 8195)
    end)
end)

describe('Pirates NM spawn selection', function()
    it('requires the ride to still be NM-eligible', function()
        assert(xi.pirates.nmEligible(1))
        assert(not xi.pirates.nmEligible(0))
    end)

    it('raises the NM on a roll of seventy-five or under', function()
        assert(xi.pirates.nmRollPassed(1))
        assert(xi.pirates.nmRollPassed(75))
        assert(not xi.pirates.nmRollPassed(76))
    end)

    it('raises the NM only on an eligible ride whose roll passes', function()
        assert(xi.pirates.mobsSpawnSelection(1, 75) == 'nm')
        assert(xi.pirates.mobsSpawnSelection(1, 1) == 'nm')
    end)

    it('falls back to the Wight on a missed roll or an ineligible ride', function()
        assert(xi.pirates.mobsSpawnSelection(1, 76) == 'wight')
        assert(xi.pirates.mobsSpawnSelection(0, 1) == 'wight')
        assert(xi.pirates.mobsSpawnSelection(0, 100) == 'wight')
    end)
end)

describe('Pirates route NM', function()
    it('sails Blackbeard on the Selbina route', function()
        assert(xi.pirates.nmIsBlackbeard(xi.zone.SHIP_BOUND_FOR_SELBINA_PIRATES))
    end)

    it('sails Silverhook on the Mhaura route', function()
        assert(not xi.pirates.nmIsBlackbeard(xi.zone.SHIP_BOUND_FOR_MHAURA_PIRATES))
    end)
end)
