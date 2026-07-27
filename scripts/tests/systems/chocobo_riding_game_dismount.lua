require('scripts/globals/chocobo_riding_game')

describe('Chocobo Riding Game dismount', function()
    local function player(startTime)
        local writes = {}
        return {
            getCharVar = function(_, key)
                assert(key == '[ChocoGame]StartTime')
                return startTime
            end,
            setCharVar = function(_, key, value)
                writes[#writes + 1] = { key = key, value = value }
            end,
            writes = function() return writes end,
        }
    end

    it('leaves inactive race state untouched', function()
        local p = player(0)
        xi.chocoboGame.dismountChoco(p)
        assert(#p.writes() == 0)
    end)

    it('clears every race variable when an active race dismounts', function()
        local p = player(1)
        xi.chocoboGame.dismountChoco(p)
        local writes = p.writes()
        assert(#writes == 4)
        assert(writes[1].key == '[ChocoGame]StartTime' and writes[1].value == 0)
        assert(writes[2].key == '[ChocoGame]StartingCity' and writes[2].value == 0)
        assert(writes[3].key == '[ChocoGame]DestCity' and writes[3].value == 0)
        assert(writes[4].key == '[ChocoGame]ClearTime' and writes[4].value == 0)
    end)
end)
