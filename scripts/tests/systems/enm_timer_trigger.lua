describe('ENM timer trigger', function()
    it('selects default, introduction, or recurring events', function()
        local started, acknowledged, cooldown = {}, false, false
        local player = {
            hasCompletedUniqueEvent = function() return acknowledged end,
            getCharVar = function() return cooldown and 1 or 0 end,
            getCurrentMission = function() return 999 end,
            hasKeyItem = function() return true end,
            startEvent = function(_, ...) started[#started + 1] = { ... } end,
        }
        local npc = { getName = function() return 'Moritz' end }

        xi.enm.timerNpcOnTrigger(player, npc)
        cooldown = true
        xi.enm.timerNpcOnTrigger(player, npc)
        acknowledged, cooldown = true, false
        xi.enm.timerNpcOnTrigger(player, npc)

        assert(#started == 3)
        assert(started[1][1] == 10027 and started[1][2] == nil)
        assert(started[2][1] == 10028 and started[2][2] == 0)
        assert(started[3][1] == 10029 and started[3][2] == 0)
    end)
end)
