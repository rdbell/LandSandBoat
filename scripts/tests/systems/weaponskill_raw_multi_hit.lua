require('scripts/globals/weaponskills')

describe('Weaponskill multi-hit remaining and loop gates', function()
    it('counts remaining mainhand hits after the first', function()
        assert(xi.weaponskills.mainhandHitsRemaining(1) == 0)
        assert(xi.weaponskills.mainhandHitsRemaining(3) == 2)
        assert(xi.weaponskills.mainhandHitsRemaining(5) == 4)
    end)

    it('continues the hit loop only below eight swings and target HP', function()
        assert(xi.weaponskills.wsHitLoopContinues(0, 0, 1000))
        assert(xi.weaponskills.wsHitLoopContinues(7, 999, 1000))
        assert(not xi.weaponskills.wsHitLoopContinues(8, 0, 1000))
        assert(not xi.weaponskills.wsHitLoopContinues(3, 1000, 1000))
        assert(not xi.weaponskills.wsHitLoopContinues(3, 1001, 1000))
    end)

    it('detects ammo exhaustion when tracking a finite count', function()
        assert(not xi.weaponskills.ammoExhausted(0, -1)) -- not tracking
        assert(not xi.weaponskills.ammoExhausted(0, 5))
        assert(not xi.weaponskills.ammoExhausted(4, 5))
        assert(xi.weaponskills.ammoExhausted(5, 5))
        assert(xi.weaponskills.ammoExhausted(6, 5))
    end)
end)

describe('Weaponskill landed-hit TP classification', function()
    it('counts second H2H or DW swing as a TP hit', function()
        assert(xi.weaponskills.classifyLandedHitTP(true, false, 1, false, false) == 'tp')
        assert(xi.weaponskills.classifyLandedHitTP(false, true, 1, false, false) == 'tp')
        assert(xi.weaponskills.classifyLandedHitTP(false, true, 1, false, true) == 'tp')
    end)

    it('counts barrage as a TP hit', function()
        assert(xi.weaponskills.classifyLandedHitTP(false, false, 3, true, false) == 'tp')
    end)

    it('classifies later mainhand and offhand hits as extra', function()
        assert(xi.weaponskills.classifyLandedHitTP(false, false, 2, false, false) == 'main')
        assert(xi.weaponskills.classifyLandedHitTP(true, false, 2, false, false) == 'main')
        assert(xi.weaponskills.classifyLandedHitTP(false, true, 2, false, true) == 'offhand')
    end)
end)

describe('Weaponskill multi-proc budget and offhand base', function()
    it('increments multi-proc count only when extras land', function()
        assert(xi.weaponskills.nextMultiProcCount(0, 0) == 0)
        assert(xi.weaponskills.nextMultiProcCount(0, 2) == 1)
        assert(xi.weaponskills.nextMultiProcCount(1, 1) == 2)
    end)

    it('builds offhand base without flooring', function()
        -- 50 + 10 + 20 * 0.9 = 78
        assert(xi.weaponskills.offhandBaseDamage(50, 10, 20, 0.9) == 78)
    end)
end)

describe('Weaponskill all-hits WSD product', function()
    it('skips entirely on Jump skills', function()
        assert(xi.weaponskills.allHitsWSDProduct(100, 20, 15, false, 30, true) == 100)
    end)

    it('applies all-hits percent then first-hit bonus', function()
        -- 100 * 1.2 + 30 = 150
        assert(xi.weaponskills.allHitsWSDProduct(100, 20, 0, false, 30, false) == 150)
    end)

    it('stacks per-WS only when positive and not a pet', function()
        -- 10 + 15 = 25%: 100 * 1.25 + 0 = 125
        assert(xi.weaponskills.allHitsWSDProduct(100, 10, 15, false, 0, false) == 125)
        assert(xi.weaponskills.allHitsWSDProduct(100, 10, 15, true, 0, false) == 110)
        assert(xi.weaponskills.allHitsWSDProduct(100, 10, 0, false, 0, false) == 110)
        assert(xi.weaponskills.allHitsWSDProduct(100, 10, -5, false, 0, false) == 110)
    end)
end)
