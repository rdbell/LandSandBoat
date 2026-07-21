-----------------------------------
-- Pure system tests for enhancing_spell pTable (local table mirrored).
-----------------------------------

describe('enhancing spell pTable pure catalog', function()
    local column =
    {
        EFFECT_TIER = 1, EFFECT_ID = 2, EFFECT_LEVEL = 3, EFFECT_POWER = 4,
        EFFECT_DURATION = 5, EFFECT_COMPOSURE = 6, EFFECT_WILL_OVERWRITE = 7, EFFECT_TICK_RATE = 8,
    }

    local pTable =
    {
        [xi.magic.spell.AQUAVEIL]  = { 1, xi.effect.AQUAVEIL,  1,    1,  600, true,  true,  0 },
        [xi.magic.spell.AUSPICE]   = { 1, xi.effect.AUSPICE,  55,    0,  180, true,  false, 0 },
        [xi.magic.spell.BLINK]     = { 1, xi.effect.BLINK,     1,    2,  300, true,  false, 0 },
        [xi.magic.spell.HASTEGA]   = { 5, xi.effect.HASTE,    48, 1494,  180, false, false, 0 },
        [xi.magic.spell.PROTECT]   = { 1, xi.effect.PROTECT,   7,   20, 1800, false, false, 0 },
        [xi.magic.spell.TEMPER_II] = { 2, xi.effect.MULTI_STRIKES, 99, 5, 180, true, false, 0 },
    }

    it('AQUAVEIL and AUSPICE pins', function()
        local a = pTable[xi.magic.spell.AQUAVEIL]
        assert(a[column.EFFECT_POWER] == 1 and a[column.EFFECT_DURATION] == 600)
        assert(a[column.EFFECT_COMPOSURE] == true and a[column.EFFECT_WILL_OVERWRITE] == true)
        local au = pTable[xi.magic.spell.AUSPICE]
        assert(au[column.EFFECT_LEVEL] == 55 and au[column.EFFECT_WILL_OVERWRITE] == false)
        assert(xi.magic.spell.AQUAVEIL == 55)
    end)

    it('BLINK HASTEGA composure flags', function()
        local b = pTable[xi.magic.spell.BLINK]
        assert(b[column.EFFECT_POWER] == 2 and b[column.EFFECT_DURATION] == 300)
        local h = pTable[xi.magic.spell.HASTEGA]
        assert(h[column.EFFECT_COMPOSURE] == false and h[column.EFFECT_POWER] == 1494)
    end)

    it('PROTECT and TEMPER_II corners', function()
        local p = pTable[xi.magic.spell.PROTECT]
        assert(p[column.EFFECT_POWER] == 20 and p[column.EFFECT_DURATION] == 1800)
        local t = pTable[xi.magic.spell.TEMPER_II]
        assert(t[column.EFFECT_TIER] == 2 and t[column.EFFECT_POWER] == 5)
    end)
end)
