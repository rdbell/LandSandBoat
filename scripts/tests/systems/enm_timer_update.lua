describe('ENM timer event update', function()
    it('acknowledges declines and reports active or expired cooldowns', function()
        local updates, cooldown = {}, 200
        stub('VanadielTime', function() return 100 end)
        local player = {
            getCharVar = function(_, key)
                assert(key == '[ENM]abandonmentTimer')
                return cooldown
            end,
            updateEvent = function(_, ...) updates[#updates + 1] = { ... } end,
        }

        xi.enm.timerNpcOnEventUpdate(player, 0, 0)
        xi.enm.timerNpcOnEventUpdate(player, 0, 1)
        cooldown = 100
        xi.enm.timerNpcOnEventUpdate(player, 0, 1)

        assert(#updates == 3)
        assert(updates[1][1] == 1)
        assert(updates[2][1] == 0 and updates[2][2] == 200)
        assert(updates[3][1] == 0 and updates[3][2] == nil)
    end)
end)
