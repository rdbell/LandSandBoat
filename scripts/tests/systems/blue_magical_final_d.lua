-----------------------------------
-- Pure system tests for blue magical Final-D assembly (slice 6122).
-- Source: scripts/globals/bluemagic.lua useMagicalSpell ~382–422
-----------------------------------

describe('blue magical FinalD pure plan', function()
    local function clamp(v, lo, hi)
        if v < lo then return lo end
        if v > hi then return hi end
        return v
    end

    local function magicalInitialD(mainLevel, dUpperCap)
        return clamp(mainLevel + 2, 0, dUpperCap)
    end

    -- Pre-mitigation final damage:
    --   wscMult = 1; AF3 → +1; Burst Affinity → +1 + enhances/100
    --   wsc *= wscMult
    --   azure = azureBonus if Azure Lore else 0
    --   final = (initialD + wsc) * (multiplier + azure + correlation) + statBonus
    local function assemble(p)
        local initialD = magicalInitialD(p.mainLevel or 0, p.dUpperCap or 0)
        local wscMult = 1
        if p.af3Proc then
            wscMult = wscMult + 1
        end
        if p.hasBurstAffinity then
            wscMult = wscMult + 1 + (p.enhancesBurstAffinity or 0) / 100
        end
        local wsc = (p.baseWSC or 0) * wscMult
        local azure = 0
        if p.hasAzureLore then
            azure = p.azureBonus or 0
        end
        local mult = (p.multiplier or 0) + azure + (p.correlation or 0)
        local statBonus = (p.diff or 0) * (p.tMultiplier or 0)
        local finalDamage = (initialD + wsc) * mult + statBonus
        return {
            initialD = initialD,
            wsc = wsc,
            wscMultiplier = wscMult,
            azureBonus = azure,
            mult = mult,
            statBonus = statBonus,
            finalDamage = finalDamage,
            consumeBurstAffinity = p.hasBurstAffinity == true,
        }
    end

    it('baseline without AF3/BA/Azure', function()
        -- lvl 75 → initial 77; wsc 10; mult 1.5; stat 5*1=5
        -- (77+10)*1.5 + 5 = 130.5 + 5 = 135.5
        local r = assemble({
            mainLevel = 75, dUpperCap = 100, baseWSC = 10,
            multiplier = 1.5, diff = 5, tMultiplier = 1,
        })
        assert(r.initialD == 77)
        assert(r.wsc == 10 and r.wscMultiplier == 1)
        assert(r.finalDamage == 135.5)
        assert(r.azureBonus == 0)
        assert(r.consumeBurstAffinity ~= true)
    end)

    it('dUpperCap clamps initial D', function()
        local r = assemble({ mainLevel = 99, dUpperCap = 50, baseWSC = 0, multiplier = 1 })
        assert(r.initialD == 50)
        assert(r.finalDamage == 50)
    end)

    it('AF3 doubles WSC (mult +1)', function()
        local r = assemble({
            mainLevel = 0, dUpperCap = 100, baseWSC = 10, af3Proc = true, multiplier = 1,
        })
        -- initialD = 2; wsc = 20; (2+20)*1 = 22
        assert(r.wscMultiplier == 2 and r.wsc == 20)
        assert(r.finalDamage == 22)
    end)

    it('burst affinity adds 1 + enhances/100 to wsc mult', function()
        local r = assemble({
            mainLevel = 0, dUpperCap = 100, baseWSC = 10,
            hasBurstAffinity = true, enhancesBurstAffinity = 50, multiplier = 1,
        })
        -- wscMult = 1 + 1 + 0.5 = 2.5; wsc = 25; (2+25)*1 = 27
        assert(r.wscMultiplier == 2.5 and r.wsc == 25)
        assert(r.finalDamage == 27)
        assert(r.consumeBurstAffinity == true)
    end)

    it('AF3 plus burst affinity stacks wsc mult', function()
        local r = assemble({
            mainLevel = 0, dUpperCap = 100, baseWSC = 10,
            af3Proc = true, hasBurstAffinity = true, enhancesBurstAffinity = 0, multiplier = 1,
        })
        -- wscMult = 1+1+1 = 3; wsc = 30; (2+30)*1 = 32
        assert(r.wscMultiplier == 3 and r.wsc == 30)
        assert(r.finalDamage == 32)
    end)

    it('azure lore adds azureBonus to mult arm', function()
        local r = assemble({
            mainLevel = 0, dUpperCap = 100, baseWSC = 0,
            multiplier = 1, hasAzureLore = true, azureBonus = 0.5, correlation = 0.25,
        })
        -- mult = 1 + 0.5 + 0.25 = 1.75; (2+0)*1.75 = 3.5
        assert(r.azureBonus == 0.5 and r.mult == 1.75)
        assert(r.finalDamage == 3.5)
    end)

    it('azure bonus ignored without azure lore', function()
        local r = assemble({
            mainLevel = 0, dUpperCap = 100, baseWSC = 0,
            multiplier = 1, hasAzureLore = false, azureBonus = 0.5,
        })
        assert(r.azureBonus == 0 and r.mult == 1)
        assert(r.finalDamage == 2)
    end)

    it('stat bonus is diff * tMultiplier', function()
        local r = assemble({
            mainLevel = 0, dUpperCap = 100, baseWSC = 0, multiplier = 0,
            diff = 20, tMultiplier = 0.5,
        })
        -- mult = 0; (2+0)*0 + 10 = 10
        assert(r.statBonus == 10 and r.finalDamage == 10)
    end)

    it('negative diff yields negative stat bonus', function()
        local r = assemble({
            mainLevel = 0, dUpperCap = 100, baseWSC = 0, multiplier = 1,
            diff = -10, tMultiplier = 1,
        })
        assert(r.statBonus == -10)
        assert(r.finalDamage == 2 - 10)
    end)
end)
