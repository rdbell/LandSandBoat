require('scripts/globals/chocobo_riding_game')

describe('Chocobo Riding Game finish', function()
    local function player(clearTime, startingCity, destCity)
        local values = {
            ['[ChocoGame]ClearTime'] = clearTime,
            ['[ChocoGame]StartingCity'] = startingCity,
            ['[ChocoGame]DestCity'] = destCity,
        }
        local mountedRemoved = false
        return {
            getCharVar = function(_, key) return values[key] end,
            setCharVar = function(_, key, value) values[key] = value end,
            delStatusEffectSilent = function(_, effect)
                assert(effect == xi.effect.MOUNTED)
                mountedRemoved = true
            end,
            clearTime = function() return values['[ChocoGame]ClearTime'] end,
            mountedRemoved = function() return mountedRemoved end,
        }
    end

    it('gives the route reward and clears mounted state only for its finish CSID', function()
        local originalGiveItem = npcUtil.giveItem
        local givenItem
        npcUtil.giveItem = function(_, item) givenItem = item end

        local ok, err = pcall(function()
            local p = player(929, xi.zone.WINDURST_WOODS, xi.zone.SAUROMUGUE_CHAMPAIGN)
            xi.chocoboGame.onEventFinish(p, 901)
            assert(givenItem == xi.item.WINDURST_WOODS_GLYPH)
            assert(p.mountedRemoved() and p.clearTime() == 0)

            givenItem = nil
            local mismatched = player(929, xi.zone.WINDURST_WOODS, xi.zone.SAUROMUGUE_CHAMPAIGN)
            xi.chocoboGame.onEventFinish(mismatched, 0)
            assert(givenItem == nil)
            assert(not mismatched.mountedRemoved() and mismatched.clearTime() == 929)
        end)
        npcUtil.giveItem = originalGiveItem
        assert(ok, err)
    end)
end)
