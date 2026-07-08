describe('Base entity quest helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player quest state while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')
        local log = xi.questLog.SANDORIA

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getQuestStatus(log, 42) == xi.questStatus.QUEST_AVAILABLE, 'Initial quest status was not available')
        assert(player:hasCompletedQuest(log, 42) == false, 'Quest unexpectedly started completed')

        player:addQuest(log, 42)
        assert(player:getQuestStatus(log, 42) == xi.questStatus.QUEST_ACCEPTED, 'addQuest did not mark quest accepted')
        assert(player:hasCompletedQuest(log, 42) == false, 'addQuest unexpectedly marked quest completed')

        player:addQuest(log, 42)
        assert(player:getQuestStatus(log, 42) == xi.questStatus.QUEST_ACCEPTED, 'Repeated addQuest changed accepted quest status')

        player:completeQuest(log, 42)
        assert(player:getQuestStatus(log, 42) == xi.questStatus.QUEST_COMPLETED, 'completeQuest did not mark quest completed')
        assert(player:hasCompletedQuest(log, 42) == true, 'completeQuest did not set completed bit')

        player:addQuest(log, 42)
        assert(player:getQuestStatus(log, 42) == xi.questStatus.QUEST_COMPLETED, 'Completed quest did not take precedence over current bit')
        player:delCurrentQuest(log, 42)
        assert(player:getQuestStatus(log, 42) == xi.questStatus.QUEST_COMPLETED, 'delCurrentQuest removed completed quest state')
        assert(player:hasCompletedQuest(log, 42) == true, 'delCurrentQuest cleared completed bit')

        player:delQuest(log, 42)
        assert(player:getQuestStatus(log, 42) == xi.questStatus.QUEST_AVAILABLE, 'delQuest did not clear quest state')
        assert(player:hasCompletedQuest(log, 42) == false, 'delQuest did not clear completed bit')

        player:addQuest(log, 255)
        assert(player:getQuestStatus(log, 255) == xi.questStatus.QUEST_ACCEPTED, 'Max valid quest ID was not accepted')
        player:completeQuest(log, 255)
        assert(player:getQuestStatus(log, 255) == xi.questStatus.QUEST_COMPLETED, 'Max valid quest ID did not complete')

        player:addQuest(log, 65537)
        assert(player:getQuestStatus(log, 1) == xi.questStatus.QUEST_ACCEPTED, 'Quest ID did not wrap through uint16 input')
        player:delQuest(log, 1)

        player:addQuest(log, 256)
        player:completeQuest(log, 256)
        assert(player:getQuestStatus(log, 256) == xi.questStatus.QUEST_AVAILABLE, 'Out-of-range quest ID did not read as available')
        assert(player:hasCompletedQuest(log, 256) == false, 'Out-of-range quest ID read completed')

        player:addQuest(255, 42)
        player:completeQuest(255, 42)
        assert(player:getQuestStatus(255, 42) == xi.questStatus.QUEST_AVAILABLE, 'Invalid quest log did not read as available')
        assert(player:hasCompletedQuest(255, 42) == false, 'Invalid quest log read completed')

        npc:addQuest(log, 42)
        npc:completeQuest(log, 42)
        npc:delQuest(log, 42)
        mob:addQuest(log, 42)
        mob:completeQuest(log, 42)
        mob:delQuest(log, 42)
        assert(npc:getQuestStatus(log, 42) == xi.questStatus.QUEST_AVAILABLE, 'NPC quest status fallback changed after setters')
        assert(mob:getQuestStatus(log, 42) == xi.questStatus.QUEST_AVAILABLE, 'Mob quest status fallback changed after setters')
        assert(npc:hasCompletedQuest(log, 42) == false, 'NPC completed quest fallback changed')
        assert(mob:hasCompletedQuest(log, 42) == false, 'Mob completed quest fallback changed')
    end)
end)
