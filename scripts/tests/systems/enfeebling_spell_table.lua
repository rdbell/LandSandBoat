-----------------------------------
-- Pure system tests for enfeebling_spell pTable (local table mirrored).
-----------------------------------

describe('enfeebling spell pTable pure catalog', function()
    -- Mirror of pTable rows used for Go parity (local in enfeebling_spell.lua).
    local column =
    {
        EFFECT_ID = 1, EFFECT_TIER = 2, STAT_USED = 3, BASE_POTENCY = 4,
        BASE_TICK = 5, BASE_DURATION = 6, MESSAGE_OFFSET = 7, SABOTEUR = 8, BONUS_MACC = 9,
    }

    local pTable =
    {
        [xi.magic.spell.BIND]     = { xi.effect.BIND, 1, xi.mod.INT, 0, 0, 60, 0, false, 0 },
        [xi.magic.spell.BLIND]    = { xi.effect.BLINDNESS, 1, xi.mod.INT, 0, 0, 180, 0, true, 0 },
        [xi.magic.spell.CURSE]    = { xi.effect.CURSE_I, 1, xi.mod.INT, 50, 0, 300, 0, false, 0 },
        [xi.magic.spell.DISTRACT] = { xi.effect.EVASION_DOWN, 1, xi.mod.MND, 0, 0, 120, 0, true, 150 },
        [xi.magic.spell.GRAVITY]  = { xi.effect.WEIGHT, 1, xi.mod.INT, 26, 0, 120, 0, true, 0 },
        [xi.magic.spell.YURIN_ICHI] = { xi.effect.INHIBIT_TP, 1, xi.mod.INT, 10, 0, 180, 1, false, 0 },
    }

    it('BIND row', function()
        local r = pTable[xi.magic.spell.BIND]
        assert(r[column.EFFECT_ID] == xi.effect.BIND)
        assert(r[column.BASE_DURATION] == 60)
        assert(r[column.SABOTEUR] == false)
        assert(xi.magic.spell.BIND == 258)
    end)

    it('BLIND saboteur and duration', function()
        local r = pTable[xi.magic.spell.BLIND]
        assert(r[column.EFFECT_ID] == xi.effect.BLINDNESS)
        assert(r[column.BASE_DURATION] == 180)
        assert(r[column.SABOTEUR] == true)
    end)

    it('CURSE fixed potency and DISTRACT bonus macc', function()
        assert(pTable[xi.magic.spell.CURSE][column.BASE_POTENCY] == 50)
        assert(pTable[xi.magic.spell.DISTRACT][column.BONUS_MACC] == 150)
        assert(pTable[xi.magic.spell.GRAVITY][column.BASE_POTENCY] == 26)
        assert(pTable[xi.magic.spell.YURIN_ICHI][column.MESSAGE_OFFSET] == 1)
    end)
end)
