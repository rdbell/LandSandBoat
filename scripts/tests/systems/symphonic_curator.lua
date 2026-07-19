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
