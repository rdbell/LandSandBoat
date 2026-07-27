describe('ENM timer finish', function()
    it('acknowledges only a known introduction cutscene with option 0 through 100', function()
        local acknowledged = 0
        local player = {
            getEventTarget = function()
                return { getName = function() return 'Moritz' end }
            end,
            setUniqueEvent = function(_, event)
                assert(event == xi.uniqueEvent.ENM_TIMER_NPCS_INTRO)
                acknowledged = acknowledged + 1
            end,
        }

        xi.enm.timerNpcOnEventFinish(player, 10028, 0)
        xi.enm.timerNpcOnEventFinish(player, 10028, 100)
        xi.enm.timerNpcOnEventFinish(player, 10028, -1)
        xi.enm.timerNpcOnEventFinish(player, 10028, 101)
        xi.enm.timerNpcOnEventFinish(player, 10027, 0)

        assert(acknowledged == 2)
    end)
end)
