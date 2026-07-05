describe('Exdata', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer(
            {
                zone = xi.zone.SOUTHERN_SAN_DORIA,
            })
    end)

    it('can get and set Betting Slip exdata', function()
        local item = player:addItem({ id = xi.item.CHOCOBET_TICKET, quantity = 1 })
        assert(item)

        item:setExData(
            {
                raceId       = 12345,
                raceGrade    = xi.chocoboRacing.raceGrade.C1,
                racePairingL = 3,
                racePairingR = 5,
                quills       = 500,
            })

        local ex = item:getExData()
        assert(ex.raceId == 12345)
        assert(ex.raceGrade == xi.chocoboRacing.raceGrade.C1)
        assert(ex.racePairingL == 3)
        assert(ex.racePairingR == 5)
        assert(ex.quills == 500)
    end)

    it('can get and set Race Certificate exdata', function()
        local item = player:addItem({ id = xi.item.RACE_COMPLETION_CERTIFICATE, quantity = 1 })
        assert(item)

        item:setExData(
            {
                raceId    = 99999,
                raceGrade = xi.chocoboRacing.raceGrade.C3,
            })

        local ex = item:getExData()
        assert(ex.raceId == 99999)
        assert(ex.raceGrade == xi.chocoboRacing.raceGrade.C3)
    end)

    it('can get and set Assault Log exdata', function()
        local item = player:addItem({ id = xi.item.LEUJAOAM_OBSERVATION_LOG, quantity = 1 })
        assert(item)

        item:setExData(
            {
                flags =
                {
                    [1]  = true,
                    [2]  = false,
                    [3]  = true,
                    [4]  = false,
                    [5]  = true,
                    [6]  = false,
                    [7]  = true,
                    [8]  = false,
                    [9]  = true,
                    [10] = false,
                },
            })

        local ex = item:getExData()
        assert(ex.flags[1] == true)
        assert(ex.flags[2] == false)
        assert(ex.flags[3] == true)
        assert(ex.flags[7] == true)
        assert(ex.flags[10] == false)
    end)

    it('can get and set Brenner Book exdata', function()
        local item = player:addItem({ id = xi.item.COPY_OF_THE_BRENNER_BLUEBOOK, quantity = 1 })
        assert(item)

        item:setExData(
            {
                timeValue = 1000000,
                level     = xi.brenner.levelCap.LV50,
            })

        local ex = item:getExData()
        assert(ex.timeValue == 1000000)
        assert(ex.level == xi.brenner.levelCap.LV50)
    end)

    it('can get and set Meeble Grimoire exdata', function()
        local item = player:addItem({ id = xi.item.DILIGENCE_GRIMOIRE, quantity = 1 })
        assert(item)

        item:setExData(
            {
                clears =
                {
                    [xi.meeble.expeditionType.ADJUNCT]        = { [1] = 3, [2] = 2, [3] = 1, [4] = 0 },
                    [xi.meeble.expeditionType.ASSISTANT]      = { [1] = 1, [2] = 0, [3] = 0, [4] = 0 },
                    [xi.meeble.expeditionType.INSTRUCTOR]     = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
                    [xi.meeble.expeditionType.ASC_RESEARCHER] = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
                    [xi.meeble.expeditionType.RESEARCHER]     = { [1] = 0, [2] = 0, [3] = 0, [4] = 0 },
                },
                count  = 5,
                zone   = xi.meeble.zone.SAUROMUGUE_CHAMPAIGN,
            })

        local ex = item:getExData()
        assert(ex.clears[xi.meeble.expeditionType.ADJUNCT][1] == 3)
        assert(ex.clears[xi.meeble.expeditionType.ADJUNCT][2] == 2)
        assert(ex.clears[xi.meeble.expeditionType.ADJUNCT][3] == 1)
        assert(ex.clears[xi.meeble.expeditionType.ADJUNCT][4] == 0)
        assert(ex.clears[xi.meeble.expeditionType.ASSISTANT][1] == 1)
        assert(ex.count == 5)
        assert(ex.zone == xi.meeble.zone.SAUROMUGUE_CHAMPAIGN)
    end)

    it('can get and set Honeymoon Ticket exdata', function()
        local item = player:addItem({ id = xi.item.VCS_HONEYMOON_TICKET, quantity = 1 })
        assert(item)

        item:setExData(
            {
                plan = xi.chocoboRaising.honeymoonPlan.HIKING,
            })

        local ex = item:getExData()
        assert(ex.plan == xi.chocoboRaising.honeymoonPlan.HIKING)
    end)

    it('can get and set Lottery Ticket exdata', function()
        local item = player:addItem({ id = xi.item.BONANZA_PEARL, quantity = 1 })
        assert(item)

        item:setExData(
            {
                number = 123456,
                title  = xi.bonanza.eventId.TWENTY_FIRST_VANAVERSARY_NOMAD,
            })

        local ex = item:getExData()
        assert(ex.number == 123456)
        assert(ex.title == xi.bonanza.eventId.TWENTY_FIRST_VANAVERSARY_NOMAD)
    end)

    it('can get and set Tabula exdata', function()
        local item = player:addItem({ id = xi.item.MAZE_TABULA_M01, quantity = 1 })
        assert(item)

        item:setExData(
            {
                voucher = xi.maze.voucher.ACTUALIZATION_TEAM,
                runes   =
                {
                    { id = xi.maze.rune.AQUAN,  rotation = 2, position = 0 },
                    { id = xi.maze.rune.DRAGON, rotation = 1, position = 13 },
                },
                uses    = 10,
            })

        local ex = item:getExData()
        assert(ex.voucher == xi.maze.voucher.ACTUALIZATION_TEAM)
        assert(#ex.runes == 2)
        assert(ex.runes[1].id == xi.maze.rune.AQUAN)
        assert(ex.runes[1].rotation == 2)
        assert(ex.runes[1].position == 0)
        assert(ex.runes[2].id == xi.maze.rune.DRAGON)
        assert(ex.runes[2].rotation == 1)
        assert(ex.runes[2].position == 13)
        assert(ex.uses == 10)
    end)

end)
