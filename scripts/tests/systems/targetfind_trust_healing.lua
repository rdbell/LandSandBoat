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

    it('Valaineral trust Majesty + Cure III heals party', function()
        local pConfig =
        {
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.WAR,
            level = 99,
        }

        local player = xi.test.world:spawnPlayer(pConfig)
        player:setGMLevel(3)
        player:setVisibleGMLevel(3)

        local stranger = xi.test.world:spawnPlayer(pConfig)

        player:addSpell(xi.magic.spell.VALAINERAL)
        player.actions:useSpell(player, xi.magic.spell.VALAINERAL)
        xi.test.world:tickEntity(player)
        xi.test.world:skipTime(10)

        local valaineral = nil
        for _, member in ipairs(player:getPartyWithTrusts()) do
            if member:getName() == 'valaineral' then
                valaineral = member
                break
            end
        end

        assert(valaineral, 'Valaineral was not summoned')

        player:setHP(1)
        stranger:setHP(1)
        stranger:setPos(player:getXPos(), player:getYPos(), player:getZPos())

        valaineral:useJobAbility(xi.ja.MAJESTY, valaineral)
        xi.test.world:tickEntity(valaineral)
        xi.test.world:skipTime(3)

        valaineral:castSpell(xi.magic.spell.CURE_III, valaineral)
        xi.test.world:tickEntity(valaineral)
        xi.test.world:skipTime(10)

        assert(player:getHP() > 1, 'Player was not healed by Valaineral Cure III')
        assert(stranger:getHP() == 1, 'Stranger was incorrectly healed by Valaineral Cure III')
    end)
end)
