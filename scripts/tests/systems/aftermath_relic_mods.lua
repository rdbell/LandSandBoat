-----------------------------------
-- Pure system tests for Relic aftermath mod catalogs and sample mythic
-- power formulas (xi.aftermath.effects).
-----------------------------------

describe('Aftermath relic mod pure plans', function()
    it('relic tier 1 catalogs match fixed mod pairs', function()
        local e = xi.aftermath.effects
        assert(e[1].mods[1] == xi.mod.SUBTLE_BLOW and e[1].mods[2] == 10)
        assert(e[6].mods[1] == xi.mod.DMG and e[6].mods[2] == -2000)
        assert(e[8].mods[1] == xi.mod.SPIKES and e[8].mods[2] == xi.subEffect.SHOCK_SPIKES)
        assert(e[8].mods[3] == xi.mod.SPIKES_DMG and e[8].mods[4] == 10)
        assert(e[14].mods[1] == xi.mod.ENMITY and e[14].mods[2] == -20)
    end)

    it('relic tier 2 catalogs and pet/spikes flags', function()
        local e = xi.aftermath.effects
        assert(e[15].mods[1] == xi.mod.SUBTLE_BLOW and e[15].mods[3] == xi.mod.KICK_ATTACK_RATE)
        assert(e[19].includePets == true)
        assert(e[19].mods[2] == 10)
        assert(e[22].mods[2] == xi.subEffect.SHOCK_SPIKES and e[22].mods[4] == 19)
        assert(e[22].mods[6] == 5 and e[22].mods[8] == 5) -- ATTP 5, DA 5
        assert(e[28].mods[1] == xi.mod.ENMITY and e[28].mods[2] == -25)
    end)

    it('relic duration formulas match tier helpers', function()
        local e = xi.aftermath.effects
        assert(e[1].duration(2000) == 40)   -- floor(2000*0.02)
        assert(e[15].duration(2000) == 120) -- floor(2000*0.06)
        assert(e[1].duration(1500) == 30)
    end)

    it('mythic id 29 level selects one mod entry with TP power function', function()
        -- onEffectGain: mods = aftermath.mods[floor(tp/1000)] then pairs (mod, fn)
        local e = xi.aftermath.effects[29]
        local tp = 2000
        local level = math.floor(tp / 1000) -- 2
        local mods = e.mods[level]
        assert(mods[1] == xi.mod.ATT)
        assert(mods[2](tp) == math.floor(2 * tp / 50 - 60)) -- 20
        -- level 1 ACC
        mods = e.mods[1]
        assert(mods[1] == xi.mod.ACC)
        assert(mods[2](tp) == math.floor(tp / 100)) -- 20
        -- level 3 OCC fixed 40
        mods = e.mods[3]
        assert(mods[2](tp) == 40)
    end)

    it('mythic id 34 level 1 ACC formula', function()
        local e = xi.aftermath.effects[34]
        local tp = 2000
        local mods = e.mods[1]
        assert(mods[1] == xi.mod.ACC)
        assert(mods[2](tp) == math.floor(3 * tp / 200)) -- 30
        mods = e.mods[2]
        assert(mods[1] == xi.mod.ATT)
        assert(mods[2](tp) == math.floor(3 * tp / 50 - 90)) -- 30
    end)
end)
