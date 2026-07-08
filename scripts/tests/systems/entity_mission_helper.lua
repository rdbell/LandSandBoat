describe('Base entity mission helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads and updates player mission state while non-players fall back', function()
        local npc = player.entities:get('Field_Manual')
        local mob = player.entities:moveTo('Wild_Rabbit')
        local sandoria = xi.mission.log_id.SANDORIA
        local toau = xi.mission.log_id.TOAU
        local cop = xi.mission.log_id.COP

        assert(npc, 'Field Manual NPC was not found')
        assert(mob, 'Wild Rabbit mob was not found')

        assert(player:getCurrentMission(sandoria) == 65535, 'Initial San dOria mission was not sentinel value')
        assert(player:getCurrentMission(toau) == 0, 'Initial ToAU mission was not zero')
        assert(player:getCurrentMission(cop) == 101, 'Initial CoP mission was not Rites of Life')
        assert(player:getCurrentMission({ mission_log = toau }) == 0, 'Mission log table did not read current mission')
        assert(player:getCurrentMission() == 65535, 'Missing mission log argument did not default to San dOria')
        assert(player:getCurrentMission('bad') == 65535, 'Non-log argument did not default to San dOria')
        assert(player:getCurrentMission(255) == 0, 'Invalid mission log did not read as zero')

        player:addMission(sandoria, 5)
        assert(player:getCurrentMission(sandoria) == 5, 'addMission did not set current nation mission')
        assert(player:hasCompletedMission(sandoria, 5) == false, 'Current nation mission unexpectedly read completed')

        player:completeMission(sandoria, 4)
        assert(player:getCurrentMission(sandoria) == 5, 'completeMission changed non-current nation mission')
        assert(player:hasCompletedMission(sandoria, 4) == false, 'completeMission completed non-current nation mission')

        player:completeMission(sandoria, 5)
        assert(player:getCurrentMission(sandoria) == 65535, 'completeMission did not reset nation current mission')
        assert(player:hasCompletedMission(sandoria, 5) == true, 'completeMission did not set nation completed bit')

        player:delMission(sandoria, 5)
        assert(player:hasCompletedMission(sandoria, 5) == false, 'delMission did not clear nation completed bit')
        assert(player:getCurrentMission(sandoria) == 65535, 'delMission changed completed-only nation current mission')

        player:addMission(toau, 12)
        assert(player:getCurrentMission(toau) == 12, 'addMission did not set current expansion mission')
        player:completeMission(toau, 12)
        assert(player:getCurrentMission(toau) == 0, 'completeMission did not reset expansion current mission')
        assert(player:hasCompletedMission(toau, 12) == true, 'completeMission did not set expansion completed bit')

        player:addMission(toau, 70)
        assert(player:hasCompletedMission(toau, 64) == true, 'Mission IDs 64+ did not use current mission ordering')
        assert(player:hasCompletedMission(toau, 70) == false, 'Current mission unexpectedly read completed through ordering')
        player:delMission(toau, 70)
        assert(player:getCurrentMission(toau) == 0, 'delMission did not clear current expansion mission')
        assert(player:hasCompletedMission(toau, 64) == false, 'delMission current reset did not affect inferred completion')

        assert(player:hasCompletedMission(cop, 100) == true, 'CoP mission below current did not read completed')
        assert(player:hasCompletedMission(cop, 101) == false, 'Current CoP mission unexpectedly read completed')
        player:addMission(cop, 110)
        assert(player:hasCompletedMission(cop, 109) == true, 'CoP mission ordering did not follow current mission')
        player:completeMission(cop, 110)
        assert(player:getCurrentMission(cop) == 0, 'completeMission did not reset CoP current mission')
        assert(player:hasCompletedMission(cop, 110) == false, 'CoP completeMission unexpectedly set a completed bit')

        player:addMission(toau, 65537)
        assert(player:getCurrentMission(toau) == 1, 'Mission ID did not wrap through uint16 input')

        player:addMission(toau, 851)
        assert(player:getCurrentMission(toau) == 1, 'Out-of-range mission ID changed current mission')
        assert(player:hasCompletedMission(toau, 851) == false, 'Out-of-range mission ID read completed')

        player:addMission(255, 7)
        assert(player:getCurrentMission(255) == 0, 'Invalid mission log changed readable current mission')
        assert(player:hasCompletedMission(255, 7) == false, 'Invalid mission log read completed')

        npc:addMission(toau, 22)
        npc:completeMission(toau, 22)
        npc:delMission(toau, 22)
        mob:addMission(toau, 22)
        mob:completeMission(toau, 22)
        mob:delMission(toau, 22)
        assert(npc:getCurrentMission(toau) == 0, 'NPC current mission fallback changed after setters')
        assert(mob:getCurrentMission(toau) == 0, 'Mob current mission fallback changed after setters')
        assert(npc:hasCompletedMission(toau, 22) == false, 'NPC completed mission fallback changed')
        assert(mob:hasCompletedMission(toau, 22) == false, 'Mob completed mission fallback changed')
    end)
end)
