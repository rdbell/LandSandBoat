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

    describe('Diffusion #blu', function()
        before_each(function()
            p1:changeJob(xi.job.BLU)
            p1:setLevel(99)
        end)

        it('converts DIFFUSION type BLU spells to 10y AoE', function()
            p1:addStatusEffect(xi.effect.DIFFUSION, { power = 1, duration = 60, origin = p1 })
            local spell = GetSpell(xi.magic.spell.METALLIC_BODY)
            assert(spell, 'METALLIC_BODY spell not found')
            local result = xi.combat.magicAoE.calculateTypeAndRadius(p1, spell)
            assert(result[1] == xi.magic.aoe.RADIAL, 'Expected RADIAL type')
            assert(result[2] == 10, 'Expected radius 10')
        end)

        it('does not affect spells without DIFFUSION type', function()
            p1:addStatusEffect(xi.effect.DIFFUSION, { power = 1, duration = 60, origin = p1 })
            local spell = GetSpell(xi.magic.spell.POLLEN)
            assert(spell, 'POLLEN spell not found')
            local result = xi.combat.magicAoE.calculateTypeAndRadius(p1, spell)
            assert(result[1] ~= xi.magic.aoe.RADIAL or result[2] ~= 10,
                'Should not convert to 10y AoE')
        end)
    end)

    describe('Convergence #blu', function()
        before_each(function()
            p1:changeJob(xi.job.BLU)
            p1:setLevel(99)
        end)

        it('forces offensive BLU magic spells to single target', function()
            p1:addStatusEffect(xi.effect.CONVERGENCE, { power = 1, duration = 60, origin = p1 })
            local spell = GetSpell(xi.magic.spell.MAELSTROM)
            assert(spell, 'MAELSTROM spell not found')
            local result = xi.combat.magicAoE.calculateTypeAndRadius(p1, spell)
            assert(result[1] == xi.magic.aoe.NONE, 'Expected NONE type (single target)')
            assert(result[2] == 0, 'Expected radius 0')
        end)

        it('does not affect physical BLU spells (ELEMENT_NONE)', function()
            p1:addStatusEffect(xi.effect.CONVERGENCE, { power = 1, duration = 60, origin = p1 })
            local spell = GetSpell(xi.magic.spell.WHIRL_OF_RAGE)
            assert(spell, 'WHIRL_OF_RAGE spell not found')
            local result = xi.combat.magicAoE.calculateTypeAndRadius(p1, spell)
            -- Physical BLU spells should retain their AoE
            assert(result[1] ~= xi.magic.aoe.NONE, 'Physical AoE spells should not be forced to single target')
        end)
    end)

end)
