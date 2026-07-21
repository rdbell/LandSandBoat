-----------------------------------
-- Pure system tests for conquest overseerInvNation Windurst catalog (slice 6205).
-----------------------------------

describe('conquest overseer stock Windurst pure plan', function()
    local OTHER = 4
    local WINDURST = 2

    -- overseerInvNation[WINDURST] (option → {rank, cp, lvl, item, place?})
    local windurst =
    {
        [32768] = { rank = 1, cp = 1000, lvl = 10, item = 17159 }, -- FREESWORDS_BOW
        [32769] = { rank = 1, cp = 1000, lvl = 10, item = 17028 }, -- FREESWORDS_CLUB
        [32770] = { rank = 1, cp = 1000, lvl = 10, item = 16442 }, -- FREESWORDS_BAGHNAKHS
        [32771] = { rank = 1, cp = 1000, lvl = 10, item = 12915, place = 2 }, -- FREESWORDS_SLOPS
        [32772] = { rank = 1, cp = 1000, lvl = 10, item = 17130, place = 1 }, -- FREESWORDS_STAFF
        [32784] = { rank = 2, cp = 2000, lvl = 18, item = 17103 }, -- MERCENARYS_POLE
        [32785] = { rank = 2, cp = 2000, lvl = 20, item = 12484 }, -- MERCENARYS_HACHIMAKI
        [32786] = { rank = 2, cp = 2000, lvl = 20, item = 12653 }, -- MERCENARYS_GI
        [32787] = { rank = 2, cp = 2000, lvl = 20, item = 12719 }, -- MERCENARYS_TEKKO
        [32788] = { rank = 2, cp = 2000, lvl = 20, item = 12855 }, -- MERCENARYS_SITABAKI
        [32789] = { rank = 2, cp = 2000, lvl = 20, item = 12975 }, -- MERCENARYS_KYAHAN
        [32790] = { rank = 2, cp = 2000, lvl = 20, item = 16746, place = 2 }, -- MERCENARYS_KNIFE
        [32791] = { rank = 2, cp = 2000, lvl = 20, item = 16930, place = 1 }, -- MERCENARYS_GREATSWORD
        [32800] = { rank = 3, cp = 4000, lvl = 30, item = 16776 }, -- MERCENARY_CAPTAINS_SCYTHE
        [32801] = { rank = 3, cp = 4000, lvl = 30, item = 12470 }, -- MERCENARY_CAPTAINS_HEADGEAR
        [32802] = { rank = 3, cp = 4000, lvl = 30, item = 12598 }, -- MERCENARY_CAPTAINS_DOUBLET
        [32803] = { rank = 3, cp = 4000, lvl = 30, item = 12726 }, -- MERCENARY_CAPTAINS_GLOVES
        [32804] = { rank = 3, cp = 4000, lvl = 30, item = 12854 }, -- MERCENARY_CAPTAINS_HOSE
        [32805] = { rank = 3, cp = 4000, lvl = 30, item = 12982 }, -- MERCENARY_CAPTAINS_GAITERS
        [32806] = { rank = 3, cp = 4000, lvl = 30, item = 16747, place = 2 }, -- MERCENARY_CAPTAINS_KUKRI
        [32807] = { rank = 3, cp = 4000, lvl = 30, item = 13221, place = 1 }, -- MERCENARY_CAPTAINS_BELT
        [32808] = { rank = 3, cp = 4000, lvl = 1, item = 13496, place = 1 }, -- WINDURSTIAN_RING
        [32816] = { rank = 4, cp = 8000, lvl = 40, item = 16463 }, -- COMBAT_CASTERS_DAGGER
        [32817] = { rank = 4, cp = 8000, lvl = 40, item = 17282 }, -- COMBAT_CASTERS_BOOMERANG
        [32818] = { rank = 4, cp = 8000, lvl = 10, item = 13101 }, -- GREEN_SCARF
        [32819] = { rank = 4, cp = 8000, lvl = 40, item = 12614 }, -- COMBAT_CASTERS_CLOAK
        [32820] = { rank = 4, cp = 8000, lvl = 40, item = 12743 }, -- COMBAT_CASTERS_MITTS
        [32821] = { rank = 4, cp = 8000, lvl = 40, item = 12870 }, -- COMBAT_CASTERS_SLACKS
        [32822] = { rank = 4, cp = 8000, lvl = 40, item = 12998 }, -- COMBAT_CASTERS_SHOES
        [32823] = { rank = 4, cp = 8000, lvl = 40, item = 16807, place = 2 }, -- COMBAT_CASTERS_SCIMITAR
        [32824] = { rank = 4, cp = 8000, lvl = 40, item = 16669, place = 1 }, -- COMBAT_CASTERS_AXE
        [32832] = { rank = 5, cp = 16000, lvl = 50, item = 17082, place = 2 }, -- TACTICIAN_MAGICIANS_WAND
        [32833] = { rank = 5, cp = 16000, lvl = 30, item = 13102 }, -- PAISLEY_SCARF
        [32834] = { rank = 5, cp = 16000, lvl = 50, item = 12478, place = 2 }, -- TACTICIAN_MAGICIANS_HAT
        [32835] = { rank = 5, cp = 16000, lvl = 50, item = 12606 }, -- TACTICIAN_MAGICIANS_COAT
        [32836] = { rank = 5, cp = 16000, lvl = 50, item = 12734 }, -- TACTICIAN_MAGICIANS_CUFFS
        [32837] = { rank = 5, cp = 16000, lvl = 50, item = 12862 }, -- TACTICIAN_MAGICIANS_SLOPS
        [32838] = { rank = 5, cp = 16000, lvl = 50, item = 12990 }, -- TACTICIAN_MAGICIANS_PIGACHES
        [32839] = { rank = 5, cp = 16000, lvl = 50, item = 16810 }, -- TACTICIAN_MAGICIANS_ESPADON
        [32840] = { rank = 5, cp = 16000, lvl = 50, item = 16694, place = 1 }, -- TACTICIAN_MAGICIANS_HOOKS
        [32848] = { rank = 6, cp = 24000, lvl = 55, item = 13103 }, -- CHECKERED_SCARF
        [32849] = { rank = 6, cp = 24000, lvl = 55, item = 13581 }, -- FEDERAL_ARMY_MANTLE
        [32850] = { rank = 6, cp = 24000, lvl = 55, item = 17094, place = 2 }, -- WISE_WIZARDS_STAFF
        [32851] = { rank = 6, cp = 24000, lvl = 55, item = 16808, place = 2 }, -- WISE_WIZARDS_BILBO
        [32852] = { rank = 6, cp = 24000, lvl = 55, item = 16809, place = 1 }, -- WISE_WIZARDS_ANELACE
        [32864] = { rank = 7, cp = 32000, lvl = 60, item = 15958 }, -- COMBAT_CASTERS_QUIVER
        [32865] = { rank = 7, cp = 32000, lvl = 60, item = 12363, place = 2 }, -- PATRIARCH_PROTECTORS_SHIELD
        [32866] = { rank = 7, cp = 32000, lvl = 60, item = 13559, place = 1 }, -- PATRIARCH_PROTECTORS_RING
        [32880] = { rank = 8, cp = 40000, lvl = 65, item = 14016 }, -- MASTER_CASTERS_MITTS
        [32881] = { rank = 8, cp = 40000, lvl = 65, item = 14017, place = 2 }, -- MASTER_CASTERS_BRACELETS
        [32882] = { rank = 8, cp = 40000, lvl = 65, item = 13142, place = 1 }, -- WINDURSTIAN_SCARF
        [32896] = { rank = 9, cp = 48000, lvl = 71, item = 18145 }, -- MASTER_CASTERS_BOW
        [32897] = { rank = 9, cp = 48000, lvl = 71, item = 17530 }, -- MASTER_CASTERS_POLE
        [32898] = { rank = 9, cp = 48000, lvl = 71, item = 17508, place = 2 }, -- MASTER_CASTERS_BAGHNAKHS
        [32899] = { rank = 9, cp = 48000, lvl = 71, item = 17617, place = 1 }, -- MASTER_CASTERS_KNIFE
        [32912] = { rank = 10, cp = 56000, lvl = 1, item = 14430, place = 1 }, -- FEDERATION_AKETON
        [32932] = { rank = 10, cp = 5000, lvl = 1, item = 17585 }, -- FEDERATION_SIGNET_STAFF
        [32940] = { rank = 10, cp = 10000, lvl = 1, item = 6379 }, -- ORNATE_STOOL_SET
    }

    local function getStock(playerNation, guardNation, option)
        if guardNation == OTHER then
            if playerNation == WINDURST then
                return windurst[option]
            end
            return nil
        end
        if guardNation == WINDURST then
            return windurst[option]
        end
        return nil
    end

    it('Windurst catalog size', function()
        local n = 0
        for _ in pairs(windurst) do n = n + 1 end
        assert(n == 58)
    end)

    it('rank-1 bow and place-gated slops/staff', function()
        local r = windurst[32768]
        assert(r.item == 17159 and r.rank == 1 and r.cp == 1000 and r.lvl == 10)
        r = windurst[32771]
        assert(r.item == 12915 and r.place == 2)
        r = windurst[32772]
        assert(r.item == 17130 and r.place == 1)
    end)

    it('nation-only signet staff and ornate stool', function()
        local r = windurst[32932]
        assert(r and r.item == 17585 and r.cp == 5000 and r.rank == 10)
        r = windurst[32940]
        assert(r and r.item == 6379 and r.cp == 10000)
    end)

    it('rank-10 aketon and windurstian ring', function()
        local r = windurst[32912]
        assert(r.item == 14430 and r.place == 1 and r.cp == 56000)
        r = windurst[32808]
        assert(r.item == 13496 and r.place == 1 and r.lvl == 1)
    end)

    it('getStock routes Windurst nation rows', function()
        local r = getStock(WINDURST, WINDURST, 32768)
        assert(r and r.item == 17159)
        r = getStock(WINDURST, OTHER, 32932)
        assert(r and r.item == 17585)
        assert(getStock(WINDURST, WINDURST, 99999) == nil)
    end)
end)
