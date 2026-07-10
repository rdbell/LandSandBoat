describe('Base entity skill rank and raw skill helpers', function()
    it('reads and updates PC skill ranks, raw skills, and caps', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        player:setSkillLevel(xi.skill.WOODWORKING, 200)
        player:setSkillRank(xi.skill.WOODWORKING, 3)
        assert(player:getSkillRank(xi.skill.WOODWORKING) == 3, 'craft rank should persist after setSkillRank')
        assert(player:getSkillLevel(xi.skill.WOODWORKING) == 643, 'craft working skill should pack rank')
        assert(player:getCharSkillLevel(xi.skill.WOODWORKING) == 200, 'getCharSkillLevel should return raw skill')

        player:setSkillLevel(xi.skill.SUMMONING_MAGIC, 209)
        player:setSkillRank(xi.skill.SUMMONING_MAGIC, 7)
        -- Combat rebuild overwrites ranks from the job catalog.
        assert(player:getSkillLevel(xi.skill.SUMMONING_MAGIC) == 20,
            'setSkillRank should still rebuild combat working skill from raw value')
        assert(player:getCharSkillLevel(xi.skill.SUMMONING_MAGIC) == 209,
            'getCharSkillLevel should retain the raw combat skill value')
        assert(player:getSkillRank(xi.skill.SUMMONING_MAGIC) ~= 7,
            'combat setSkillRank should not stick after BuildingCharSkillsTable')

        local summoningCap = player:getMaxSkillLevel(99, xi.job.SMN, xi.skill.SUMMONING_MAGIC)
        player:capSkill(xi.skill.SUMMONING_MAGIC)
        assert(player:getSkillLevel(xi.skill.SUMMONING_MAGIC) == summoningCap,
            string.format('capSkill working skill was %u, expected %u',
                player:getSkillLevel(xi.skill.SUMMONING_MAGIC), summoningCap))
        assert(player:getCharSkillLevel(xi.skill.SUMMONING_MAGIC) == summoningCap * 10,
            string.format('capSkill raw skill was %u, expected %u',
                player:getCharSkillLevel(xi.skill.SUMMONING_MAGIC), summoningCap * 10))

        player:capSkill(xi.skill.ENHANCING_MAGIC)
        assert(player:getSkillLevel(xi.skill.ENHANCING_MAGIC) == 0,
            'capSkill on unavailable main-job skill should zero the working value')
        assert(player:getCharSkillLevel(xi.skill.ENHANCING_MAGIC) == 0,
            'capSkill on unavailable main-job skill should zero the raw value')

        assert(mob:getSkillRank(xi.skill.WOODWORKING) == 0, 'mob getSkillRank should fall back to zero')
        assert(mob:getCharSkillLevel(xi.skill.WOODWORKING) == 0, 'mob getCharSkillLevel should fall back to zero')
        assert(npc:getSkillRank(xi.skill.WOODWORKING) == 0, 'NPC getSkillRank should fall back to zero')
        assert(npc:getCharSkillLevel(xi.skill.WOODWORKING) == 0, 'NPC getCharSkillLevel should fall back to zero')

        assert(not pcall(player.getSkillRank), 'getSkillRank accepted missing self')
        assert(not pcall(player.getSkillRank, player), 'getSkillRank accepted missing skill ID')
        assert(not pcall(player.getSkillRank, player, 'bad'), 'getSkillRank accepted non-numeric skill ID')
        assert(not pcall(player.setSkillRank), 'setSkillRank accepted missing self')
        assert(not pcall(player.setSkillRank, player), 'setSkillRank accepted missing skill ID')
        assert(not pcall(player.setSkillRank, player, xi.skill.WOODWORKING), 'setSkillRank accepted missing rank')
        assert(not pcall(player.setSkillRank, player, 'bad', 3), 'setSkillRank accepted non-numeric skill ID')
        assert(not pcall(player.setSkillRank, player, xi.skill.WOODWORKING, 'bad'), 'setSkillRank accepted non-numeric rank')
        assert(not pcall(player.getCharSkillLevel), 'getCharSkillLevel accepted missing self')
        assert(not pcall(player.getCharSkillLevel, player), 'getCharSkillLevel accepted missing skill ID')
        assert(not pcall(player.getCharSkillLevel, player, 'bad'), 'getCharSkillLevel accepted non-numeric skill ID')
        assert(not pcall(player.capSkill), 'capSkill accepted missing self')
        assert(not pcall(player.capSkill, player), 'capSkill accepted missing skill ID')
        assert(not pcall(player.capSkill, player, 'bad'), 'capSkill accepted non-numeric skill ID')
    end)
end)
