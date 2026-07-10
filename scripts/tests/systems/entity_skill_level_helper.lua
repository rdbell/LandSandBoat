describe('Base entity skill level helper bindings', function()
    it('reads and updates bounded PC skill levels with receiver fallbacks', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        local npc = player.entities:get('Field_Manual')
        assert(mob, 'Wild Rabbit mob was not found')
        assert(npc, 'Field Manual NPC was not found')

        player:setSkillLevel(xi.skill.SUMMONING_MAGIC, 200)
        local updatedSkill = player:getSkillLevel(xi.skill.SUMMONING_MAGIC)
        assert(updatedSkill == 20, string.format('setSkillLevel working skill was %u, expected 20', updatedSkill))
        assert(player:getSkillLevel(xi.skill.DIG) == 0x7FFF, 'setSkillLevel should rebuild untouched utility skills')

        player:setSkillLevel(xi.skill.WOODWORKING, 200)
        assert(player:getSkillLevel(xi.skill.WOODWORKING) == 640, 'craft skill should retain packed working representation')
        player:setSkillLevel(xi.skill.DIG, 200)
        assert(player:getSkillLevel(xi.skill.DIG) == 0x7FFF, 'utility skill should use the working sentinel value')
        assert(player:getSkillLevel(64) == 0, 'getSkillLevel should return zero for an out-of-range skill')

        local mobSkill = mob:getSkillLevel(xi.skill.SUMMONING_MAGIC)
        mob:setSkillLevel(xi.skill.SUMMONING_MAGIC, 200)
        assert(mob:getSkillLevel(xi.skill.SUMMONING_MAGIC) == mobSkill, 'non-PC setSkillLevel should be ignored')

        assert(npc:getSkillLevel(xi.skill.SUMMONING_MAGIC) == 0, 'NPC getSkillLevel should fall back to zero')
        npc:setSkillLevel(xi.skill.SUMMONING_MAGIC, 200)
        assert(npc:getSkillLevel(xi.skill.SUMMONING_MAGIC) == 0, 'NPC setSkillLevel should be ignored')

        player:setSkillLevel(64, 200)
        assert(player:getSkillLevel(64) == 0, 'out-of-range setSkillLevel should be ignored')

        player:addMod(13 + 79, 5)
        player:setSkillLevel(13, 200)
        assert(player:getSkillLevel(13) == 0, 'reserved skills should ignore positive modifiers')
        player:delMod(13 + 79, 5)
        player:addMod(13 + 79, -5)
        player:setSkillLevel(13, 200)
        assert(player:getSkillLevel(13) == 0, 'reserved skills should ignore negative modifiers')
        player:delMod(13 + 79, -5)

        player:addMod(xi.mod.SUMMONING, 0x7FFF)
        player:setSkillLevel(xi.skill.SUMMONING_MAGIC, 0xFFFF)
        assert(player:getSkillLevel(xi.skill.SUMMONING_MAGIC) == 0, 'combat skill adjustment should preserve int16 narrowing')
        player:delMod(xi.mod.SUMMONING, 0x7FFF)

        assert(not pcall(mob.getSkillLevel), 'getSkillLevel accepted missing self')
        assert(not pcall(mob.getSkillLevel, mob), 'getSkillLevel accepted missing skill ID')
        assert(not pcall(mob.getSkillLevel, mob, 'bad'), 'getSkillLevel accepted non-numeric skill ID')
        assert(not pcall(player.setSkillLevel), 'setSkillLevel accepted missing self')
        assert(not pcall(player.setSkillLevel, player), 'setSkillLevel accepted missing skill ID')
        assert(not pcall(player.setSkillLevel, player, xi.skill.SUMMONING_MAGIC), 'setSkillLevel accepted missing value')
        assert(not pcall(player.setSkillLevel, player, 'bad', 200), 'setSkillLevel accepted non-numeric skill ID')
        assert(not pcall(player.setSkillLevel, player, xi.skill.SUMMONING_MAGIC, 'bad'), 'setSkillLevel accepted non-numeric value')
    end)
end)
