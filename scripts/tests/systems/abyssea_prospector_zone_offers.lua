require('scripts/globals/abyssea')

describe('Abyssea starter-zone Cruor Prospector key-item offers', function()
    local cases = {
        { 'La Theine', 'scripts/zones/Abyssea-La_Theine/npcs/Cruor_Prospector', xi.ki.MAP_OF_ABYSSEA_LA_THEINE },
        { 'Konschtat', 'scripts/zones/Abyssea-Konschtat/npcs/Cruor_Prospector', xi.ki.MAP_OF_ABYSSEA_KONSCHTAT },
        { 'Tahrongi', 'scripts/zones/Abyssea-Tahrongi/npcs/Cruor_Prospector', xi.ki.MAP_OF_ABYSSEA_TAHRONGI },
    }

    for _, entry in ipairs(cases) do
        it('offers its map key item in ' .. entry[1], function()
            local granted = nil
            local removed = nil
            local player = {
                getCurrency = function() return 3500 end,
                delCurrency = function(_, currency, amount) removed = { currency, amount } end,
            }
            local oldGiveKeyItem = npcUtil.giveKeyItem
            npcUtil.giveKeyItem = function(_, keyItem)
                granted = keyItem
                return true
            end

            local npc = require(entry[2])
            npc.onEventFinish(player, 0, xi.abyssea.itemType.KEYITEM + 65536)

            npcUtil.giveKeyItem = oldGiveKeyItem
            assert(granted == entry[3])
            assert(removed[1] == 'cruor' and removed[2] == 3500)
        end)
    end
end)
