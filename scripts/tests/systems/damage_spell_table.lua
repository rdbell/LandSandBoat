-----------------------------------
-- Pure system tests for damage_spell pTable (local table mirrored).
-----------------------------------

describe('damage spell pTable pure catalog', function()
    -- Column indices match xi.spells.damage column in damage_spell.lua.
    local column =
    {
        STAT_USED         =  1,
        BONUS_MACC        =  2,
        FORCE_DAY_WEATHER =  3,
        NPC_POWER         =  4,
        NPC_MULTIPLIER    =  5,
        PC_POWER          =  6,
        INFLEXION_POINT   =  7,
        MULTIPLIER_0      =  8,
        MULTIPLIER_50     =  9,
        MULTIPLIER_100    = 10,
        MULTIPLIER_200    = 11,
        MULTIPLIER_300    = 12,
        MULTIPLIER_400    = 13,
        MULTIPLIER_500    = 14,
    }

    -- Corner rows mirrored from damage_spell.lua pTable for Go parity.
    local pTable =
    {
        [xi.magic.spell.AERO] = { xi.mod.INT, 0, false, 25, 1, 40, 35, 1.6, 1, 0, 0, 0, 0, 0 },
        [xi.magic.spell.AERO_V] = { xi.mod.INT, 25, false, 738, 2.3, 750, 550, 5.2, 4.5, 3.9, 2.98, 1.98, 1, 0 },
        [xi.magic.spell.GEOHELIX] = { xi.mod.INT, 0, true, 35, 1, 31, 100, 1, 1, 0.5, 0, 0, 0, 0 },
        [xi.magic.spell.DEATH] = { 0, 0, false, 32, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0 },
        [xi.magic.spell.DOTON_ICHI] = { xi.mod.INT, 0, false, 16, 1, 16, 25, 0 },
        [xi.magic.spell.CURE] = { xi.mod.MND, 0, false, 7, 1, 7, 16, 0 },
        [xi.magic.spell.CURE_VI] = { xi.mod.MND, 0, false, 295, 2, 295, 212, 0 },
        [xi.magic.spell.BANISH] = { xi.mod.MND, 0, false, 14, 1, 14, 25, 0 },
        [xi.magic.spell.KAUSTRA] = { xi.mod.INT, 0, false, 0, 0.67, 0, 300, 0.67, 0.67, 0.67, 0.67, 0, 0, 0 },
        [xi.magic.spell.COMET] = { xi.mod.INT, 0, false, 552, 2, 700, 700, 2, 2, 2, 2, 2, 2, 2 },
    }

    it('AERO INT power and new-system mults', function()
        local r = pTable[xi.magic.spell.AERO]
        assert(xi.magic.spell.AERO == 154)
        assert(r[column.STAT_USED] == xi.mod.INT)
        assert(r[column.NPC_POWER] == 25)
        assert(r[column.PC_POWER] == 40)
        assert(r[column.INFLEXION_POINT] == 35)
        assert(r[column.MULTIPLIER_0] == 1.6)
        assert(r[column.MULTIPLIER_50] == 1)
        assert(r[column.FORCE_DAY_WEATHER] == false)
    end)

    it('AERO_V bonus macc and ladder mults', function()
        local r = pTable[xi.magic.spell.AERO_V]
        assert(r[column.BONUS_MACC] == 25)
        assert(r[column.NPC_MULTIPLIER] == 2.3)
        assert(r[column.MULTIPLIER_400] == 1)
        assert(r[column.MULTIPLIER_500] == 0)
    end)

    it('GEOHELIX force day weather and Mult100', function()
        local r = pTable[xi.magic.spell.GEOHELIX]
        assert(r[column.FORCE_DAY_WEATHER] == true)
        assert(r[column.PC_POWER] == 31)
        assert(r[column.MULTIPLIER_100] == 0.5)
    end)

    it('DEATH zero stat mod and Mult0', function()
        local r = pTable[xi.magic.spell.DEATH]
        assert(xi.magic.spell.DEATH == 367)
        assert(r[column.STAT_USED] == 0)
        assert(r[column.NPC_POWER] == 32)
        assert(r[column.NPC_MULTIPLIER] == 0)
        assert(r[column.MULTIPLIER_0] == 0)
    end)

    it('short rows DOTON/CURE/BANISH pad Mults as zero', function()
        local d = pTable[xi.magic.spell.DOTON_ICHI]
        assert(d[column.NPC_POWER] == 16)
        assert(d[column.INFLEXION_POINT] == 25)
        assert(d[column.MULTIPLIER_0] == 0)
        -- Short Lua rows omit Mult50..Mult500; nil is treated as 0.
        assert((d[column.MULTIPLIER_50] or 0) == 0)

        local c = pTable[xi.magic.spell.CURE]
        assert(xi.magic.spell.CURE == 1)
        assert(c[column.STAT_USED] == xi.mod.MND)
        assert(c[column.NPC_POWER] == 7)

        local b = pTable[xi.magic.spell.BANISH]
        assert(b[column.STAT_USED] == xi.mod.MND)
        assert(b[column.NPC_POWER] == 14)
    end)

    it('CURE_VI KAUSTRA COMET corners', function()
        local c6 = pTable[xi.magic.spell.CURE_VI]
        assert(c6[column.NPC_POWER] == 295)
        assert(c6[column.NPC_MULTIPLIER] == 2)
        assert(c6[column.INFLEXION_POINT] == 212)

        local k = pTable[xi.magic.spell.KAUSTRA]
        assert(k[column.NPC_MULTIPLIER] == 0.67)
        assert(k[column.MULTIPLIER_0] == 0.67)
        assert(k[column.MULTIPLIER_200] == 0.67)
        assert(k[column.MULTIPLIER_300] == 0)

        local comet = pTable[xi.magic.spell.COMET]
        assert(comet[column.PC_POWER] == 700)
        assert(comet[column.MULTIPLIER_0] == 2)
        assert(comet[column.MULTIPLIER_500] == 2)
    end)
end)
