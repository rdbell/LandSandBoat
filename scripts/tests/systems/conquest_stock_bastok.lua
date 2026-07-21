-----------------------------------
-- Pure system tests for conquest overseerInvNation Bastok catalog (slice 6204).
-----------------------------------

describe('conquest overseer stock Bastok pure plan', function()
    local OTHER = 4
    local BASTOK = 1

    -- overseerInvNation[BASTOK] (option → {rank, cp, lvl, item, place?})
    local bastok =
    {
        [32768] = { rank = 1, cp = 1000, lvl = 10, item = 16433 }, -- LEGIONNAIRES_KNUCKLES
        [32769] = { rank = 1, cp = 1000, lvl = 10, item = 17223 }, -- LEGIONNAIRES_CROSSBOW
        [32770] = { rank = 1, cp = 1000, lvl = 10, item = 16648 }, -- LEGIONNAIRES_AXE
        [32771] = { rank = 1, cp = 1000, lvl = 10, item = 12509 }, -- LEGIONNAIRES_CAP
        [32772] = { rank = 1, cp = 1000, lvl = 10, item = 12752 }, -- LEGIONNAIRES_MITTENS
        [32773] = { rank = 1, cp = 1000, lvl = 10, item = 13003 }, -- LEGIONNAIRES_LEGGINGS
        [32774] = { rank = 1, cp = 1000, lvl = 10, item = 17128, place = 2 }, -- LEGIONNAIRES_STAFF
        [32775] = { rank = 1, cp = 1000, lvl = 10, item = 16780, place = 1 }, -- LEGIONNAIRES_SCYTHE
        [32784] = { rank = 2, cp = 2000, lvl = 18, item = 17048 }, -- DECURIONS_HAMMER
        [32785] = { rank = 2, cp = 2000, lvl = 10, item = 12629 }, -- LEGIONNAIRES_HARNESS
        [32786] = { rank = 2, cp = 2000, lvl = 20, item = 12881 }, -- LEGIONNAIRES_SUBLIGAR
        [32787] = { rank = 2, cp = 2000, lvl = 20, item = 16745, place = 2 }, -- DECURIONS_DAGGER
        [32788] = { rank = 2, cp = 2000, lvl = 20, item = 12337, place = 1 }, -- DECURIONS_SHIELD
        [32800] = { rank = 3, cp = 4000, lvl = 30, item = 16712 }, -- CENTURIONS_AXE
        [32801] = { rank = 3, cp = 4000, lvl = 10, item = 13098 }, -- REPUBLICAN_BRONZE_MEDAL
        [32802] = { rank = 3, cp = 4000, lvl = 30, item = 12438 }, -- CENTURIONS_VISOR
        [32803] = { rank = 3, cp = 4000, lvl = 30, item = 12566 }, -- CENTURIONS_SCALE_MAIL
        [32804] = { rank = 3, cp = 4000, lvl = 30, item = 12694 }, -- CENTURIONS_FINGER_GAUNTLETS
        [32805] = { rank = 3, cp = 4000, lvl = 30, item = 12822 }, -- CENTURIONS_CUISSES
        [32806] = { rank = 3, cp = 4000, lvl = 30, item = 12950 }, -- CENTURIONS_GREAVES
        [32807] = { rank = 3, cp = 4000, lvl = 30, item = 16806, place = 2 }, -- CENTURIONS_SWORD
        [32808] = { rank = 3, cp = 4000, lvl = 30, item = 13830, place = 1 }, -- LEGIONNAIRES_CIRCLET
        [32809] = { rank = 3, cp = 4000, lvl = 1, item = 13497, place = 1 }, -- BASTOKAN_RING
        [32816] = { rank = 4, cp = 8000, lvl = 40, item = 16516 }, -- JUNIOR_MUSKETEERS_TUCK
        [32817] = { rank = 4, cp = 8000, lvl = 40, item = 12422 }, -- IRON_MUSKETEERS_ARMET
        [32818] = { rank = 4, cp = 8000, lvl = 40, item = 12678 }, -- IRON_MUSKETEERS_GAUNTLETS
        [32819] = { rank = 4, cp = 8000, lvl = 40, item = 12934 }, -- IRON_MUSKETEERS_SABATONS
        [32820] = { rank = 4, cp = 8000, lvl = 40, item = 13721, place = 2 }, -- IRON_MUSKETEERS_GAMBISON
        [32821] = { rank = 4, cp = 8000, lvl = 40, item = 17283, place = 1 }, -- JUNIOR_MUSKETEERS_CHAKRAM
        [32832] = { rank = 5, cp = 16000, lvl = 50, item = 16529 }, -- MUSKETEERS_SWORD
        [32833] = { rank = 5, cp = 16000, lvl = 30, item = 13099 }, -- REPUBLICAN_IRON_MEDAL
        [32834] = { rank = 5, cp = 16000, lvl = 50, item = 12550 }, -- IRON_MUSKETEERS_CUIRASS
        [32835] = { rank = 5, cp = 16000, lvl = 50, item = 12806 }, -- IRON_MUSKETEERS_CUISSES
        [32836] = { rank = 5, cp = 16000, lvl = 50, item = 17129, place = 2 }, -- MUSKETEERS_POLE
        [32837] = { rank = 5, cp = 16000, lvl = 50, item = 17253, place = 1 }, -- MUSKETEER_GUN
        [32848] = { rank = 6, cp = 24000, lvl = 55, item = 13100 }, -- REPUBLICAN_MYTHRIL_MEDAL
        [32849] = { rank = 6, cp = 24000, lvl = 55, item = 13582 }, -- REPUBLICAN_ARMY_MANTLE
        [32850] = { rank = 6, cp = 24000, lvl = 55, item = 16557, place = 2 }, -- MUSKETEER_COMMANDERS_FALCHION
        [32851] = { rank = 6, cp = 24000, lvl = 55, item = 12304, place = 2 }, -- MUSKETEER_COMMANDERS_SHIELD
        [32852] = { rank = 6, cp = 24000, lvl = 55, item = 17151, place = 1 }, -- MUSKETEER_COMMANDERS_ROD
        [32853] = { rank = 6, cp = 24000, lvl = 55, item = 13064, place = 1 }, -- IRON_MUSKETEERS_GORGET
        [32864] = { rank = 7, cp = 32000, lvl = 60, item = 15957 }, -- IRON_MUSKETEERS_QUIVER
        [32865] = { rank = 7, cp = 32000, lvl = 60, item = 17807, place = 2 }, -- GOLD_MUSKETEERS_UCHIGATANA
        [32866] = { rank = 7, cp = 32000, lvl = 60, item = 13558, place = 1 }, -- GOLD_MUSKETEERS_RING
        [32880] = { rank = 8, cp = 40000, lvl = 65, item = 14015 }, -- PRAEFECTUSS_GLOVES
        [32881] = { rank = 8, cp = 40000, lvl = 65, item = 13880, place = 2 }, -- PRESIDENTIAL_HAIRPIN
        [32882] = { rank = 8, cp = 40000, lvl = 65, item = 13141, place = 1 }, -- REPUBLICAN_GOLD_MEDAL
        [32896] = { rank = 9, cp = 48000, lvl = 71, item = 16799 }, -- SENIOR_GOLD_MUSKETEERS_SCYTHE
        [32897] = { rank = 9, cp = 48000, lvl = 71, item = 17457 }, -- SENIOR_GOLD_MUSKETEERS_ROD
        [32898] = { rank = 9, cp = 48000, lvl = 71, item = 18196, place = 2 }, -- SENIOR_GOLD_MUSKETEERS_AXE
        [32899] = { rank = 9, cp = 48000, lvl = 71, item = 17655, place = 1 }, -- SENIOR_GOLD_MUSKETEERS_SCIMITAR
        [32912] = { rank = 10, cp = 56000, lvl = 1, item = 14429, place = 1 }, -- REPUBLIC_AKETON
        [32932] = { rank = 10, cp = 5000, lvl = 1, item = 17584 }, -- REPUBLIC_SIGNET_STAFF
        [32940] = { rank = 10, cp = 10000, lvl = 1, item = 6378 }, -- DECORATIVE_CHAIR_SET
    }

    local function getStock(playerNation, guardNation, option)
        if guardNation == OTHER then
            if playerNation == BASTOK then
                return bastok[option]
            end
            return nil
        end
        if guardNation == BASTOK then
            return bastok[option]
        end
        return nil
    end

    it('Bastok catalog size', function()
        local n = 0
        for _ in pairs(bastok) do n = n + 1 end
        assert(n == 54)
    end)

    it('rank-1 knuckles and place-gated staff/scythe', function()
        local r = bastok[32768]
        assert(r.item == 16433 and r.rank == 1 and r.cp == 1000 and r.lvl == 10)
        r = bastok[32774]
        assert(r.item == 17128 and r.place == 2)
        r = bastok[32775]
        assert(r.item == 16780 and r.place == 1)
    end)

    it('nation-only signet staff and decorative chair', function()
        local r = bastok[32932]
        assert(r and r.item == 17584 and r.cp == 5000 and r.rank == 10)
        r = bastok[32940]
        assert(r and r.item == 6378 and r.cp == 10000)
    end)

    it('rank-10 aketon and bastokan ring', function()
        local r = bastok[32912]
        assert(r.item == 14429 and r.place == 1 and r.cp == 56000)
        r = bastok[32809]
        assert(r.item == 13497 and r.place == 1 and r.lvl == 1)
    end)

    it('getStock routes Bastok nation rows', function()
        local r = getStock(BASTOK, BASTOK, 32768)
        assert(r and r.item == 16433)
        r = getStock(BASTOK, OTHER, 32932)
        assert(r and r.item == 17584)
        assert(getStock(BASTOK, BASTOK, 99999) == nil)
    end)
end)
