describe('Exdata', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer(
            {
                zone = xi.zone.SOUTHERN_SAN_DORIA,
            })
    end)

    it('can get and set Soul Plate exdata', function()
        local item = player:addItem({ id = xi.item.SOUL_PLATE, quantity = 1 })
        assert(item)

        item:setExData(
            {
                signature   = 'Goblin_Bounty_Hunter',
                zoneId      = xi.zone.QUFIM_ISLAND,
                familyId    = xi.mobFamily.GOBLIN,
                poolId      = xi.mobPool.BUGBEAR_MATMAN,
                level       = 12,
                feralSkill  = xi.pankration.feralSkill.MAIN_JOB_WARRIOR,
                feralPoints = 50,
                quality     = 42,
            })

        local ex = item:getExData()
        assert(ex.signature == 'GoblinBountyH')
        assert(ex.zoneId == xi.zone.QUFIM_ISLAND)
        assert(ex.familyId == xi.mobFamily.GOBLIN)
        assert(ex.poolId == xi.mobPool.BUGBEAR_MATMAN)
        assert(ex.level == 12)
        assert(ex.feralSkill == xi.pankration.feralSkill.MAIN_JOB_WARRIOR)
        assert(ex.feralPoints == 50)
        assert(ex.quality == 42)
    end)

    it('soul plate signature encodes and truncates correctly', function()
        local item = player:addItem({ id = xi.item.SOUL_PLATE, quantity = 1 })
        assert(item)

        -- Goblin_Bounty_Hunter -> Goblin_Bounty_H -> GoblinBountyH
        item:setExData({ signature = 'Goblin_Bounty_Hunter' })
        assert(item:getExData().signature == 'GoblinBountyH')

        -- Thunder_Elemental -> Thunder_Element -> ThunderElement
        item:setExData({ signature = 'Thunder_Elemental' })
        assert(item:getExData().signature == 'ThunderElement')

        item:setExData({ signature = 'Crab' })
        assert(item:getExData().signature == 'Crab')
    end)

    it('can get and set Soul Reflector exdata', function()
        local item = player:addItem({ id = xi.item.SOUL_REFLECTOR, quantity = 1 })
        assert(item)

        item:setExData(
            {
                nameFirst      = xi.pankration.firstName.BLOODY,
                nameLast       = xi.pankration.secondName.BEAST,
                poolId         = 200,
                exp            = 150,
                discipline     = 112,
                temperament    = 10,
                aggressiveness = 8,
                level          = 60,
                feralSkills    =
                {
                    { skillId = xi.pankration.feralSkill.MAIN_JOB_WARRIOR, level = 5 },
                    { skillId = xi.pankration.feralSkill.SUPPORT_JOB_MONK, level = 3 },
                },
            })

        local ex = item:getExData()
        assert(ex.nameFirst == xi.pankration.firstName.BLOODY)
        assert(ex.nameLast == xi.pankration.secondName.BEAST)
        assert(ex.poolId == 200)
        assert(ex.exp == 150)
        assert(ex.discipline == 112)
        assert(ex.temperament == 10)
        assert(ex.aggressiveness == 8)
        assert(ex.level == 60)
        assert(#ex.feralSkills == 7)
        assert(ex.feralSkills[1].skillId == xi.pankration.feralSkill.MAIN_JOB_WARRIOR)
        assert(ex.feralSkills[1].level == 5)
        assert(ex.feralSkills[2].skillId == xi.pankration.feralSkill.SUPPORT_JOB_MONK)
        assert(ex.feralSkills[2].level == 3)
    end)

    it('can get and set Furniture exdata', function()
        local item = player:addItem({ id = xi.item.OAK_TABLE, quantity = 1 })
        assert(item)

        item:setExData(
            {
                installed  = true,
                on2ndFloor = false,
                x          = 5,
                z          = 0,
                y          = 10,
                rotation   = 3,
                order      = 1,
            })

        local ex = item:getExData()
        assert(ex.installed == true)
        assert(ex.on2ndFloor == false)
        assert(ex.x == 5)
        assert(ex.z == 0)
        assert(ex.y == 10)
        assert(ex.rotation == 3)
        assert(ex.order == 1)
    end)

    it('can get and set FlowerPot exdata', function()
        local item = player:addItem({ id = xi.item.BRASS_FLOWERPOT, quantity = 1 })
        assert(item)

        item:setExData(
            {
                step         = 3,
                crystal1     = 1,
                crystal2     = 4,
                kind         = 2,
                examined     = true,
                strength     = 50,
                timePlanted  = 100000,
                timeNextStep = 200000,
            })

        local ex = item:getExData()
        assert(ex.step == 3)
        assert(ex.crystal1 == 1)
        assert(ex.crystal2 == 4)
        assert(ex.kind == 2)
        assert(ex.examined == true)
        assert(ex.strength == 50)
        assert(ex.timePlanted == 100000)
        assert(ex.timeNextStep == 200000)
    end)

    it('can get and set Mannequin exdata', function()
        local item = player:addItem({ id = xi.item.HUME_M_MANNEQUIN, quantity = 1 })
        assert(item)

        item:setExData(
            {
                race = xi.mannequin.type.HUME_M,
                pose = xi.mannequin.pose.HURRAY,
            })

        local ex = item:getExData()
        assert(ex.race == xi.mannequin.type.HUME_M)
        assert(ex.pose == xi.mannequin.pose.HURRAY)
    end)

end)
