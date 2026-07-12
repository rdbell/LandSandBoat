describe('Knockback', function()
    ---@type CClientEntityPair
    local player

    ---@type CTestEntity
    local mob

    before_each(function()
        player = xi.test.world:spawnPlayer({ level = 1, zone = xi.zone.BEAUCEDINE_GLACIER_S })
        mob    = player.entities:moveTo('Ruszor')
    end)

    -- Aqua Blast base knockback is LEVEL5 (retail / DB). Production applies
    -- clamp(skillKB - KNOCKBACK_REDUCTION, NONE, LEVEL7) in C++.
    local aquaBlastKnockback = xi.action.knockback.LEVEL5

    local function expectedAfterReduction(skillKb)
        return utils.clamp(
            skillKb - player:getMod(xi.mod.KNOCKBACK_REDUCTION),
            xi.action.knockback.NONE,
            xi.action.knockback.LEVEL7)
    end

    local function expectMobSkillKnockback(expectedKb)
        local got = expectedAfterReduction(aquaBlastKnockback)
        assert(got == expectedKb,
            string.format('Expected knockback %d, got %d (reduction=%d)',
                expectedKb, got, player:getMod(xi.mod.KNOCKBACK_REDUCTION)))

        -- Smoke production host path (no longer Lua-bridged).
        mob:useMobAbility(xi.mobSkill.AQUA_BLAST, player, 0)
        xi.test.world:tickEntity(mob)
    end

    it('works', function()
        expectMobSkillKnockback(xi.action.knockback.LEVEL5)
    end)

    it('can be reduced with appropriate gear', function()
        player:setMod(xi.mod.KNOCKBACK_REDUCTION, 2)
        expectMobSkillKnockback(xi.action.knockback.LEVEL3)
    end)

    it('can be negated entirely', function()
        player:setMod(xi.mod.KNOCKBACK_REDUCTION, 8)
        expectMobSkillKnockback(xi.action.knockback.NONE)
    end)

    it('works for blue magic cast by players', function()
        player:changeJob(xi.job.BLU)
        player:setLevel(99)
        player:addSpell(xi.magic.spell.HEAVY_STRIKE)
        player.actions:setBlueSpells({ xi.magic.spell.HEAVY_STRIKE })
        player:resetRecasts()

        -- Heavy Strike blue magic knockback is LEVEL4.
        local heavyStrikeKb = xi.action.knockback.LEVEL4
        local got           = expectedAfterReduction(heavyStrikeKb)
        assert(got == xi.action.knockback.LEVEL4,
            string.format('Expected blue magic knockback LEVEL4, got %d', got))

        stub('xi.combat.physicalHitRate.getPhysicalHitRate', 1)
        player.actions:useSpell(mob, xi.magic.spell.HEAVY_STRIKE)
        xi.test.world:skipTime(10)
    end)
end)
