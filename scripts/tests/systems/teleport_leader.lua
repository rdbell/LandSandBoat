require('scripts/globals/teleports')

describe('Teleport to party leader', function()
    it('teleports to an outside leader by name', function()
        local destination
        local player =
        {
            getPartyLeader = function()
                return
                {
                    inMogHouse = function() return false end,
                    getName = function() return 'PartyLeader' end,
                }
            end,
            gotoPlayer = function(_, name) destination = name end,
        }

        xi.teleport.toLeader(player)

        assert(destination == 'PartyLeader', 'outside leader did not receive a teleport')
    end)

    it('does nothing without a party leader', function()
        local calls = 0
        local player =
        {
            getPartyLeader = function() return nil end,
            gotoPlayer = function() calls = calls + 1 end,
        }

        xi.teleport.toLeader(player)

        assert(calls == 0, 'teleported without a party leader')
    end)

    it('does not teleport to a leader in a Mog House', function()
        local calls = 0
        local player =
        {
            getPartyLeader = function()
                return
                {
                    inMogHouse = function() return true end,
                    getName = function() return 'MogHouseLeader' end,
                }
            end,
            gotoPlayer = function() calls = calls + 1 end,
        }

        xi.teleport.toLeader(player)

        assert(calls == 0, 'teleported to a leader in a Mog House')
    end)
end)
