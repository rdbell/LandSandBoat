-----------------------------------
-- Pure system tests for Mythic/Empyrean aftermath mod catalogs.
-----------------------------------

describe('Aftermath mythic and empyrean mod pure plans', function()
    it('mythic id 29 level selects one mod entry with TP power', function()
        local e = xi.aftermath.effects[29]
        local tp = 2000
        local level = math.floor(tp / 1000)
        local mods = e.mods[level]
        assert(mods[1] == xi.mod.ATT)
        assert(mods[2](tp) == math.floor(2 * tp / 50 - 60)) -- 20
        mods = e.mods[1]
        assert(mods[1] == xi.mod.ACC)
        assert(mods[2](tp) == math.floor(tp / 100)) -- 20 at tp 2000 when used with L1 formula
        -- L1 at tp 1000
        assert(e.mods[1][2](1000) == 10)
        mods = e.mods[3]
        assert(mods[2](tp) == 40)
        assert(e.duration[1] == 60 and e.duration[3] == 120)
    end)

    it('mythic id 30 and 33 tier-1 formulas', function()
        local e = xi.aftermath.effects[30]
        assert(e.mods[1][1] == xi.mod.MACC)
        assert(e.mods[1][2](1000) == 10)
        assert(e.mods[2][1] == xi.mod.ACC)
        assert(e.mods[2][2](2000) == 10) -- floor(20-10)
        assert(e.duration[1] == 180)

        e = xi.aftermath.effects[33]
        assert(e.mods[1][1] == xi.mod.RACC)
        assert(e.mods[2][1] == xi.mod.RATT)
        assert(e.mods[2][2](2000) == 20)
        assert(e.mods[3][1] == xi.mod.REM_OCC_DO_DOUBLE_DMG_RANGED)
        assert(e.mods[3][2](3000) == 40)
    end)

    it('mythic tier-2 id 34 and 38 formulas', function()
        local e = xi.aftermath.effects[34]
        assert(e.mods[1][2](1000) == math.floor(3 * 1000 / 200)) -- 15
        assert(e.mods[2][2](2000) == math.floor(3 * 2000 / 50 - 90)) -- 30
        assert(e.mods[3][2](3000) == 60)
        assert(e.duration[3] == 180)

        e = xi.aftermath.effects[38]
        assert(e.mods[1][2](1000) == math.floor(1000 / 50)) -- 20
        assert(e.mods[3][1] == xi.mod.REM_OCC_DO_DOUBLE_DMG_RANGED)
        assert(e.mods[3][2](3000) == 60)
    end)

    it('mythic tier-3 id 39 and 43 multi-mod level 3', function()
        local e = xi.aftermath.effects[39]
        assert(e.mods[1][2](1000) == math.floor(1000 / 50 + 10)) -- 30
        assert(e.mods[2][2](2000) == math.floor(2000 * 0.6 - 80)) -- 1120
        local mods = e.mods[3]
        assert(mods[1] == xi.mod.MYTHIC_OCC_ATT_TWICE)
        assert(mods[2](3000) == 40)
        assert(mods[3] == xi.mod.MYTHIC_OCC_ATT_THRICE)
        assert(mods[4](3000) == 20)

        e = xi.aftermath.effects[43]
        mods = e.mods[3]
        assert(mods[1] == xi.mod.REM_OCC_DO_DOUBLE_DMG_RANGED)
        assert(mods[3] == xi.mod.REM_OCC_DO_TRIPLE_DMG_RANGED)
        assert(mods[2](3000) == 40 and mods[4](3000) == 20)
        assert(e.mods[2][2](2000) == math.floor(2000 * 0.6 - 80))
    end)

    it('empyrean id 44 and 45 mod and power ladders', function()
        local e = xi.aftermath.effects[44]
        assert(e.mod == xi.mod.REM_OCC_DO_DOUBLE_DMG)
        assert(e.power[1] == 300 and e.power[2] == 400 and e.power[3] == 500)
        assert(e.duration[1] == 30 and e.duration[3] == 90)

        e = xi.aftermath.effects[45]
        assert(e.mod == xi.mod.REM_OCC_DO_TRIPLE_DMG)
        assert(e.power[2] == 400)
        assert(e.duration[2] == 120)
    end)
end)
