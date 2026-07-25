-----------------------------------
-- Pure system tests for fSTR / fSTR2 dual-wire helpers (slice 6687).
-- Calls production xi.combat.physical.meleeStatFactor / rangedStatFactor.
-- Goldens match internal/fstr (0906).
-----------------------------------

require('scripts/globals/combat/physical_utilities')

local phys = xi.combat.physical

describe('fSTR melee early return and mob/pet ladder', function()
    it('mobs at mainLvl <= 1 return 1; pets still evaluate', function()
        assert(phys.meleeStatFactor(true, false, 1, 100, 0, 0) == 1)
        assert(phys.meleeStatFactor(true, false, 0, 100, 0, 0) == 1)
        -- Pet lvl 1: floor((100-4)/4)=24 → clamp [floor(0.2)-1=-1, 5] → 5
        assert(phys.meleeStatFactor(false, true, 1, 100, 0, 0) == 5)
        -- PC ignores early return: dSTR0 rank0 → raw 8 /4 = 2
        assert(phys.meleeStatFactor(false, false, 1, 50, 50, 0) == 2)
    end)

    it('mob/pet ladder at lvl 10 clamps to [1, 7]', function()
        assert(phys.meleeStatFactorMobPet(0, 10) == 1)   -- floor(0)=0 → 1
        assert(phys.meleeStatFactorMobPet(9, 10) == 2)   -- floor(8/4)=2
        assert(phys.meleeStatFactorMobPet(26, 10) == 5)
        assert(phys.meleeStatFactorMobPet(36, 10) == 7)  -- floor(8)=8 → clamp 7
        assert(phys.meleeStatFactor(true, false, 10, 9, 0, 0) == 2)
        assert(phys.meleeStatFactor(false, true, 10, 9, 0, 0) == 2)
    end)

    it('mob/pet level clamp at 75 is [14, 20]', function()
        assert(phys.meleeStatFactorMobPet(200, 75) == 20)
        assert(phys.meleeStatFactorMobPet(-200, 75) == 14)
        assert(phys.meleeStatFactorMobPet(60, 75) == 14)
        assert(phys.meleeStatFactorMobPet(84, 75) == 20)
    end)
end)

describe('fSTR melee PC bands', function()
    it('rank 3 lower=-3 upper=11 with fractional results', function()
        assert(phys.meleeStatFactorPC(0, 3) == 2)            -- raw 8/4
        assert(phys.meleeStatFactorPC(12, 3) == 4)           -- 16/4
        assert(math.abs(phys.meleeStatFactorPC(11, 3) - 4.25) < 1e-12) -- 17/4
        assert(math.abs(phys.meleeStatFactorPC(-2, 3) - 1.5) < 1e-12)
        assert(math.abs(phys.meleeStatFactorPC(-7, 3) - 0.5) < 1e-12)
        assert(math.abs(phys.meleeStatFactorPC(-15, 3) - (-1.25)) < 1e-12)
        assert(phys.meleeStatFactorPC(40, 3) == 11)          -- upper
        assert(phys.meleeStatFactorPC(41, 3) == 11)          -- stat clamp
        assert(phys.meleeStatFactorPC(-26, 3) == -3)         -- lower
        assert(phys.meleeStatFactor(false, false, 75, 0, 0, 3) == 2)
    end)

    it('rank 0 lower cap is -1 special case', function()
        assert(phys.meleeStatFactorPC(-14, 0) == -1)
        assert(phys.meleeStatFactorPC(-100, 0) == -1)
        assert(phys.meleeStatFactorPC(28, 0) == 8)
        assert(phys.meleeStatFactorPC(100, 0) == 8)
    end)

    it('keeps half-integers (no llround)', function()
        assert(phys.meleeStatFactorPC(-7, 5) == 0.5)
        assert(phys.meleeStatFactorPC(-2, 5) == 1.5)
        assert(phys.meleeStatFactorPC(-12, 5) == -0.5)
        assert(phys.meleeStatFactorPC(-18, 5) == -1.5)
    end)
end)

describe('fSTR2 ranged paths', function()
    it('mob early return and pet ladder', function()
        assert(phys.rangedStatFactor(true, false, 1, 99, 0, 0) == 1)
        -- Pet lvl 1: clamp floor((0.2-1)*2)=-2 .. floor((0.2+5)*2)=10; dSTR99 → 47 → 10
        assert(phys.rangedStatFactor(false, true, 1, 99, 0, 0) == 10)
    end)

    it('PC ranged uses /2 clamp with rank 0/1 lower specials', function()
        -- rank 3: upper=(3+8)*2=22; lower=-6
        -- dSTR 0 → raw 8 /2 = 4
        assert(phys.rangedStatFactorPC(0, 3) == 4)
        -- dSTR 12 → raw 16 /2 = 8
        assert(phys.rangedStatFactorPC(12, 3) == 8)
        -- rank 0 lower -2
        assert(phys.rangedStatFactorPC(-100, 0) == -2)
        -- rank 1 lower -3
        assert(phys.rangedStatFactorPC(-100, 1) == -3)
        assert(phys.rangedStatFactor(false, false, 75, 0, 0, 3) == 4)
    end)

    it('mob/pet ranged clamps with float mLvl/5', function()
        -- lvl 10 → ml=2; lower=floor(2)=2? floor((2-1)*2)=2, upper=floor((2+5)*2)=14
        -- Wait: floor((10/5 - 1)*2)=floor(1*2)=2, floor((2+5)*2)=14
        local got = phys.rangedStatFactorMobPet(0, 10)
        -- floor(0/2)=0 → clamp 2
        assert(got == 2)
        -- high dSTR
        assert(phys.rangedStatFactorMobPet(200, 10) == 14)
    end)
end)

describe('fSTR player raw and ladders independence', function()
    it('playerStatDiffRaw bands', function()
        assert(phys.playerStatDiffRaw(12) == 16)
        assert(phys.playerStatDiffRaw(6) == 12)
        assert(phys.playerStatDiffRaw(0) == 8)
        assert(phys.playerStatDiffRaw(-2) == 6)
        assert(phys.playerStatDiffRaw(-22) == -9)
    end)

    it('melee and ranged mob ladders differ at dSTR 15', function()
        -- melee >=17 uses (sd-2)/4; at 15 uses (sd-1)/4
        assert(math.abs(phys.mobMeleeStatLadder(15) - (15 - 1) / 4) < 1e-12)
        -- ranged >=15 uses (sd-2)/2
        assert(math.abs(phys.mobRangedStatLadder(15) - (15 - 2) / 2) < 1e-12)
    end)
end)
