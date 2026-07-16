require('scripts/globals/abyssea/conflux_surveyor')

describe('Abyssea Conflux Surveyor', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({ zone = xi.zone.WEST_RONFAURE })
        player:addStatusEffect(xi.effect.VISITANT, { duration = 60, origin = player })
    end)

    it('uses the selected stones and caps the visitant duration', function()
        player:addKeyItem(xi.ki.TRAVERSER_STONE1)
        player:addKeyItem(xi.ki.TRAVERSER_STONE2)
        player:addKeyItem(xi.ki.RHAPSODY_IN_MAUVE)

        xi.abyssea.surveyorOnEventFinish(player, 2001, bit.lshift(2, 16) + 2)

        local effect = player:getStatusEffect(xi.effect.VISITANT)
        assert(effect:getDuration() == 7200000, 'surveyor did not cap visitant duration at two hours')
        player.assert.no:hasKI(xi.ki.TRAVERSER_STONE1)
        player.assert.no:hasKI(xi.ki.TRAVERSER_STONE2)
    end)

    it('ignores unaccepted options and unrelated cutscenes', function()
        player:addKeyItem(xi.ki.TRAVERSER_STONE1)
        local effect = player:getStatusEffect(xi.effect.VISITANT)
        local duration = effect:getDuration()

        xi.abyssea.surveyorOnEventFinish(player, 2001, bit.lshift(1, 16) + 1)
        assert(effect:getDuration() == duration, 'unaccepted option changed visitant duration')
        player.assert:hasKI(xi.ki.TRAVERSER_STONE1)

        xi.abyssea.surveyorOnEventFinish(player, 9999, bit.lshift(1, 16) + 2)
        assert(effect:getDuration() == duration, 'unrelated cutscene changed visitant duration')
        player.assert:hasKI(xi.ki.TRAVERSER_STONE1)
    end)
end)
