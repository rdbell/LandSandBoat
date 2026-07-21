-----------------------------------
-- Pure system tests for enfeebling_song pTable (local table mirrored).
-----------------------------------

describe('enfeebling song pTable pure catalog', function()
    local column =
    {
        SONG_EFFECT = 1, SONG_TIER = 2, SONG_POWER_BASE = 3,
        SONG_POWER_CAP = 4, SONG_DURATION = 5, SONG_MODIFIER = 6,
    }

    local pTable =
    {
        [xi.magic.spell.FOE_REQUIEM]       = { xi.effect.REQUIEM,  1,     1,   300,  64, xi.mod.REQUIEM_EFFECT  },
        [xi.magic.spell.FOE_REQUIEM_VII]   = { xi.effect.REQUIEM,  7,     8,   300, 160, xi.mod.REQUIEM_EFFECT  },
        [xi.magic.spell.CARNAGE_ELEGY]     = { xi.effect.ELEGY,    1,  5000,  5000, 180, xi.mod.ELEGY_EFFECT    },
        [xi.magic.spell.FIRE_THRENODY]     = { xi.effect.THRENODY, 1,    50,    95,  60, xi.mod.THRENODY_EFFECT },
        [xi.magic.spell.MAIDENS_VIRELAI]   = { xi.effect.CHARM_I,  1,     0,     0,  30, xi.mod.VIRELAI_EFFECT  },
        [xi.magic.spell.PINING_NOCTURNE]   = { xi.effect.NOCTURNE, 1,    15,    25, 120, 0                      },
        [xi.magic.spell.MAGIC_FINALE]      = { xi.effect.NONE,     1,     1,     1,   0, xi.mod.FINALE_EFFECT   },
    }

    it('FOE_REQUIEM I and VII pins', function()
        local r = pTable[xi.magic.spell.FOE_REQUIEM]
        assert(r[column.SONG_EFFECT] == xi.effect.REQUIEM)
        assert(r[column.SONG_TIER] == 1)
        assert(r[column.SONG_POWER_BASE] == 1)
        assert(r[column.SONG_DURATION] == 64)
        assert(r[column.SONG_MODIFIER] == xi.mod.REQUIEM_EFFECT)
        local vii = pTable[xi.magic.spell.FOE_REQUIEM_VII]
        assert(vii[column.SONG_TIER] == 7 and vii[column.SONG_POWER_BASE] == 8 and vii[column.SONG_DURATION] == 160)
        assert(xi.magic.spell.FOE_REQUIEM == 368)
    end)

    it('Elegy Threnody Virelai Nocturne Finale corners', function()
        local e = pTable[xi.magic.spell.CARNAGE_ELEGY]
        assert(e[column.SONG_POWER_BASE] == 5000 and e[column.SONG_DURATION] == 180)
        local th = pTable[xi.magic.spell.FIRE_THRENODY]
        assert(th[column.SONG_POWER_BASE] == 50 and th[column.SONG_POWER_CAP] == 95)
        local v = pTable[xi.magic.spell.MAIDENS_VIRELAI]
        assert(v[column.SONG_EFFECT] == xi.effect.CHARM_I and v[column.SONG_DURATION] == 30)
        local n = pTable[xi.magic.spell.PINING_NOCTURNE]
        assert(n[column.SONG_POWER_BASE] == 15 and n[column.SONG_MODIFIER] == 0)
        local f = pTable[xi.magic.spell.MAGIC_FINALE]
        assert(f[column.SONG_EFFECT] == xi.effect.NONE and f[column.SONG_DURATION] == 0)
    end)

    it('spell and effect enum pins', function()
        assert(xi.effect.REQUIEM == 192)
        assert(xi.effect.ELEGY == 194)
        assert(xi.effect.THRENODY == 217)
        assert(xi.effect.NOCTURNE == 223)
        assert(xi.mod.REQUIEM_EFFECT ~= nil)
    end)
end)
