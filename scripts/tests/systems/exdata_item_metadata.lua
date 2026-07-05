describe('Exdata', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer(
            {
                zone = xi.zone.SOUTHERN_SAN_DORIA,
            })
    end)

    it('can get and set Evolith exdata', function()
        local item = player:addItem({ id = xi.item.EVOLITH, quantity = 1 })
        assert(item)

        item:setExData(
            {
                augment   = 120,
                shape     = xi.evolith.shape.DOWN_FILLED,
                element   = xi.evolith.element.FIRE,
                bonus     = 3,
                signature = player:getName(),
            })

        local ex = item:getExData()
        assert(ex.augment == 120)
        assert(ex.shape == xi.evolith.shape.DOWN_FILLED)
        assert(ex.element == xi.evolith.element.FIRE)
        assert(ex.bonus == 3)
        assert(ex.signature == player:getName())
    end)

    it('can get and set Crafting Set exdata', function()
        local item = player:addItem({ id = xi.item.WOODWORKING_SET_25, quantity = 1 })
        assert(item)

        item:setExData(
            {
                quality   = 100,
                signature = 'Test',
            })

        local ex = item:getExData()
        assert(ex.quality == 100)
        assert(ex.signature == 'Test')
    end)

    it('can get and set Glowing Lamp exdata', function()
        local item = player:addItem({ id = xi.item.GLOWING_LAMP, quantity = 1 })
        assert(item)

        local now = GetSystemTime()
        item:setExData(
            {
                chamberId = xi.einherjar.chamber.SCHWERTLEITE,
                flags     = 3,
                startTime = now,
                endTime   = now + 1800,
            })

        local ex = item:getExData()
        assert(ex.chamberId == xi.einherjar.chamber.SCHWERTLEITE)
        assert(ex.flags == 3)
        assert(ex.startTime == now)
        assert(ex.endTime == now + 1800)
    end)

    it('can get and set Chocobo Egg exdata', function()
        local item = player:addItem({ id = xi.item.CHOCOBO_EGG_FAINTLY_WARM, quantity = 1 })
        assert(item)

        item:setExData(
            {
                dna     = { 3, 5, 7 },
                ability = xi.chocoboRaising.ability.TREASURE_FINDER,
                plan    = xi.chocoboRaising.honeymoonPlan.SPORTS,
                isBred  = true,
            })

        local ex = item:getExData()
        assert(ex.dna[1] == 3)
        assert(ex.dna[2] == 5)
        assert(ex.dna[3] == 7)
        assert(ex.ability == xi.chocoboRaising.ability.TREASURE_FINDER)
        assert(ex.plan == xi.chocoboRaising.honeymoonPlan.SPORTS)
        assert(ex.isBred == true)
    end)

    it('can get and set Chocobo Card exdata', function()
        local item = player:addItem({ id = xi.item.VCS_REGISTRATION_FORM, quantity = 1 })
        assert(item)

        item:setExData(
            {
                strength    = { trait = true, rp = 8, rank = xi.chocoboRaising.statRank.IMPRESSIVE },
                endurance   = { trait = false, rp = 4, rank = xi.chocoboRaising.statRank.AVERAGE },
                discernment = { trait = true, rp = 12, rank = xi.chocoboRaising.statRank.OUTSTANDING },
                receptivity = { rp = 20, rank = xi.chocoboRaising.statRank.FIRST_CLASS },
                dna         = { 2, 4, 6 },
                abilities   = { xi.chocoboRaising.ability.GALLOP, xi.chocoboRaising.ability.CANTER },
                temperament = xi.chocoboRaising.temperament.ENIGMATIC,
                weather     = xi.chocoboRaising.weather.CLOUDY,
                gender      = xi.chocoboRaising.gender.FEMALE,
                color       = xi.chocoboRaising.color.RED,
                size        = xi.chocoboRacing.jockeySize.HUME_F,
                name        = 'ChocoTest',
            })

        local ex = item:getExData()
        assert(ex.strength.trait == true)
        assert(ex.strength.rp == 8)
        assert(ex.strength.rank == xi.chocoboRaising.statRank.IMPRESSIVE)
        assert(ex.endurance.trait == false)
        assert(ex.endurance.rp == 4)
        assert(ex.endurance.rank == xi.chocoboRaising.statRank.AVERAGE)
        assert(ex.discernment.trait == true)
        assert(ex.discernment.rp == 12)
        assert(ex.discernment.rank == xi.chocoboRaising.statRank.OUTSTANDING)
        assert(ex.receptivity.rp == 20)
        assert(ex.receptivity.rank == xi.chocoboRaising.statRank.FIRST_CLASS)
        assert(ex.dna[1] == 2)
        assert(ex.dna[2] == 4)
        assert(ex.dna[3] == 6)
        assert(ex.abilities[1] == xi.chocoboRaising.ability.GALLOP)
        assert(ex.abilities[2] == xi.chocoboRaising.ability.CANTER)
        assert(ex.temperament == xi.chocoboRaising.temperament.ENIGMATIC)
        assert(ex.weather == xi.chocoboRaising.weather.CLOUDY)
        assert(ex.gender == xi.chocoboRaising.gender.FEMALE)
        assert(ex.color == xi.chocoboRaising.color.RED)
        assert(ex.size == xi.chocoboRacing.jockeySize.HUME_F)
        assert(ex.name == 'ChocoTest')
    end)

    it('can get and set Fish exdata', function()
        local item = player:addItem({ id = xi.item.LIK, quantity = 1 })
        assert(item)

        item:setExData(
            {
                size     = 300,
                weight   = 1500,
                isRanked = true,
            })

        local ex = item:getExData()
        assert(ex.size == 300)
        assert(ex.weight == 1500)
        assert(ex.isRanked == true)
    end)

    it('can get and set Escutcheon exdata', function()
        local item = player:addItem({ id = xi.item.JOINERS_ASPIS, quantity = 1 })
        assert(item)

        item:setExData(
            {
                status             = 1,
                bonusObjective     = xi.escutcheon.bonusObjective.CRAFT_DAYTIME,
                craftsmanship      = 2500,
                stage              = xi.escutcheon.stage.ASPIS,
                successDownPenalty = 0,
                signature          = player:getName(),
            })

        local ex = item:getExData()
        assert(ex.status == 1)
        assert(ex.bonusObjective == xi.escutcheon.bonusObjective.CRAFT_DAYTIME)
        assert(ex.craftsmanship == 2500)
        assert(ex.stage == xi.escutcheon.stage.ASPIS)
        assert(ex.successDownPenalty == 0)
        assert(ex.signature == player:getName())
    end)

end)
