-----------------------------------
-- Pure system tests for WS multi-attack dual-wire (slice 6753).
-- Calls production xi.weaponskills.multiAttackBonusHitsFromParams.
-- Goldens match internal/wsmulti.BonusHits (0982).
-----------------------------------

require('scripts/globals/weaponskills')

local w = xi.weaponskills

describe('WS multi-attack pure pins', function()
    it('bonus hit deltas', function()
        assert(w.quadBonusHits == 3 and w.tripleBonusHits == 2 and w.doubleBonusHits == 1)
    end)
end)

describe('multiAttackBonusHitsFromParams ladder', function()
    it('zero rates never proc', function()
        assert(w.multiAttackBonusHitsFromParams({
            quadRoll = 1, tripleRoll = 1, doubleRoll = 1,
            oaThriceRoll = 1, oaTwiceRoll = 1, firstHit = true,
        }) == 0)
        assert(w.multiAttackBonusHitsFromParams({}) == 0)
    end)

    it('exclusive QA then TA then DA', function()
        assert(w.multiAttackBonusHitsFromParams({
            quadRate = 50, tripleRate = 100, doubleRate = 100,
            oaThriceRate = 100, oaTwiceRate = 100, firstHit = true,
            quadRoll = 50, tripleRoll = 1, doubleRoll = 1,
            oaThriceRoll = 1, oaTwiceRoll = 1,
        }) == 3)
        assert(w.multiAttackBonusHitsFromParams({
            quadRate = 10, tripleRate = 25, doubleRate = 100,
            quadRoll = 11, tripleRoll = 25, doubleRoll = 1,
        }) == 2)
        assert(w.multiAttackBonusHitsFromParams({
            doubleRate = 40, quadRoll = 1, tripleRoll = 1, doubleRoll = 40,
        }) == 1)
        assert(w.multiAttackBonusHitsFromParams({
            doubleRate = 40, quadRoll = 1, tripleRoll = 1, doubleRoll = 41,
        }) == 0)
        assert(w.multiAttackBonusHitsFromParams({
            quadRate = 100, quadRoll = 100,
        }) == 3)
        assert(w.multiAttackBonusHitsFromParams({
            tripleRate = 150, quadRoll = 100, tripleRoll = 100,
        }) == 2)
    end)

    it('mythic OA first-hit only', function()
        assert(w.multiAttackBonusHitsFromParams({
            oaThriceRate = 30, firstHit = true,
            quadRoll = 1, tripleRoll = 1, doubleRoll = 1, oaThriceRoll = 30,
        }) == 2)
        assert(w.multiAttackBonusHitsFromParams({
            oaThriceRate = 10, oaTwiceRate = 50, firstHit = true,
            quadRoll = 1, tripleRoll = 1, doubleRoll = 1,
            oaThriceRoll = 11, oaTwiceRoll = 50,
        }) == 1)
        assert(w.multiAttackBonusHitsFromParams({
            oaThriceRate = 100, oaTwiceRate = 100, firstHit = false,
            quadRoll = 1, tripleRoll = 1, doubleRoll = 1,
            oaThriceRoll = 1, oaTwiceRoll = 1,
        }) == 0)
    end)

    it('DA blocks mythic OA', function()
        assert(w.multiAttackBonusHitsFromParams({
            doubleRate = 50, oaThriceRate = 100, oaTwiceRate = 100,
            firstHit = true, doubleRoll = 50,
            oaThriceRoll = 1, oaTwiceRoll = 1, quadRoll = 1, tripleRoll = 1,
        }) == 1)
    end)

    it('jump double stack and weapon hit fallback', function()
        assert(w.multiAttackBonusHitsFromParams({
            doubleRate = 10, jumpDouble = 30, doubleRoll = 40,
            quadRoll = 1, tripleRoll = 1,
        }) == 0)
        assert(w.multiAttackBonusHitsFromParams({
            doubleRate = 10, jumpDouble = 30, doubleRoll = 40, isJump = true,
            quadRoll = 1, tripleRoll = 1,
        }) == 1)

        assert(w.multiAttackBonusHitsFromParams({
            isJump = true, isPC = true, weaponHitCount = 3,
            quadRoll = 1, tripleRoll = 1, doubleRoll = 1,
        }) == 2)
        assert(w.multiAttackBonusHitsFromParams({
            isJump = true, isPC = true, weaponHitCount = 1,
            quadRoll = 1, tripleRoll = 1, doubleRoll = 1,
        }) == 0)
        assert(w.multiAttackBonusHitsFromParams({
            isJump = true, isPC = true, weaponHitCount = 0,
            quadRoll = 1, tripleRoll = 1, doubleRoll = 1,
        }) == -1)
        assert(w.multiAttackBonusHitsFromParams({
            isJump = true, isPC = false, weaponHitCount = 5,
            quadRoll = 1, tripleRoll = 1, doubleRoll = 1,
        }) == 0)
        assert(w.multiAttackBonusHitsFromParams({
            isJump = true, isPC = true, doubleRate = 100, doubleRoll = 1,
            weaponHitCount = 4, quadRoll = 1, tripleRoll = 1,
        }) == 1)
    end)
end)
