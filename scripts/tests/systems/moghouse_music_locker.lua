require('scripts/globals/moghouse')

describe('Mog House available music', function()
    it('offers nothing with no music furnishings installed', function()
        assert(#xi.moghouse.availableMusic(false, false, false, false) == 0)
    end)

    it('offers the Spinet catalogue', function()
        local songs = xi.moghouse.availableMusic(false, true, false, false)
        assert(#songs == 14)
        assert(songs[1] == 112)   -- Selbina
        assert(songs[14] == 126)  -- Mog House
    end)

    it('adds one song per Nanaa Mihgo statue', function()
        assert(#xi.moghouse.availableMusic(false, false, true, false) == 1)
        assert(xi.moghouse.availableMusic(false, false, true, false)[1] == 69)

        assert(#xi.moghouse.availableMusic(false, false, false, true) == 1)
        assert(xi.moghouse.availableMusic(false, false, false, true)[1] == 59)

        assert(#xi.moghouse.availableMusic(false, true, true, true) == 16)
    end)

    -- An installed Orchestrion drives its own music, so nothing else applies.
    it('offers nothing at all while an Orchestrion is installed', function()
        assert(#xi.moghouse.availableMusic(true, true, true, true) == 0)
        assert(#xi.moghouse.availableMusic(true, false, false, false) == 0)
    end)
end)

describe('Mog Locker days per bronze', function()
    -- Al Zahbi-only access buys more days per coin than all-areas access.
    it('gives more days for the restricted access type', function()
        assert(xi.moghouse.mogLockerDaysPerBronze(xi.moghouse.lockerAccessType.ALZAHBI) == 7)
        assert(xi.moghouse.mogLockerDaysPerBronze(xi.moghouse.lockerAccessType.ALLAREAS) == 5)
    end)

    it('treats an unknown access type as all-areas', function()
        assert(xi.moghouse.mogLockerDaysPerBronze(99) == 5)
    end)
end)

describe('Mog Locker expiry state', function()
    it('reports a never-unlocked locker', function()
        assert(xi.moghouse.mogLockerExpiryState(0, 1000) == 'locked')
    end)

    it('reports an expired locker once the timestamp passes', function()
        assert(xi.moghouse.mogLockerExpiryState(500, 501) == 'expired')
        assert(xi.moghouse.mogLockerExpiryState(-1, 1000) == 'expired')
    end)

    it('reports an active locker up to and including its timestamp', function()
        assert(xi.moghouse.mogLockerExpiryState(500, 500) == 'active')
        assert(xi.moghouse.mogLockerExpiryState(500, 499) == 'active')
    end)
end)

describe('Mog Locker added expiry', function()
    it('adds whole days per bronze coin', function()
        local day = 60 * 60 * 24
        assert(xi.moghouse.mogLockerAddedExpiry(0, 1, 5) == day * 5)
        assert(xi.moghouse.mogLockerAddedExpiry(0, 2, 7) == day * 14)
        assert(xi.moghouse.mogLockerAddedExpiry(100, 1, 5) == 100 + day * 5)
    end)

    it('adds nothing for no coins', function()
        assert(xi.moghouse.mogLockerAddedExpiry(100, 0, 5) == 100)
    end)
end)
