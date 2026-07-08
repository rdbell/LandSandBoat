describe('Base entity spatial query helper bindings', function()
    ---@type CClientEntityPair
    local player

    before_each(function()
        player = xi.test.world:spawnPlayer({
            zone = xi.zone.WEST_RONFAURE,
        })
    end)

    it('measures distance, world angles, and facing relationships', function()
        local east = player.entities:moveTo('Wild_Rabbit')
        local south = player.entities:moveTo('Carrion_Worm')
        local north = player.entities:moveTo('Tunnel_Worm')
        assert(east, 'Wild Rabbit mob was not found')
        assert(south, 'Carrion Worm mob was not found')
        assert(north, 'Tunnel Worm mob was not found')

        player:setPos(0, 0, 0, 0)
        east:setPos(5, 0, 0, 128)
        south:setPos(0, 0, -5, 0)
        north:setPos(0, 0, 5, 0)

        assert(player:checkDistance(east) == 5, 'checkDistance entity mismatch')
        assert(player:checkDistance({ x = 3, y = 4, z = 0 }) == 5, 'checkDistance table mismatch')
        assert(player:checkDistance(0, 0, -5) == 5, 'checkDistance numeric mismatch')
        assert(player:checkDistance('bad') == 0, 'checkDistance invalid input did not fall back to zero')

        assert(player:getWorldAngle(east) == 0, 'getWorldAngle entity east mismatch')
        assert(player:getWorldAngle(south) == 64, 'getWorldAngle entity south mismatch')
        assert(player:getWorldAngle(south, 360) == 90, 'getWorldAngle degree conversion mismatch')
        assert(player:getWorldAngle(north, 360) == 270, 'getWorldAngle degree conversion wrapped north angle')
        assert(player:getWorldAngle(0, 0, 5) == 192, 'getWorldAngle numeric north mismatch')

        assert(player:getFacingAngle(east) == 0, 'getFacingAngle east mismatch')
        assert(player:getFacingAngle(north) == -64, 'getFacingAngle north mismatch')
        assert(player:isFacing(east), 'isFacing did not accept east target')
        assert(player:isInfront(east), 'isInfront did not accept west-facing east target')
        assert(not player:isBehind(east), 'isBehind accepted west-facing east target')
        east:setRotation(0)
        assert(player:isBehind(east), 'isBehind did not accept east-facing east target')
        assert(player:isBeside(north), 'isBeside did not accept north target')
        assert(player:isToEntitysLeft(south), 'isToEntitysLeft did not accept south target')
        assert(player:isToEntitysRight(north), 'isToEntitysRight did not accept north target')

        assert(not pcall(function()
            player:getFacingAngle()
        end), 'getFacingAngle accepted missing target')

        assert(not pcall(function()
            player:isFacing()
        end), 'isFacing accepted missing target')

        assert(not pcall(function()
            player:isFacing(east, 'bad')
        end), 'isFacing accepted invalid angle')
    end)
end)
