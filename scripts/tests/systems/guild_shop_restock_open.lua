-----------------------------------
-- Pure system tests for guild shop restock / open-hours helpers.
-- Mirrors pure APIs in scripts/globals/guild_shops.lua (no entity hosts).
-----------------------------------

describe('Guild shop restock/open pure plans', function()
    it('isOpenHour uses half-open [open, close)', function()
        -- Kamilah hours { 8, 23 }
        assert(not xi.guildShops.isOpenHour(7, 8, 23))
        assert(xi.guildShops.isOpenHour(8, 8, 23))
        assert(xi.guildShops.isOpenHour(12, 8, 23))
        assert(xi.guildShops.isOpenHour(22, 8, 23))
        assert(not xi.guildShops.isOpenHour(23, 8, 23))
        assert(not xi.guildShops.isOpenHour(0, 8, 23))
    end)

    it('alreadyRolledToday and daysSinceRoll', function()
        assert(xi.guildShops.alreadyRolledToday(100, 100))
        assert(not xi.guildShops.alreadyRolledToday(100, 99))
        assert(not xi.guildShops.alreadyRolledToday(100, -1))

        local days, first = xi.guildShops.daysSinceRoll(50, -1)
        assert(first)
        assert(days == 0)

        days, first = xi.guildShops.daysSinceRoll(50, 48)
        assert(not first)
        assert(days == 2)

        days, first = xi.guildShops.daysSinceRoll(50, 50)
        assert(not first)
        assert(days == 0)
    end)

    it('restockAndTrim restocks toward target and never overshoots', function()
        -- restockRate=40, target=180 (Tin Ore style)
        assert(xi.guildShops.restockAndTrim(100, 180, 40, 1, false) == 140)
        assert(xi.guildShops.restockAndTrim(100, 180, 40, 2, false) == 180)
        assert(xi.guildShops.restockAndTrim(100, 180, 40, 3, false) == 180) -- cap at target
        assert(xi.guildShops.restockAndTrim(180, 180, 40, 1, false) == 180) -- already at target
        assert(xi.guildShops.restockAndTrim(100, 180, 0, 5, false) == 100) -- restockRate 0
        -- first roll: no restock, only trim
        assert(xi.guildShops.restockAndTrim(100, 180, 40, 0, true) == 100)
        assert(xi.guildShops.restockAndTrim(200, 180, 40, 0, true) == 180) -- trim only
    end)

    it('restockAndTrim trims overstock from mid-day sales', function()
        assert(xi.guildShops.restockAndTrim(240, 180, 40, 1, false) == 180)
        assert(xi.guildShops.restockAndTrim(240, 180, 0, 1, false) == 180)
    end)

    it('isOffered / seedStock / clamps / sell trade / hidden price / canonical', function()
        assert(xi.guildShops.isOffered(1))
        assert(not xi.guildShops.isOffered(0))

        assert(xi.guildShops.seedStock(nil, 36) == 36)
        assert(xi.guildShops.seedStock(10, 36) == 10)

        assert(xi.guildShops.clampBuyQuantity(5, 3) == 3)
        assert(xi.guildShops.clampBuyQuantity(2, 10) == 2)

        assert(xi.guildShops.clampSellWant(50, 240, 200) == 40) -- room = 40
        assert(xi.guildShops.clampSellWant(10, 240, 200) == 10)

        assert(xi.guildShops.sellTradeResult(5, 5) == 5)
        assert(xi.guildShops.sellTradeResult(3, 5) == -1)

        assert(xi.guildShops.hiddenSellPrice(1000) == bit.bor(1000, 0x80000000))

        assert(xi.guildShops.canonicalName('AliasNPC', 'RealShop') == 'RealShop')
        assert(xi.guildShops.canonicalName('OwnShop', nil) == 'OwnShop')
    end)
end)
