describe('AoE', function()
    ---@type CClientEntityPair
    local p1
    ---@type CClientEntityPair
    local p2

    before_each(function()
        p1 = xi.test.world:spawnPlayer(
            {
                zone  = xi.zone.GM_HOME,
                job   = xi.job.BLM,
                level = 99,
            })
        p2 = xi.test.world:spawnPlayer(
            {
                zone  = xi.zone.GM_HOME,
                job   = xi.job.BLM,
                level = 99,
            })
        p1.actions:inviteToParty(p2)
        p2.actions:acceptPartyInvite()
    end)

    describe('Utsusemi AoE #nin', function()
        before_each(function()
            p1:changeJob(xi.job.NIN)
            p1:setLevel(99)
            p1:addSpell(xi.magic.spell.UTSUSEMI_NI)
            p1:addItem(xi.item.SHIHEI, 99)
            p1:setMod(xi.mod.UTSUSEMI_AOE, 1)
        end)

        it('converts Utsusemi spells to 10y AoE with UTSUSEMI_AOE mod', function()
            p1.actions:useSpell(p1, xi.magic.spell.UTSUSEMI_NI)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(5)
            p1.assert:hasEffect(xi.effect.COPY_IMAGE)
            p2.assert:hasEffect(xi.effect.COPY_IMAGE)
        end)

        it('does not affect Utsusemi without the mod', function()
            p1:setMod(xi.mod.UTSUSEMI_AOE, 0)
            p1.actions:useSpell(p1, xi.magic.spell.UTSUSEMI_NI)
            xi.test.world:tickEntity(p1)
            xi.test.world:skipTime(5)
            p1.assert:hasEffect(xi.effect.COPY_IMAGE)
            p2.assert.no:hasEffect(xi.effect.COPY_IMAGE)
        end)
    end)

end)
