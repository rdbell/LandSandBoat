-----------------------------------
-- Pure system tests for blue physical Final-D assembly (slice 6121).
-- Source: scripts/globals/bluemagic.lua usePhysicalSpell ~232–274
-----------------------------------

describe('blue physical FinalD pure plan', function()
    local function clamp(v, lo, hi)
        if v < lo then return lo end
        if v > hi then return hi end
        return v
    end

    local function physicalInitialD(blueSkill, dUpperCap)
        local d = math.floor(blueSkill * 0.11) * 2 + 3
        return clamp(d, 0, dUpperCap)
    end

    local function capFSTR(fstr, ignoreCap)
        if not ignoreCap and fstr > 22 then
            return 22
        end
        return fstr
    end

    -- assemble: initialD + capped fSTR + WSC*(1+bonusWSC); multiplier/correlation
    local function assemble(p)
        local initialD = physicalInitialD(p.blueSkill or 0, p.dUpperCap or 0)
        local fStr = capFSTR(p.fStr or 0, p.ignoreFSTRCap == true)
        local bonusWSC = 0
        if p.af3Proc then
            bonusWSC = 2
        end
        local multiplier = p.baseMultiplier or 1
        if p.hasChainAffinity then
            local tp = clamp((p.tp or 0) + (p.enchainment or 0), 0, 3000)
            -- FTP inject already resolved as fTP
            multiplier = p.fTP or multiplier
            bonusWSC = bonusWSC + 1
            -- tp available is recorded for host
            p._tpUsed = tp
        end
        if p.hasAzureLore then
            multiplier = p.azureTP or multiplier
        end
        local wsc = (p.baseWSC or 0) * (1 + bonusWSC)
        local finalD = math.floor(initialD + fStr + wsc)
        local correlation = p.correlation or 0
        return {
            finalD = finalD,
            multiplier = multiplier,
            correlation = correlation,
            initialD = initialD,
            fStr = fStr,
            wsc = wsc,
            bonusWSC = bonusWSC,
        }
    end

    it('baseline without status or AF3', function()
        -- skill 300 → floor(33)*2+3 = 69; fStr 10; wsc 20 → final 99
        local r = assemble({
            blueSkill = 300, dUpperCap = 100, fStr = 10, baseWSC = 20,
            baseMultiplier = 1.5,
        })
        assert(r.initialD == 69)
        assert(r.finalD == 99)
        assert(r.multiplier == 1.5 and r.bonusWSC == 0)
        assert(r.wsc == 20)
    end)

    it('fSTR soft cap 22 unless ignore', function()
        local r = assemble({
            blueSkill = 100, dUpperCap = 50, fStr = 30, baseWSC = 0,
        })
        assert(r.fStr == 22)
        r = assemble({
            blueSkill = 100, dUpperCap = 50, fStr = 30, baseWSC = 0, ignoreFSTRCap = true,
        })
        assert(r.fStr == 30)
    end)

    it('AF3 proc triples WSC (bonusWSC=2)', function()
        local r = assemble({
            blueSkill = 0, dUpperCap = 100, fStr = 0, baseWSC = 10, af3Proc = true,
        })
        -- initialD for skill 0 = 3
        assert(r.bonusWSC == 2 and r.wsc == 30)
        assert(r.finalD == 33)
    end)

    it('chain affinity adds bonusWSC and uses fTP multiplier', function()
        local r = assemble({
            blueSkill = 0, dUpperCap = 100, fStr = 0, baseWSC = 10,
            hasChainAffinity = true, fTP = 2.5, baseMultiplier = 1,
            tp = 1500, enchainment = 0,
        })
        assert(r.bonusWSC == 1 and r.wsc == 20)
        assert(r.multiplier == 2.5)
        assert(r.finalD == 23)
    end)

    it('AF3 plus chain affinity bonusWSC=3', function()
        local r = assemble({
            blueSkill = 0, dUpperCap = 100, fStr = 0, baseWSC = 10,
            af3Proc = true, hasChainAffinity = true, fTP = 2,
        })
        assert(r.bonusWSC == 3 and r.wsc == 40)
    end)

    it('azure lore overrides multiplier after CA', function()
        local r = assemble({
            blueSkill = 0, dUpperCap = 100, fStr = 0, baseWSC = 0,
            hasChainAffinity = true, fTP = 2.0,
            hasAzureLore = true, azureTP = 3.5,
        })
        assert(r.multiplier == 3.5)
    end)

    it('dUpperCap clamps initial D', function()
        local r = assemble({
            blueSkill = 500, dUpperCap = 40, fStr = 0, baseWSC = 0,
        })
        -- floor(55)*2+3 = 113 → clamp 40
        assert(r.initialD == 40 and r.finalD == 40)
    end)

    it('correlation passes through', function()
        local r = assemble({
            blueSkill = 0, dUpperCap = 10, fStr = 0, baseWSC = 0, correlation = 0.25,
        })
        assert(r.correlation == 0.25)
    end)
end)
