describe('Base entity capAllSkills helper', function()
    it('caps combat skills for the current main job without touching crafts', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })
        local mob = player.entities:moveTo('Wild_Rabbit')
        assert(mob, 'Wild Rabbit mob was not found')

        player:setSkillLevel(xi.skill.SUMMONING_MAGIC, 10)
        player:setSkillLevel(xi.skill.DAGGER, 50)
        player:setSkillLevel(xi.skill.WOODWORKING, 200)
        local craftBefore = player:getCharSkillLevel(xi.skill.WOODWORKING)

        player:capAllSkills()

        local summoningCap = player:getMaxSkillLevel(99, xi.job.SMN, xi.skill.SUMMONING_MAGIC)
        local daggerCap = player:getMaxSkillLevel(99, xi.job.SMN, xi.skill.DAGGER)
        assert(player:getSkillLevel(xi.skill.SUMMONING_MAGIC) == summoningCap,
            string.format('summoning working skill was %u, expected %u',
                player:getSkillLevel(xi.skill.SUMMONING_MAGIC), summoningCap))
        assert(player:getCharSkillLevel(xi.skill.SUMMONING_MAGIC) == summoningCap * 10,
            string.format('summoning raw skill was %u, expected %u',
                player:getCharSkillLevel(xi.skill.SUMMONING_MAGIC), summoningCap * 10))
        assert(player:getSkillLevel(xi.skill.DAGGER) == daggerCap,
            string.format('dagger working skill was %u, expected %u',
                player:getSkillLevel(xi.skill.DAGGER), daggerCap))
        assert(player:getSkillLevel(xi.skill.ENHANCING_MAGIC) == 0,
            'unavailable combat skills should cap to zero working value')
        assert(player:getCharSkillLevel(xi.skill.WOODWORKING) == craftBefore,
            'capAllSkills should not rewrite craft raw skills')

        local mobSkill = mob:getSkillLevel(xi.skill.SUMMONING_MAGIC)
        mob:capAllSkills()
        assert(mob:getSkillLevel(xi.skill.SUMMONING_MAGIC) == mobSkill,
            'non-PC capAllSkills should be ignored')

        assert(not pcall(player.capAllSkills), 'capAllSkills accepted missing self')
    end)
end)
