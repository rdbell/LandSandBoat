require('scripts/globals/weaponskills')

describe('Weaponskill fTP with hybrid override', function()
    it('adds bonus fTP to the segment curve', function()
        -- fTP at 1000 with {1,2,3} is 1; +0.25 = 1.25
        assert(xi.weaponskills.weaponskillFTP(1000, { 1, 2, 3 }, 0.25, false) == 1.25)
    end)

    it('uses one plus bonus fTP only on hybrid hits', function()
        assert(xi.weaponskills.weaponskillFTP(2500, { 1, 2, 3 }, 0.25, true) == 1.25)
        -- hybrid ignores the ftp table entirely
        assert(xi.weaponskills.weaponskillFTP(1000, { 5, 5, 5 }, 0, true) == 1)
    end)
end)

describe('Weaponskill Sneak and Trick Attack first-hit bonus', function()
    it('adds pDIF times floored DEX factor for Sneak Attack', function()
        -- dexFactor = floor(100 * 1.1) = 110; floor(50 + 1.5 * 110) = floor(50+165) = 215
        assert(xi.weaponskills.weaponskillSneakAttackBonus(50, 1.5, 100, 10) == 215)
    end)

    it('adds pDIF times floored AGI factor for Trick Attack', function()
        -- agiFactor = floor(80 * 1.05) = 84; floor(40 + 2 * 84) = 208
        assert(xi.weaponskills.weaponskillTrickAttackBonus(40, 2, 80, 5) == 208)
    end)

    it('applies AUGMENTS_SA and AUGMENTS_TA as percent multiplies', function()
        assert(xi.weaponskills.weaponskillAugmentSA(100, 25) == 125)
        assert(xi.weaponskills.weaponskillAugmentTA(200, 10) == 220)
        assert(xi.weaponskills.weaponskillAugmentSA(100, 0) == 100)
    end)
end)

describe('Weaponskill first-hit WSD and multi-hit fTP', function()
    it('stores first-hit bonus as a fraction of final damage', function()
        -- 200 * 15 / 100 = 30
        assert(xi.weaponskills.weaponskillFirstHitBonus(200, 15) == 30)
        assert(xi.weaponskills.weaponskillFirstHitBonus(100, 0) == 0)
    end)

    it('keeps fTP across hits only when multiHitfTP is set', function()
        assert(xi.weaponskills.multiHitFTPAfterFirst(2.5, true) == 2.5)
        assert(xi.weaponskills.multiHitFTPAfterFirst(2.5, false) == 1)
        assert(xi.weaponskills.multiHitFTPAfterFirst(2.5, nil) == 1)
    end)
end)
