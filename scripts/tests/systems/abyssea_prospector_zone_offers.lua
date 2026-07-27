require('scripts/globals/abyssea')

describe('Abyssea Cruor Prospector key-item offers', function()
    local cases = {
        { 'La Theine', 'scripts/zones/Abyssea-La_Theine/npcs/Cruor_Prospector', xi.ki.MAP_OF_ABYSSEA_LA_THEINE, 3500 },
        { 'Konschtat', 'scripts/zones/Abyssea-Konschtat/npcs/Cruor_Prospector', xi.ki.MAP_OF_ABYSSEA_KONSCHTAT, 3500 },
        { 'Tahrongi', 'scripts/zones/Abyssea-Tahrongi/npcs/Cruor_Prospector', xi.ki.MAP_OF_ABYSSEA_TAHRONGI, 3500 },
        { 'Altepa', 'scripts/zones/Abyssea-Altepa/npcs/Cruor_Prospector', xi.ki.MAP_OF_ABYSSEA_ALTEPA, 4500 },
        { 'Grauberg', 'scripts/zones/Abyssea-Grauberg/npcs/Cruor_Prospector', xi.ki.MAP_OF_ABYSSEA_GRAUBERG, 4500 },
        { 'Uleguerand', 'scripts/zones/Abyssea-Uleguerand/npcs/Cruor_Prospector', xi.ki.MAP_OF_ABYSSEA_ULEGUERAND, 4500 },
    }

    for _, entry in ipairs(cases) do
        it('offers its map key item in ' .. entry[1], function()
            local granted = nil
            local removed = nil
            local player = {
                getCurrency = function() return entry[4] end,
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
            assert(removed[1] == 'cruor' and removed[2] == entry[4])
        end)
    end
end)
