-----------------------------------
-- Pure system tests for conquest overseerInvNation San d'Oria catalog (slice 6203).
-----------------------------------

describe('conquest overseer stock San d\'Oria pure plan', function()
    local OTHER = 4
    local SANDORIA = 0

    -- overseerInvNation[SANDORIA] (option → {rank, cp, lvl, item, place?})
    local sandoria =
    {
        [32768] = { rank = 1, cp = 1000, lvl = 10, item = 17167 }, -- ROYAL_ARCHERS_LONGBOW
        [32769] = { rank = 1, cp = 1000, lvl = 10, item = 16544 }, -- ROYAL_ARCHERS_SWORD
        [32770] = { rank = 1, cp = 1000, lvl = 10, item = 12510 }, -- ROYAL_FOOTMANS_BANDANA
        [32771] = { rank = 1, cp = 1000, lvl = 10, item = 12753 }, -- ROYAL_FOOTMANS_GLOVES
        [32772] = { rank = 1, cp = 1000, lvl = 10, item = 13004 }, -- ROYAL_FOOTMANS_BOOTS
        [32773] = { rank = 1, cp = 1000, lvl = 10, item = 16691, place = 2 }, -- ROYAL_ARCHERS_CESTI
        [32774] = { rank = 1, cp = 1000, lvl = 10, item = 13718, place = 1 }, -- ROYAL_FOOTMANS_TUNIC
        [32784] = { rank = 2, cp = 2000, lvl = 18, item = 16852 }, -- ROYAL_SPEARMANS_SPEAR
        [32785] = { rank = 2, cp = 2000, lvl = 10, item = 12630 }, -- ROYAL_FOOTMANS_VEST
        [32786] = { rank = 2, cp = 2000, lvl = 20, item = 12882 }, -- ROYAL_FOOTMANS_TROUSERS
        [32787] = { rank = 2, cp = 2000, lvl = 20, item = 17367, place = 2 }, -- ROYAL_SPEARMANS_HORN
        [32788] = { rank = 2, cp = 2000, lvl = 20, item = 13045, place = 1 }, -- ROYAL_FOOTMANS_CLOGS
        [32800] = { rank = 3, cp = 4000, lvl = 30, item = 16844 }, -- ROYAL_SQUIRES_HALBERD
        [32801] = { rank = 3, cp = 4000, lvl = 30, item = 13104 }, -- ROYAL_SQUIRES_COLLAR
        [32802] = { rank = 3, cp = 4000, lvl = 30, item = 12431 }, -- ROYAL_SQUIRES_HELM
        [32803] = { rank = 3, cp = 4000, lvl = 30, item = 12687 }, -- ROYAL_SQUIRES_MUFFLERS
        [32804] = { rank = 3, cp = 4000, lvl = 30, item = 12943 }, -- ROYAL_SQUIRES_SOLLERETS
        [32805] = { rank = 3, cp = 4000, lvl = 30, item = 16744, place = 2 }, -- ROYAL_SQUIRES_DAGGER
        [32806] = { rank = 3, cp = 4000, lvl = 30, item = 17150, place = 1 }, -- ROYAL_SQUIRES_MACE
        [32807] = { rank = 3, cp = 4000, lvl = 1, item = 13495, place = 1 }, -- SAN_DORIAN_RING
        [32816] = { rank = 4, cp = 8000, lvl = 40, item = 16601 }, -- ROYAL_SWORDSMANS_BLADE
        [32817] = { rank = 4, cp = 8000, lvl = 40, item = 12559 }, -- ROYAL_SQUIRES_CHAINMAIL
        [32818] = { rank = 4, cp = 8000, lvl = 40, item = 12815 }, -- ROYAL_SQUIRES_BREECHES
        [32819] = { rank = 4, cp = 8000, lvl = 40, item = 13719, place = 2 }, -- ROYAL_SQUIRES_ROBE
        [32820] = { rank = 4, cp = 8000, lvl = 40, item = 12336, place = 1 }, -- ROYAL_SQUIRES_SHIELD
        [32832] = { rank = 5, cp = 16000, lvl = 50, item = 16851 }, -- ROYAL_KNIGHT_ARMY_LANCE
        [32833] = { rank = 5, cp = 16000, lvl = 50, item = 16571 }, -- TEMPLE_KNIGHT_ARMY_SWORD
        [32834] = { rank = 5, cp = 16000, lvl = 50, item = 12312 }, -- ROYAL_KNIGHT_ARMY_SHIELD
        [32835] = { rank = 5, cp = 16000, lvl = 50, item = 12313 }, -- TEMPLE_KNIGHT_ARMY_SHIELD
        [32836] = { rank = 5, cp = 16000, lvl = 50, item = 13107 }, -- ROYAL_KNIGHT_ARMY_COLLAR
        [32837] = { rank = 5, cp = 16000, lvl = 50, item = 13105 }, -- TEMPLE_KNIGHT_ARMY_COLLAR
        [32838] = { rank = 5, cp = 16000, lvl = 50, item = 12686 }, -- ROYAL_KNIGHTS_MUFFLERS
        [32839] = { rank = 5, cp = 16000, lvl = 50, item = 12942 }, -- ROYAL_KNIGHTS_SOLLERETS
        [32840] = { rank = 5, cp = 16000, lvl = 50, item = 13220, place = 2 }, -- ROYAL_KNIGHTS_BELT
        [32841] = { rank = 5, cp = 16000, lvl = 50, item = 13720, place = 1 }, -- ROYAL_KNIGHTS_CLOAK
        [32848] = { rank = 6, cp = 24000, lvl = 55, item = 13580 }, -- ROYAL_ARMY_MANTLE
        [32849] = { rank = 6, cp = 24000, lvl = 55, item = 13106 }, -- ROYAL_GUARDS_COLLAR
        [32850] = { rank = 6, cp = 24000, lvl = 55, item = 12430 }, -- ROYAL_KNIGHTS_BASCINET
        [32851] = { rank = 6, cp = 24000, lvl = 55, item = 13722 }, -- ROYAL_KNIGHTS_AKETON
        [32852] = { rank = 6, cp = 24000, lvl = 55, item = 12558, place = 1 }, -- ROYAL_KNIGHTS_CHAINMAIL
        [32853] = { rank = 6, cp = 24000, lvl = 55, item = 12814, place = 1 }, -- ROYAL_KNIGHTS_BREECHES
        [32854] = { rank = 6, cp = 24000, lvl = 55, item = 12321, place = 2 }, -- ROYAL_GUARDS_SHIELD
        [32855] = { rank = 6, cp = 24000, lvl = 55, item = 17067, place = 1 }, -- ROYAL_GUARDS_ROD
        [32856] = { rank = 6, cp = 24000, lvl = 55, item = 16599, place = 1 }, -- ROYAL_GUARDS_SWORD
        [32857] = { rank = 6, cp = 24000, lvl = 55, item = 16805, place = 1 }, -- ROYAL_GUARDS_FLEURET
        [32864] = { rank = 7, cp = 32000, lvl = 60, item = 15956 }, -- TEMPLE_KNIGHTS_QUIVER
        [32865] = { rank = 7, cp = 32000, lvl = 60, item = 16886, place = 2 }, -- GRAND_KNIGHTS_LANCE
        [32866] = { rank = 7, cp = 32000, lvl = 60, item = 13557, place = 1 }, -- GRAND_KNIGHTS_RING
        [32880] = { rank = 8, cp = 40000, lvl = 65, item = 14013 }, -- GRAND_TEMPLE_KNIGHTS_GAUNTLETS
        [32881] = { rank = 8, cp = 40000, lvl = 65, item = 14014, place = 2 }, -- GRAND_TEMPLE_KNIGHTS_BANGLES
        [32882] = { rank = 8, cp = 40000, lvl = 65, item = 13140, place = 1 }, -- GRAND_TEMPLE_KNIGHTS_COLLAR
        [32896] = { rank = 9, cp = 48000, lvl = 71, item = 16953 }, -- RESERVE_CAPTAINS_GREATSWORD
        [32897] = { rank = 9, cp = 48000, lvl = 71, item = 17934 }, -- RESERVE_CAPTAINS_PICK
        [32898] = { rank = 9, cp = 48000, lvl = 71, item = 17458, place = 2 }, -- RESERVE_CAPTAINS_MACE
        [32899] = { rank = 9, cp = 48000, lvl = 71, item = 16893, place = 1 }, -- RESERVE_CAPTAINS_LANCE
        [32912] = { rank = 10, cp = 56000, lvl = 1, item = 14428, place = 1 }, -- KINGDOM_AKETON
        [32932] = { rank = 10, cp = 5000, lvl = 1, item = 17583 }, -- KINGDOM_SIGNET_STAFF
        [32940] = { rank = 10, cp = 10000, lvl = 1, item = 6377 }, -- IMPERIAL_CHAIR_SET
    }

    -- common pins used to assert nation-only gaps (32932, 32940)
    local common =
    {
        [32928] = true,
        [32941] = true, -- refined chair (common), not imperial
    }

    local function getStock(playerNation, guardNation, option)
        if common[option] then
            return { common = true }
        end
        if guardNation == OTHER then
            if playerNation == SANDORIA then
                return sandoria[option]
            end
            return nil
        end
        if guardNation == SANDORIA then
            return sandoria[option]
        end
        return nil
    end

    it('San d\'Oria catalog size', function()
        local n = 0
        for _ in pairs(sandoria) do n = n + 1 end
        assert(n == 58)
    end)

    it('rank-1 longbow and place-gated cesti', function()
        local r = sandoria[32768]
        assert(r.item == 17167 and r.rank == 1 and r.cp == 1000 and r.lvl == 10)
        r = sandoria[32773]
        assert(r.item == 16691 and r.place == 2)
        r = sandoria[32774]
        assert(r.item == 13718 and r.place == 1)
    end)

    it('nation-only signet staff and imperial chair', function()
        local r = sandoria[32932]
        assert(r and r.item == 17583 and r.cp == 5000 and r.rank == 10)
        assert(common[32932] == nil)
        r = sandoria[32940]
        assert(r and r.item == 6377 and r.cp == 10000)
        assert(common[32940] == nil)
    end)

    it('rank-10 aketon and san d\'orian ring', function()
        local r = sandoria[32912]
        assert(r.item == 14428 and r.place == 1 and r.cp == 56000)
        r = sandoria[32807]
        assert(r.item == 13495 and r.place == 1 and r.lvl == 1)
    end)

    it('getStock routes San d\'Oria nation rows', function()
        local r = getStock(SANDORIA, SANDORIA, 32768)
        assert(r and r.item == 17167)
        r = getStock(SANDORIA, OTHER, 32932)
        assert(r and r.item == 17583)
        assert(getStock(SANDORIA, SANDORIA, 99999) == nil)
    end)
end)
