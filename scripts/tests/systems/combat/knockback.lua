describe('Knockback', function()
    ---@type CClientEntityPair
    local player

    ---@type CTestEntity
    local mob

    before_each(function()
        player = xi.test.world:spawnPlayer({ level = 1, zone = xi.zone.BEAUCEDINE_GLACIER_S })
        mob    = player.entities:moveTo('Ruszor')
    end)

    local function expectMobSkillKnockback(expectedKnockback)
        local s = stub('xi.combat.knockback.calculate')
        mob:useMobAbility(xi.mobSkill.AQUA_BLAST, player, 0)
        xi.test.world:tickEntity(mob)
        s:called(1)
        assert(s.calls[1].returned == expectedKnockback,
            string.format('Expected knockback %d, got %d', expectedKnockback, s.calls[1].returned))
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
        local s = stub('xi.combat.knockback.calculate')
        player:changeJob(xi.job.BLU)
        player:setLevel(99)
        player:addSpell(xi.magic.spell.HEAVY_STRIKE)
        player.actions:setBlueSpells({ xi.magic.spell.HEAVY_STRIKE })
        player:resetRecasts()
        stub('xi.combat.physicalHitRate.getPhysicalHitRate', 1)
        player.actions:useSpell(mob, xi.magic.spell.HEAVY_STRIKE)
        xi.test.world:skipTime(10)
        s:called()
        s:returnValue(xi.action.knockback.LEVEL4)
    end)
end)
