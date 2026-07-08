describe('Traverser Stones', function()
    ---@type CClientEntityPair
    local player

    local function advanceUntilStonesIncrease(from)
        for _ = 1, 64 do
            if player:getAvailableTraverserStones() > from then
                return
            end

            xi.test.world:skipToNextVanaDay()
        end

        error('Traverser Stones did not accrue in time')
    end

    before_each(function()
        player = xi.test.world:spawnPlayer({ zone = xi.zone.WEST_RONFAURE })
    end)

    it('does not accrue before the epoch is set', function()
        assert(player:getTraverserEpoch() == 0)
        assert(player:getAvailableTraverserStones() == 0)
    end)

    it('persists the epoch when set', function()
        player:setTraverserEpoch()

        assert(player:getTraverserEpoch() > 0)
    end)

    it('accrues one stone per interval', function()
        player:setTraverserEpoch()

        local baseline = player:getAvailableTraverserStones()
        advanceUntilStonesIncrease(baseline)
        assert(player:getAvailableTraverserStones() == baseline + 1)

        advanceUntilStonesIncrease(baseline + 1)
        assert(player:getAvailableTraverserStones() == baseline + 2)
    end)

    it('subtracts claimed stones from available', function()
        player:setTraverserEpoch()

        local baseline = player:getAvailableTraverserStones()
        advanceUntilStonesIncrease(baseline)

        local available = player:getAvailableTraverserStones()
        player:addClaimedTraverserStones(available)
        assert(player:getClaimedTraverserStones() == available)
        assert(player:getAvailableTraverserStones() == 0)
    end)

    it('sets claimed stones directly and ignores non-player receivers', function()
        local npc = player.entities:get('Field_Manual')

        assert(npc, 'Field Manual NPC was not found')

        player:setClaimedTraverserStones(3)
        assert(player:getClaimedTraverserStones() == 3)

        player:addClaimedTraverserStones(2)
        assert(player:getClaimedTraverserStones() == 5)

        npc:setTraverserEpoch()
        npc:addClaimedTraverserStones(9)
        npc:setClaimedTraverserStones(7)
        assert(npc:getTraverserEpoch() == 0)
        assert(npc:getAvailableTraverserStones() == 0)
        assert(npc:getClaimedTraverserStones() == 0)

        assert(not pcall(player.addClaimedTraverserStones), 'addClaimedTraverserStones accepted missing self')
        assert(not pcall(player.addClaimedTraverserStones, player), 'addClaimedTraverserStones accepted missing count')
        assert(not pcall(player.addClaimedTraverserStones, player, 'bad'), 'addClaimedTraverserStones accepted non-numeric count')
        assert(not pcall(player.setClaimedTraverserStones), 'setClaimedTraverserStones accepted missing self')
        assert(not pcall(player.setClaimedTraverserStones, player), 'setClaimedTraverserStones accepted missing total')
        assert(not pcall(player.setClaimedTraverserStones, player, 'bad'), 'setClaimedTraverserStones accepted non-numeric total')
    end)
end)
