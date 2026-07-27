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

describe('Pirates route positions', function()
    it('pins the three pirate positions for each route', function()
        local selbina = xi.pirates.positions(xi.zone.SHIP_BOUND_FOR_SELBINA_PIRATES)
        local mhaura = xi.pirates.positions(xi.zone.SHIP_BOUND_FOR_MHAURA_PIRATES)
        assert(#selbina == 3 and #mhaura == 3)
        assert(selbina[1].startPos.x == -33.601 and selbina[1].startPos.rotation == 0)
        assert(selbina[2].standingPos.z == 6.59)
        assert(mhaura[1].startPos.x == 33.601 and mhaura[1].startPos.rotation == 128)
        assert(mhaura[3].standingPos.z == 2.10)
        assert(#xi.pirates.positions(0) == 0)
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

describe('Pirates zone-state plan', function()
    it('does nothing when an NPC repeats the current action', function()
        local plan = xi.pirates.zoneStatePlan(
            xi.pirates.actions.MOBS_SPAWN,
            xi.pirates.actions.MOBS_SPAWN,
            1,
            1
        )

        assert(not plan.setAction)
    end)

    it('clears the deck, summons Crossbones, and selects the NM on an eligible passing ride', function()
        local plan = xi.pirates.zoneStatePlan(0, xi.pirates.actions.MOBS_SPAWN, 1, 75)

        assert(plan.setAction)
        assert(plan.action == xi.pirates.actions.MOBS_SPAWN)
        assert(plan.clearPirates)
        assert(plan.respawnCrossbones)
        assert(plan.spawn == 'nm')
        assert(plan.clearNMCanSpawn)
    end)

    it('summons the Wight for an ineligible ride', function()
        local plan = xi.pirates.zoneStatePlan(0, xi.pirates.actions.MOBS_SPAWN, 0, 1)

        assert(plan.setAction)
        assert(plan.spawn == 'wight')
        assert(not plan.clearNMCanSpawn)
    end)

    it('clears the deck on retreat and only records ordinary actions', function()
        local retreat = xi.pirates.zoneStatePlan(0, xi.pirates.actions.PIRATES_RETREAT, 0, 1)
        assert(retreat.setAction)
        assert(retreat.clearPirates)
        assert(not retreat.respawnCrossbones)

        local depart = xi.pirates.zoneStatePlan(0, xi.pirates.actions.DEPART, 0, 1)
        assert(depart.setAction)
        assert(depart.action == xi.pirates.actions.DEPART)
        assert(not depart.clearPirates)
    end)
end)

describe('Pirates summon-animation plan', function()
    it('does nothing for a hidden pirate and only polls while pathing', function()
        local hidden = xi.pirates.summonAnimationPlan(true, false, false, 100, 0, 0, 1, 1, 5)
        assert(not hidden.scheduleNext)

        local pathing = xi.pirates.summonAnimationPlan(false, true, false, 100, 0, 0, 1, 1, 5)
        assert(pathing.scheduleNext)
        assert(not pathing.hide)
    end)

    it('rotates an arriving pirate and offsets its first summon by NPC order', function()
        local plan = xi.pirates.summonAnimationPlan(false, false, true, 100, 0, 0, 2, 1, 5)

        assert(plan.clearInitialState)
        assert(plan.rotateToBoat)
        assert(plan.setSummonStartTime and plan.summonStartTime == 102)
        assert(plan.scheduleNext)
    end)

    it('starts the first pirate immediately after rotating it', function()
        local plan = xi.pirates.summonAnimationPlan(false, false, true, 100, 0, 0, 1, 1, 5)

        assert(plan.clearInitialState and plan.rotateToBoat)
        assert(plan.setSummonStartTime and plan.summonStartTime == 0)
        assert(plan.setSummonEndTime and plan.summonEndTime == 101)
        assert(plan.startAnimation and plan.scheduleNext)
    end)

    it('starts a due summon and records its randomized end time', function()
        local plan = xi.pirates.summonAnimationPlan(false, false, false, 100, 100, 0, 1, 2, 5)

        assert(plan.setSummonStartTime and plan.summonStartTime == 0)
        assert(plan.setSummonEndTime and plan.summonEndTime == 102)
        assert(plan.startAnimation)
        assert(plan.scheduleNext)
    end)

    it('stops a due summon and schedules the next one with its offset', function()
        local plan = xi.pirates.summonAnimationPlan(false, false, false, 100, 0, 100, 3, 1, 7)

        assert(plan.setSummonStartTime and plan.summonStartTime == 107)
        assert(plan.setSummonEndTime and plan.summonEndTime == 0)
        assert(plan.stopAnimation)
        assert(plan.scheduleNext)
    end)

    it('hides an idle pirate without scheduling another poll', function()
        local plan = xi.pirates.summonAnimationPlan(false, false, false, 100, 0, 0, 1, 1, 5)
        assert(plan.hide)
        assert(not plan.scheduleNext)
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
