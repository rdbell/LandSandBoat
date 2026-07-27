require('scripts/globals/abyssea')

describe('Abyssea warp NPC finish', function()
    local function player(cruor)
        local spent = 0
        local position
        return {
            getCurrency = function() return cruor end,
            delCurrency = function(_, _, amount) spent = spent + amount end,
            setPos = function(_, ...) position = { ... } end,
            spent = function() return spent end,
            position = function() return position end,
        }
    end

    it('spends cruor and teleports for a confirmed selection', function()
        local p = player(200)
        xi.abyssea.warpNPCOnEventFinish(p, 0, 260)
        assert(p.spent() == 200 and p.position()[1] == -562 and p.position()[5] == xi.zone.LA_THEINE_PLATEAU)
    end)

    it('does not teleport without enough cruor', function()
        local p = player(199)
        xi.abyssea.warpNPCOnEventFinish(p, 0, 260)
        assert(p.spent() == 0 and p.position() == nil)
    end)
end)
