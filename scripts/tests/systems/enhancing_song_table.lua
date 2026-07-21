-----------------------------------
-- Pure system tests for enhancing_song pTable (local table mirrored).
-----------------------------------

describe('enhancing song pTable pure catalog', function()
    local pTable =
    {
        [xi.magic.spell.MAGES_BALLAD]     = { 1, xi.effect.BALLAD,   xi.mod.AUGMENT_SONG_STAT, xi.mod.BALLAD_EFFECT,   0, 0,  1,   0,  1, 1,  0, true },
        [xi.magic.spell.SWORD_MADRIGAL]   = { 1, xi.effect.MADRIGAL, xi.mod.AUGMENT_SONG_STAT, xi.mod.MADRIGAL_EFFECT, xi.merit.MADRIGAL_EFFECT, 0, 5, 85, 45, 4.5, 18, true },
        [xi.magic.spell.KNIGHTS_MINNE]    = { 1, xi.effect.MINNE,    xi.mod.AUGMENT_SONG_STAT, xi.mod.MINNE_EFFECT,    xi.merit.MINNE_EFFECT, xi.jp.MINNE_EFFECT, 8, 0, 30, 3, 10, true },
        [xi.magic.spell.VALOR_MINUET]     = { 1, xi.effect.MINUET,   xi.mod.AUGMENT_SONG_STAT, xi.mod.MINUET_EFFECT,   xi.merit.MINUET_EFFECT, xi.jp.MINUET_EFFECT, 5, 50, 32, 3, 4.3, true },
        [xi.magic.spell.FIRE_CAROL]       = { 1, xi.effect.CAROL,    xi.element.FIRE,          xi.mod.CAROL_EFFECT,    0, 0, 20, 200, 80, 8, 10, true },
        [xi.magic.spell.SINEWY_ETUDE]     = { 1, xi.effect.ETUDE,    xi.mod.STR,               xi.mod.ETUDE_EFFECT,    0, 0,  3,   0,  9, 1,  0, true },
    }

    it('Ballad and Madrigal pins', function()
        local b = pTable[xi.magic.spell.MAGES_BALLAD]
        assert(b[1] == 1 and b[7] == 1 and b[9] == 1 and b[12] == true)
        local m = pTable[xi.magic.spell.SWORD_MADRIGAL]
        assert(m[7] == 5 and m[8] == 85 and m[10] == 4.5 and m[11] == 18)
        assert(m[5] == xi.merit.MADRIGAL_EFFECT)
        assert(xi.magic.spell.MAGES_BALLAD == 386)
    end)

    it('Minne Minuet merit and JP', function()
        local n = pTable[xi.magic.spell.KNIGHTS_MINNE]
        assert(n[5] == xi.merit.MINNE_EFFECT and n[6] == xi.jp.MINNE_EFFECT)
        assert(n[7] == 8 and n[9] == 30)
        local u = pTable[xi.magic.spell.VALOR_MINUET]
        assert(u[5] == xi.merit.MINUET_EFFECT and u[6] == xi.jp.MINUET_EFFECT)
        assert(u[11] == 4.3)
    end)

    it('Carol element sub and Etude stat sub', function()
        local c = pTable[xi.magic.spell.FIRE_CAROL]
        assert(c[3] == xi.element.FIRE and c[7] == 20 and c[9] == 80)
        local e = pTable[xi.magic.spell.SINEWY_ETUDE]
        assert(e[3] == xi.mod.STR and e[7] == 3 and e[9] == 9)
    end)
end)
