describe('Base entity jail and misc helper bindings', function()
    ---@type CClientEntityPair
    local ronfaurePlayer

    before_each(function()
        ronfaurePlayer = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reports jail state from zone while non-players fall back', function()
        local jailedPlayer = xi.test.world:spawnPlayer({
            zone = xi.zone.MORDION_GAOL,
        })
        local gmInGaol = xi.test.world:spawnPlayer({
            zone = xi.zone.MORDION_GAOL,
        })
        local npc = ronfaurePlayer.entities:get('Field_Manual')
        local mob = ronfaurePlayer.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        gmInGaol:setGMLevel(1)
        assert(ronfaurePlayer:isJailed() == false, 'Player outside Mordion Gaol was jailed')
        assert(jailedPlayer:isJailed() == true, 'Non-GM player in Mordion Gaol was not jailed')
        assert(gmInGaol:isJailed() == false, 'GM player in Mordion Gaol was jailed')
        assert(npc:isJailed() == false, 'NPC jail fallback was not false')
        assert(mob:isJailed() == false, 'Mob jail fallback was not false')

        ronfaurePlayer:jail()
        npc:jail()
        mob:jail()
        assert(ronfaurePlayer:isJailed() == false, 'jail helper unexpectedly changed visible jail state')
        assert(npc:isJailed() == false, 'NPC jail fallback changed after setter')
        assert(mob:isJailed() == false, 'Mob jail fallback changed after setter')
    end)

    it('checks current zone misc flags', function()
        local npc = ronfaurePlayer.entities:get('Field_Manual')
        local mob = ronfaurePlayer.entities:moveTo('Wild_Rabbit')

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(ronfaurePlayer:canUseMisc(xi.zoneMisc.NONE) == true, 'Zone misc NONE did not pass')
        assert(ronfaurePlayer:canUseMisc(xi.zoneMisc.MOUNT) == true, 'West Ronfaure did not allow MOUNT')
        assert(ronfaurePlayer:canUseMisc(xi.zoneMisc.PET) == true, 'West Ronfaure did not allow PET')
        assert(ronfaurePlayer:canUseMisc(xi.zoneMisc.TRUST) == true, 'West Ronfaure did not allow TRUST')
        assert(ronfaurePlayer:canUseMisc(xi.zoneMisc.MOUNT + xi.zoneMisc.PET) == true, 'Combined zone misc flags did not pass')
        assert(ronfaurePlayer:canUseMisc(xi.zoneMisc.COSTUME) == false, 'West Ronfaure unexpectedly allowed COSTUME')
        assert(ronfaurePlayer:canUseMisc(xi.zoneMisc.MOUNT + xi.zoneMisc.COSTUME) == false, 'Missing combined zone misc flag passed')

        assert(npc:canUseMisc(xi.zoneMisc.MOUNT) == true, 'NPC in West Ronfaure did not use zone misc flags')
        assert(mob:canUseMisc(xi.zoneMisc.MOUNT) == true, 'Mob in West Ronfaure did not use zone misc flags')
    end)
end)
