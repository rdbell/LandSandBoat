require('scripts/globals/bluemagic')

describe('Blue magic alpha', function()
    it('uses ceil bands through 60 and 75', function()
        assert(xi.spells.blue.alpha(1) == math.ceil(100 - 1 / 6) / 100)
        assert(xi.spells.blue.alpha(60) == math.ceil(100 - 60 / 6) / 100)
        assert(xi.spells.blue.alpha(61) == math.ceil(100 - (61 - 40) / 2) / 100)
        assert(xi.spells.blue.alpha(75) == math.ceil(100 - (75 - 40) / 2) / 100)
        assert(xi.spells.blue.alpha(76) == 0.83)
        assert(xi.spells.blue.alpha(99) == 0.83)
    end)
end)

describe('Blue magic WSC', function()
    it('sums stat terms then multiplies by alpha', function()
        -- level 99 alpha 0.83; STR 100 * 0.3 = 30 → 24.9
        local wsc = xi.spells.blue.wsc(100, 0, 0, 0, 0, 0, 0, 0.3, 0, 0, 0, 0, 0, 0, 99)
        assert(math.abs(wsc - 24.9) < 1e-9)

        -- level 1 alpha = ceil(100 - 1/6)/100 = ceil(99.833...)/100 = 100/100 = 1
        wsc = xi.spells.blue.wsc(50, 50, 0, 0, 0, 0, 0, 0.2, 0.2, 0, 0, 0, 0, 0, 1)
        assert(math.abs(wsc - 20) < 1e-9)
    end)

    it('treats nil multipliers as zero', function()
        assert(xi.spells.blue.wsc(100, 0, 0, 0, 0, 0, 0, nil, nil, nil, nil, nil, nil, nil, 99) == 0)
    end)
end)

describe('Blue magic cRatio', function()
    it('applies level correction and piecewise min/max', function()
        local c = xi.spells.blue.cRatio(1.0, 75, 75)
        -- no level cor; ratio 1 → min 1.2*1-0.5=0.7; max 1.2*1=1.2
        assert(math.abs(c[1] - 0.7) < 1e-9)
        assert(math.abs(c[2] - 1.2) < 1e-9)

        -- atk below def: ratio 1.0 - 0.05*5 = 0.75
        c = xi.spells.blue.cRatio(1.0, 70, 75)
        assert(math.abs(c[1] - (1.2 * 0.75 - 0.5)) < 1e-9)
    end)

    it('floors min at zero and clamps ratio to [0, 2]', function()
        local c = xi.spells.blue.cRatio(0.1, 75, 75)
        assert(c[1] >= 0)
        c = xi.spells.blue.cRatio(5, 75, 75)
        -- clamped to 2 → min 1.2*2-0.8=1.6; max 1.2*2=2.4
        assert(math.abs(c[1] - 1.6) < 1e-9)
        assert(math.abs(c[2] - 2.4) < 1e-9)
    end)
end)

describe('Blue magic fTP', function()
    it('interpolates two segments', function()
        assert(xi.spells.blue.fTP(0, 1, 2, 3) == 1)
        assert(xi.spells.blue.fTP(1500, 1, 2, 3) == 2)
        assert(xi.spells.blue.fTP(3000, 1, 2, 3) == 3)
        -- midpoint low segment: 750 → 1.5
        assert(math.abs(xi.spells.blue.fTP(750, 1, 2, 3) - 1.5) < 1e-9)
        -- midpoint high segment: 2250 → 2.5
        assert(math.abs(xi.spells.blue.fTP(2250, 1, 2, 3) - 2.5) < 1e-9)
    end)

    it('clamps TP to [0, 3000]', function()
        assert(xi.spells.blue.fTP(-100, 1, 2, 3) == 1)
        assert(xi.spells.blue.fTP(5000, 1, 2, 3) == 3)
    end)
end)

describe('Blue magic fSTR', function()
    it('uses the blue dSTR ladder over two', function()
        assert(xi.spells.blue.fSTR(12) == 8)   -- (12+4)/2
        assert(xi.spells.blue.fSTR(6) == 6)    -- (6+6)/2
        assert(xi.spells.blue.fSTR(0) == 4)    -- (0+8)/2
        assert(xi.spells.blue.fSTR(-22) == -4.5) -- (-22+13)/2
    end)
end)

describe('Blue magic capFSTR', function()
    it('soft-caps at 22 unless ignoreCap', function()
        assert(xi.spells.blue.capFSTR(30, false) == 22)
        assert(xi.spells.blue.capFSTR(30, true) == 30)
        assert(xi.spells.blue.capFSTR(10, false) == 10)
    end)
end)

describe('Blue magic correlation', function()
    it('scales strength bonus and only boosts positives with merits', function()
        assert(xi.spells.blue.correlation(1, 0) == 0.25)
        assert(math.abs(xi.spells.blue.correlation(1, 10) - 0.26) < 1e-9)
        assert(xi.spells.blue.correlation(-1, 50) == -0.25) -- merits do not penalize further
        assert(xi.spells.blue.correlation(0, 50) == 0)
    end)
end)

describe('Blue magic physical initial D', function()
    it('floors skill term and clamps to dUpperCap', function()
        -- floor(300*0.11)*2+3 = floor(33)*2+3 = 69
        assert(xi.spells.blue.physicalInitialD(300, 100) == 69)
        assert(xi.spells.blue.physicalInitialD(500, 40) == 40)
        assert(xi.spells.blue.physicalInitialD(0, 10) == 3)
    end)
end)
