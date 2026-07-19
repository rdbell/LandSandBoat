require('scripts/globals/ancestry_moogle')

describe('Ancestry Moogle trigger', function()
    it('starts the race-change event for an eligible player', function()
        local event = nil
        local player = {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            getCharVar = function(_, name)
                return name == '[RaceChange]Eligible' and 9999999999 or 0
            end,
            getRace = function() return xi.race.HUME_F end,
            getFace = function() return 5 end,
            getSize = function() return 2 end,
            startEvent = function(_, ...) event = { ... } end,
        }
        local npc = {
            getID = function() return 100 end,
        }

        xi.ancestryMoogle.onTrigger(player, npc)

        assert(event[1] == 479)
        assert(event[2] == xi.race.HUME_F and event[3] == 2 and event[4] == 2 and event[5] == 1)
        assert(event[6] > 0 and event[7] == 0 and event[8] == 0 and event[9] == 0)
    end)

    it('shows the cooldown event before checking service eligibility', function()
        local event = nil
        local player = {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            getCharVar = function(_, name)
                return name == '[RaceChange]Last' and 9999999999 or 9999999999
            end,
            startEvent = function(_, ...) event = { ... } end,
        }

        xi.ancestryMoogle.onTrigger(player, {})

        assert(event[1] == 480 and event[2] == 236)
        assert(event[7] == 5 and event[8] == 1)
    end)

    it('clears an expired service window after its not-eligible event', function()
        local event = nil
        local eligible = nil
        local player = {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            getCharVar = function() return 0 end,
            setCharVar = function(_, name, value) eligible = { name, value } end,
            startEvent = function(_, ...) event = { ... } end,
        }

        xi.ancestryMoogle.onTrigger(player, {})

        assert(event[1] == 480 and event[2] == 236)
        assert(eligible[1] == '[RaceChange]Eligible' and eligible[2] == 0)
    end)
end)

describe('Ancestry Moogle finish', function()
    it('consumes eligibility and applies a valid appearance change', function()
        local vars = {}
        local changed = nil
        local player = {
            getZoneID = function() return xi.zone.PORT_BASTOK end,
            getCharVar = function(_, name) return name == '[RaceChange]Eligible' and 9999999999 or 0 end,
            setCharVar = function(_, name, value) vars[name] = value end,
            getName = function() return 'Tester' end,
            getRace = function() return xi.race.HUME_M end,
            getFace = function() return 0 end,
            getSize = function() return 0 end,
            raceChange = function(_, race, face, size) changed = { race, face, size }; return true end,
        }

        xi.ancestryMoogle.onEventFinish(player, 479, 0x00025200, {})

        assert(vars['[RaceChange]Eligible'] == 0 and vars['[RaceChange]Last'] > 0)
        assert(changed[1] == xi.race.HUME_F and changed[2] == 5 and changed[3] == 2)
    end)
end)
