describe('TargetFind', function()
    ---@type CClientEntityPair
    local p1, p2, p3
    local wyv1, wyv2, wyv3

    before_each(function()
        -- Spawn everyone in Dragon's Aery as DRG
        local pConfig =
        {
            zone  = xi.zone.DRAGONS_AERY,
            job   = xi.job.DRG,
            level = 1,
        }

        p1 = xi.test.world:spawnPlayer(pConfig)
        p1:setUnkillable(true)
        p1.actions:useAbility(p1, xi.jobAbility.CALL_WYVERN)
        xi.test.world:tickEntity(p1)
        wyv1 = p1:getPet()

        p2 = xi.test.world:spawnPlayer(pConfig)
        p2:setUnkillable(true)
        p2.actions:useAbility(p2, xi.jobAbility.CALL_WYVERN)
        xi.test.world:tickEntity(p2)
        wyv2 = p2:getPet()

        p3 = xi.test.world:spawnPlayer(pConfig)
        p3:setUnkillable(true)
        p3.actions:useAbility(p3, xi.jobAbility.CALL_WYVERN)
        xi.test.world:tickEntity(p3)
        wyv3 = p3:getPet()

        assert(wyv1, 'P1 did not get a Wyvern')
        assert(wyv2, 'P2 did not get a Wyvern')
        assert(wyv3, 'P3 did not get a Wyvern')

        -- P1 and P2 are partied, P3 is solo
        p1.actions:inviteToParty(p2)
        p2.actions:acceptPartyInvite()
    end)

    it('Qultada trust Corsair Roll affects master and party', function()
        local pConfig =
        {
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        }

        local player = xi.test.world:spawnPlayer(pConfig)
        player:setGMLevel(3)
        player:setVisibleGMLevel(3)
        player:spawnPet(xi.petId.TITAN)
        local titan = player:getPet()
        assert(titan, 'Titan was not summoned')

        local partyMember = xi.test.world:spawnPlayer(pConfig)
        partyMember:setPos(player:getXPos(), player:getYPos(), player:getZPos())
        player.actions:inviteToParty(partyMember)
        partyMember.actions:acceptPartyInvite()

        local stranger = xi.test.world:spawnPlayer(pConfig)
        stranger:setPos(player:getXPos(), player:getYPos(), player:getZPos())

        player:addSpell(xi.magic.spell.QULTADA)
        player.actions:useSpell(player, xi.magic.spell.QULTADA)
        xi.test.world:tickEntity(player)
        xi.test.world:skipTime(10)

        local qultada = nil
        for _, member in ipairs(player:getPartyWithTrusts()) do
            if member:getName() == 'qultada' then
                qultada = member
                break
            end
        end

        assert(qultada, 'Qultada was not summoned')

        qultada:useJobAbility(xi.ja.CORSAIRS_ROLL, qultada)
        xi.test.world:tickEntity(qultada)
        xi.test.world:skipTime(5)

        assert(qultada:hasStatusEffect(xi.effect.CORSAIRS_ROLL), 'Qultada does not have Corsair Roll')
        assert(player:hasStatusEffect(xi.effect.CORSAIRS_ROLL), 'Player does not have Corsair Roll')
        assert(partyMember:hasStatusEffect(xi.effect.CORSAIRS_ROLL), 'Party member does not have Corsair Roll')
        assert(not stranger:hasStatusEffect(xi.effect.CORSAIRS_ROLL), 'Stranger incorrectly has Corsair Roll')
        assert(not titan:hasStatusEffect(xi.effect.CORSAIRS_ROLL), 'Titan incorrectly has Corsair Roll')
    end)

    it('Player Corsair Roll affects self and party', function()
        local pConfig =
        {
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.COR,
            sjob  = xi.job.SMN,
            level = 99,
        }

        local player = xi.test.world:spawnPlayer(pConfig)
        player:addLearnedAbility(xi.ja.CORSAIRS_ROLL)
        player:spawnPet(xi.petId.CARBUNCLE)
        local carby = player:getPet()
        assert(carby, 'Carbuncle was not summoned')

        local partyMember = xi.test.world:spawnPlayer(pConfig)
        partyMember:setPos(player:getXPos(), player:getYPos(), player:getZPos())
        player.actions:inviteToParty(partyMember)
        partyMember.actions:acceptPartyInvite()

        local stranger = xi.test.world:spawnPlayer(pConfig)
        stranger:setPos(player:getXPos(), player:getYPos(), player:getZPos())

        player.actions:useAbility(player, xi.ja.CORSAIRS_ROLL)
        xi.test.world:tickEntity(player)
        xi.test.world:skipTime(5)

        assert(player:hasStatusEffect(xi.effect.CORSAIRS_ROLL), 'Player does not have Corsair Roll')
        assert(partyMember:hasStatusEffect(xi.effect.CORSAIRS_ROLL), 'Party member does not have Corsair Roll')
        assert(not stranger:hasStatusEffect(xi.effect.CORSAIRS_ROLL), 'Stranger incorrectly has Corsair Roll')
        assert(not carby:hasStatusEffect(xi.effect.CORSAIRS_ROLL), 'Carbuncle incorrectly has Corsair Roll')
    end)

end)
