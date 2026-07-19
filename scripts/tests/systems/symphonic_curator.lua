require('scripts/globals/symphonic_curator')

describe('Symphonic Curator finish', function()
    it('restores the stored song on reset and persists a confirmed selection', function()
        local changed, stored = nil, nil
        local player = {
            getLocalVar = function() return 108 end,
            setLocalVar = function(_, _, song) stored = song end,
            changeMusic = function(_, musicType, song) changed = { musicType, song } end,
        }

        xi.symphonic_curator.onEventFinish(player, 30034, 0)
        assert(changed[1] == 6 and changed[2] == 108 and stored == nil)

        xi.symphonic_curator.onEventFinish(player, 30034, 18)
        assert(stored == 108 and changed[1] == 6 and changed[2] == 108)
    end)
end)

describe('Symphonic Curator trigger', function()
    it('seeds the default song and packs the base menu', function()
        local event, stored = nil, nil
        local player = {
            getLocalVar = function() return 0 end,
            setLocalVar = function(_, _, value) stored = value end,
            hasKeyItem = function() return false end,
            findItem = function() return nil end,
            getVisibleGMLevel = function() return 0 end,
            startEvent = function(_, ...) event = { ... } end,
        }

        xi.symphonic_curator.onTrigger(player)

        assert(stored == 126)
        assert(event[1] == 30034 and event[2] == 0 and event[3] == 0xFFFF)
        assert(event[4] == 1 and event[5] == 0x0F)
    end)
end)

describe('Symphonic Curator update', function()
    it('previews the selected song without changing the stored selection', function()
        local changed = nil
        local player = {
            changeMusic = function(_, musicType, song) changed = { musicType, song } end,
            setLocalVar = function() error('preview must not change the stored selection') end,
        }

        xi.symphonic_curator.onEventUpdate(player, 30034, 18)

        assert(changed[1] == 6 and changed[2] == 108)
    end)
end)
