require('scripts/globals/mobs')

describe('NM lottery placeholder list', function()
    it('flattens plain and grouped placeholder entries', function()
        local ids = xi.mob.phListNmIds({ [10] = 100, [11] = { 200, 201 }, [12] = 300 })
        table.sort(ids)

        assert(#ids == 4)
        assert(ids[1] == 100 and ids[2] == 200 and ids[3] == 201 and ids[4] == 300)
    end)

    it('returns an empty list for an absent or empty table', function()
        assert(#xi.mob.phListNmIds(nil) == 0)
        assert(#xi.mob.phListNmIds({}) == 0)
    end)
end)

describe('NM lottery primed state', function()
    it('treats a spawned or pending-respawn NM as primed', function()
        assert(xi.mob.nmPrimed(true, 0))
        assert(xi.mob.nmPrimed(false, 120))
        assert(xi.mob.nmPrimed(true, 120))
        assert(not xi.mob.nmPrimed(false, 0))
    end)
end)

describe('NM lottery candidates', function()
    it('yields one candidate for a plain placeholder mapping', function()
        local ids = xi.mob.nmCandidatesForPh({ [10] = 100 }, 10)
        assert(#ids == 1 and ids[1] == 100)
    end)

    it('yields every candidate for a grouped placeholder mapping', function()
        local ids = xi.mob.nmCandidatesForPh({ [10] = { 100, 101 } }, 10)
        table.sort(ids)

        assert(#ids == 2 and ids[1] == 100 and ids[2] == 101)
    end)

    it('yields nothing when the placeholder is not in the list', function()
        assert(#xi.mob.nmCandidatesForPh({ [10] = 100 }, 11) == 0)
        assert(#xi.mob.nmCandidatesForPh(nil, 10) == 0)
        assert(#xi.mob.nmCandidatesForPh({}, 10) == 0)
    end)
end)

describe('NM lottery chance scaling', function()
    it('converts the script chance into the 1..1000 roll domain', function()
        assert(xi.mob.lotteryScaledChance(10, nil) == 100)
        assert(xi.mob.lotteryScaledChance(10, 1.0) == 100)
        assert(xi.mob.lotteryScaledChance(0.5, nil) == 5)
    end)

    it('scales by the configured multiplier and rounds up', function()
        assert(xi.mob.lotteryScaledChance(10, 0.5) == 50)
        assert(xi.mob.lotteryScaledChance(10, 2.0) == 200)
        assert(xi.mob.lotteryScaledChance(0.55, 1.0) == 6)
    end)

    it('forces a guaranteed pop for a negative multiplier', function()
        assert(xi.mob.lotteryScaledChance(10, -1) == 1000)
    end)

    it('never pops for a zero multiplier', function()
        assert(xi.mob.lotteryScaledChance(10, 0) == 0)
    end)
end)

describe('NM lottery cooldown scaling', function()
    it('scales the cooldown by the configured multiplier', function()
        assert(xi.mob.lotteryScaledCooldown(100, nil) == 100)
        assert(xi.mob.lotteryScaledCooldown(100, 1.0) == 100)
        assert(xi.mob.lotteryScaledCooldown(100, 2.0) == 200)
        assert(xi.mob.lotteryScaledCooldown(100, 0) == 0)
    end)

    it('leaves the cooldown alone for a negative multiplier', function()
        assert(xi.mob.lotteryScaledCooldown(100, -1) == 100)
    end)
end)

describe('NM lottery repop hour', function()
    it('reports the Vanadiel hour the next repop lands on', function()
        assert(xi.mob.lotteryRepopHour(0, 0) == 0)
        assert(xi.mob.lotteryRepopHour(0, 5 * xi.vanaTime.HOUR) == 5)
        assert(xi.mob.lotteryRepopHour(2 * xi.vanaTime.HOUR, 3 * xi.vanaTime.HOUR) == 5)
    end)

    it('wraps across the Vanadiel day', function()
        assert(xi.mob.lotteryRepopHour(0, xi.vanaTime.DAY) == 0)
        assert(xi.mob.lotteryRepopHour(0, xi.vanaTime.DAY + 3 * xi.vanaTime.HOUR) == 3)
    end)
end)

describe('NM lottery repop window', function()
    it('blocks a night-only NM whose repop lands in daylight', function()
        assert(xi.mob.lotteryRepopBlocked(false, true, 4))
        assert(xi.mob.lotteryRepopBlocked(false, true, 12))
        assert(xi.mob.lotteryRepopBlocked(false, true, 19))
    end)

    it('allows a night-only NM whose repop lands at night', function()
        assert(not xi.mob.lotteryRepopBlocked(false, true, 3))
        assert(not xi.mob.lotteryRepopBlocked(false, true, 20))
        assert(not xi.mob.lotteryRepopBlocked(false, true, 23))
    end)

    -- The dayOnly arm requires hour < 4 AND hour >= 20, which no hour satisfies,
    -- so dayOnly never blocks a pop. Pinned as-is for parity.
    it('never blocks a day-only NM at any hour', function()
        for hour = 0, 23 do
            assert(not xi.mob.lotteryRepopBlocked(true, false, hour), hour)
        end
    end)

    it('allows an unrestricted NM at any hour', function()
        for hour = 0, 23 do
            assert(not xi.mob.lotteryRepopBlocked(false, false, hour), hour)
        end
    end)
end)

describe('NM lottery pop gate', function()
    it('holds the pop while the cooldown is still running', function()
        assert(xi.mob.lotteryCooldownActive(100, 200))
        assert(xi.mob.lotteryCooldownActive(200, 200))
        assert(not xi.mob.lotteryCooldownActive(201, 200))
    end)

    it('hits on a roll at or under the scaled chance', function()
        assert(xi.mob.lotteryRollPassed(1, 100))
        assert(xi.mob.lotteryRollPassed(100, 100))
        assert(not xi.mob.lotteryRollPassed(101, 100))
        assert(not xi.mob.lotteryRollPassed(1, 0))
    end)

    it('admits the pop only when every gate passes', function()
        assert(xi.mob.lotteryPopAdmitted(201, 200, false, 100, 100))

        assert(not xi.mob.lotteryPopAdmitted(200, 200, false, 100, 100))
        assert(not xi.mob.lotteryPopAdmitted(201, 200, true, 100, 100))
        assert(not xi.mob.lotteryPopAdmitted(201, 200, false, 101, 100))
    end)
end)

describe('NM lottery respawn time', function()
    it('pops immediately or reuses the placeholder timer', function()
        assert(xi.mob.lotteryRespawnTime(true, 900) == 1)
        assert(xi.mob.lotteryRespawnTime(false, 900) == 900)
    end)
end)
