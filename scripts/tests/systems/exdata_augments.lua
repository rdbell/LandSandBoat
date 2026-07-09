describe('Exdata', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer(
            {
                zone = xi.zone.SOUTHERN_SAN_DORIA,
            })
    end)

    it('can get and set AugmentStandard exdata', function()
        local item = player:addItem({ id = xi.item.BRONZE_SWORD, quantity = 1 })
        assert(item)

        item:setExData(
            {
                augmentKind    = xi.augment.kind.HAS_AUGMENTS,
                augmentSubKind = xi.augment.subKind.STANDARD,
                augments       =
                {
                    { id = 1, value = 4 },
                    { id = 9, value = 2 },
                },
                signature = player:getName(),
            })

        local ex = item:getExData()
        assert(ex.augmentKind == xi.augment.kind.HAS_AUGMENTS)
        assert(ex.augmentSubKind == xi.augment.subKind.STANDARD)
        assert(ex.augments[1].id == 1)
        assert(ex.augments[1].value == 4)
        assert(ex.augments[2].id == 9)
        assert(ex.augments[2].value == 2)
        assert(ex.signature == player:getName())
        assert(item:getMod(xi.mod.HP) == 5)
        assert(item:getMod(xi.mod.MP) == 3)
    end)

    it('can get and set AugmentTrial exdata', function()
        local item = player:addItem({ id = xi.item.BRONZE_SWORD, quantity = 1 })
        assert(item)

        item:setExData(
            {
                augmentKind    = xi.augment.kind.HAS_AUGMENTS,
                augmentSubKind = xi.augment.subKind.STANDARD + xi.augment.subKind.TRIAL,
                augments       =
                {
                    { id = 1, value = 4 },
                },
                trial = { id = 100, completed = false },
            })

        local ex = item:getExData()
        assert(ex.trial.id == 100)
        assert(ex.trial.completed == false)
        assert(ex.augments[1].id == 1)
        assert(item:getMod(xi.mod.HP) == 5)
    end)

    it('can get and set AugmentBundle exdata', function()
        local item = player:addItem({ id = xi.item.BRONZE_SWORD, quantity = 1 })
        assert(item)

        item:setExData(
            {
                augmentKind   = xi.augment.kind.BUNDLED,
                type          = 2,
                rank          = 18,
                accumulatedRP = 350,
                maxRankTier   = 2,
                rpCurve       = xi.augment.rpCurve.B,
                augmentIndex  = 63,
            })

        local ex = item:getExData()
        assert(ex.augmentKind == xi.augment.kind.BUNDLED)
        assert(ex.rank == 18)
        assert(ex.accumulatedRP == 350)
        assert(ex.augmentIndex == 63)
    end)

    it('can get and set AugmentMezzotint exdata', function()
        local item = player:addItem({ id = xi.item.BRONZE_SWORD, quantity = 1 })
        assert(item)

        item:setExData(
            {
                augmentKind    = xi.augment.kind.HAS_AUGMENTS,
                augmentSubKind = xi.augment.subKind.MEZZOTINT,
                type           = xi.mezzotint.type.B,
                rank           = 10,
                accumulatedRP  = 500,
                augments       =
                {
                    { index = xi.mezzotint.augment.MP_II, value = 3 },
                    { index = xi.mezzotint.augment.ACCURACY, value = 7 },
                },
            })

        local ex = item:getExData()
        assert(ex.rank == 10)
        assert(ex.accumulatedRP == 500)
        assert(ex.augments[1].index == xi.mezzotint.augment.MP_II)
        assert(ex.augments[1].value == 3)
    end)

    it('can get and set Serialized exdata', function()
        local item = player:addItem({ id = 19320, quantity = 1 })
        assert(item)

        item:setExData(
            {
                serverIndex  = 0x7F,
                serialNumber = 500,
                signature    = player:getName(),
            })

        local ex = item:getExData()
        assert(ex.serverIndex == 0x7F)
        assert(ex.serialNumber == 500)
        assert(ex.signature == player:getName())
    end)

    it('unhandled items fall back to raw bytes', function()
        local item = player:addItem({ id = xi.item.FIRE_CRYSTAL, quantity = 1 })
        assert(item)

        item:setExDataRaw({ [0] = 0xAB, [5] = 0xCD })

        local ex = item:getExData()
        assert(ex[0] == 0xAB)
        assert(ex[5] == 0xCD)
    end)
end)
