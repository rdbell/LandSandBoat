describe('Base entity job-driven skill cap rebuild', function()
    it('rebuilds combat skills from main/sub job ranks and levels', function()
        local player = xi.test.world:spawnPlayer({
            zone  = xi.zone.WEST_RONFAURE,
            job   = xi.job.SMN,
            level = 99,
        })

        player:setSkillLevel(xi.skill.SUMMONING_MAGIC, 200)
        assert(player:getSkillLevel(xi.skill.SUMMONING_MAGIC) == 20,
            'setSkillLevel should convert raw summoning skill by ten')

        local summoningCap = player:getMaxSkillLevel(99, xi.job.SMN, xi.skill.SUMMONING_MAGIC)
        assert(summoningCap > 20, 'expected a non-trivial SMN summoning cap')
        player:setSkillLevel(xi.skill.SUMMONING_MAGIC, 20000)
        assert(player:getSkillLevel(xi.skill.SUMMONING_MAGIC) == summoningCap,
            string.format('job-driven summoning cap was %u, expected %u',
                player:getSkillLevel(xi.skill.SUMMONING_MAGIC), summoningCap))

        player:setLevel(1)
        local levelOneCap = player:getMaxSkillLevel(1, xi.job.SMN, xi.skill.SUMMONING_MAGIC)
        assert(player:getSkillLevel(xi.skill.SUMMONING_MAGIC) == levelOneCap,
            string.format('setLevel should rebuild summoning to level-1 cap %u, got %u',
                levelOneCap, player:getSkillLevel(xi.skill.SUMMONING_MAGIC)))

        player:setLevel(99)
        player:changeJob(xi.job.WAR)
        assert(player:getSkillLevel(xi.skill.SUMMONING_MAGIC) == 0,
            'WAR has no summoning rank, so working skill should fall back to zero')

        player:addMod(xi.mod.ENHANCE, 5)
        player:setSkillLevel(xi.skill.ENHANCING_MAGIC, 200)
        assert(player:getSkillLevel(xi.skill.ENHANCING_MAGIC) == 5,
            'unavailable main-job skills should retain non-negative skill modifiers only')
        player:delMod(xi.mod.ENHANCE, 5)

        player:changesJob(xi.job.RDM)
        player:setsLevel(50)
        -- Default SUBJOB_RATIO 1/2 clamps sub level 50 under main 99 to 49.
        local effectiveSubLevel = player:getSubLvl()
        assert(effectiveSubLevel == 49,
            string.format('expected sub level clamp 49, got %u', effectiveSubLevel))
        local subCap = player:getMaxSkillLevel(effectiveSubLevel, xi.job.RDM, xi.skill.ENHANCING_MAGIC)
        assert(subCap > 0, 'expected a non-zero RDM subjob enhancing cap')
        player:setSkillLevel(xi.skill.ENHANCING_MAGIC, 20000)
        assert(player:getSkillLevel(xi.skill.ENHANCING_MAGIC) == subCap,
            string.format('subjob enhancing cap was %u, expected %u',
                player:getSkillLevel(xi.skill.ENHANCING_MAGIC), subCap))
    end)
end)
