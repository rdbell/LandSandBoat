describe('Base entity position and orientation helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('reads position and rotates entities toward numeric and table points', function()
        local mob = player.entities:moveTo('Wild_Rabbit')
        assert(mob, 'Wild Rabbit mob was not found')

        mob:setPos(0, 0, 0, 32)
        local pos = mob:getPos()
        assert(pos.x == 0, 'getPos x did not match setPos')
        assert(pos.y == 0, 'getPos y did not match setPos')
        assert(pos.z == 0, 'getPos z did not match setPos')
        assert(pos.rot == 32, 'getPos rot did not match setPos')
        assert(mob:getXPos() == 0, 'getXPos did not match setPos')
        assert(mob:getYPos() == 0, 'getYPos did not match setPos')
        assert(mob:getZPos() == 0, 'getZPos did not match setPos')
        assert(mob:getRotPos() == 32, 'getRotPos did not match setPos')

        mob:setRotation(99)
        assert(mob:getRotPos() == 99, 'setRotation did not update rotation')

        mob:lookAt(5, 0, 0)
        assert(mob:getRotPos() == 0, 'lookAt numeric east did not face east')

        mob:lookAt(0, 0, -5)
        assert(mob:getRotPos() == 64, 'lookAt numeric south did not face south')

        mob:lookAt({ x = 0, y = 0, z = 5 })
        assert(mob:getRotPos() == 192, 'lookAt table north did not face north')

        mob:setRotation(77)
        mob:lookAt({ x = 0.05, y = 99, z = 0.05 })
        assert(mob:getRotPos() == 77, 'lookAt close flat point did not preserve rotation')

        assert(not pcall(function()
            mob:setRotation()
        end), 'setRotation accepted missing rotation')

        assert(not pcall(function()
            mob:lookAt()
        end), 'lookAt accepted missing point')

        assert(not pcall(function()
            mob:lookAt(1)
        end), 'lookAt accepted incomplete numeric point')

        assert(not pcall(function()
            mob:lookAt({ x = 'bad', y = 0, z = 0 })
        end), 'lookAt accepted invalid table point')
    end)
end)
