-----------------------------------
-- Pure system tests for ranged distance dual-wire helpers (slice 6698).
-- Calls production xi.combat.ranged pure exports.
-- Goldens match internal/rangeddist (0845 / 6102).
-----------------------------------

require('scripts/globals/combat/ranged_utilities')

local rng = xi.combat.ranged

describe('ranged distance constants and sweet spots', function()
    it('pins penalty caps and defaults', function()
        assert(rng.maxInnerPenalty == 25)
        assert(rng.maxOuterPenalty == 20)
        assert(rng.maxDistance == 25)
        assert(rng.unlimitedShotRecycleChance == 100)
        assert(rng.sweetSpotDefaults['throwing'][1] == 0.0 and rng.sweetSpotDefaults['throwing'][2] == 1.3)
        assert(rng.sweetSpotDefaults['gun'][1] == 3.0 and rng.sweetSpotDefaults['gun'][2] == 4.3)
        assert(rng.sweetSpotDefaults['longbow'][1] == 6.0 and rng.sweetSpotDefaults['longbow'][2] == 9.5)
        assert(rng.sweetSpots[xi.item.YOICHINOYUMI_75][1] == 5.5)
        assert(rng.sweetSpots[xi.item.YOICHINOYUMI_119_III_NO_QUIVER][2] == 9.5)
    end)
end)

describe('resolveSweetSpotFromParams', function()
    it('resolves weapon ID, skill/subskill, and defaults', function()
        local ss = rng.resolveSweetSpotFromParams({ hasWeapon = false })
        assert(ss[1] == 0.0 and ss[2] == 1.3)

        ss = rng.resolveSweetSpotFromParams({
            hasWeapon = true, weaponId = xi.item.YOICHINOYUMI_119,
            weaponSkillType = xi.skill.ARCHERY, weaponSubSkillType = 0,
        })
        assert(ss[1] == 5.5 and ss[2] == 9.5)

        ss = rng.resolveSweetSpotFromParams({
            hasWeapon = true, weaponId = 1,
            weaponSkillType = xi.skill.ARCHERY, weaponSubSkillType = 4,
        })
        assert(ss[1] == 6.0 and ss[2] == 9.5)

        ss = rng.resolveSweetSpotFromParams({
            hasWeapon = true, weaponId = 1,
            weaponSkillType = xi.skill.ARCHERY, weaponSubSkillType = 0,
        })
        assert(ss[1] == 4.0 and ss[2] == 6.4)

        ss = rng.resolveSweetSpotFromParams({
            hasWeapon = true, weaponId = 1,
            weaponSkillType = xi.skill.MARKSMANSHIP, weaponSubSkillType = 0,
        })
        assert(ss[1] == 5.0 and ss[2] == 8.4)

        ss = rng.resolveSweetSpotFromParams({
            hasWeapon = true, weaponId = 1,
            weaponSkillType = xi.skill.MARKSMANSHIP, weaponSubSkillType = 1,
        })
        assert(ss[1] == 3.0 and ss[2] == 4.3)

        ss = rng.resolveSweetSpotFromParams({
            hasWeapon = true, weaponId = 1,
            weaponSkillType = xi.skill.MARKSMANSHIP, weaponSubSkillType = 2,
        })
        assert(ss[1] == 3.0 and ss[2] == 4.3)

        ss = rng.resolveSweetSpotFromParams({
            hasWeapon = true, weaponId = 1,
            weaponSkillType = xi.skill.THROWING, weaponSubSkillType = 0,
        })
        assert(ss[1] == 0.0 and ss[2] == 1.3)
    end)
end)

describe('attackDistancePenaltyFromParams', function()
    it('returns 0 for non-PC and in-band distances', function()
        assert(rng.attackDistancePenaltyFromParams({
            isPC = false, distance = 0, sweetSpot = { 3.0, 4.3 },
            defenderHitbox = 1, attackerHitbox = 1, cSkillMax = 200,
        }) == 0)

        -- gun 3–4.3 + hitboxes 1+1 → centroid 5.0–6.3
        assert(rng.attackDistancePenaltyFromParams({
            isPC = true, distance = 5.5, sweetSpot = { 3.0, 4.3 },
            defenderHitbox = 1, attackerHitbox = 1, cSkillMax = 200,
        }) == 0)
        assert(rng.attackDistancePenaltyFromParams({
            isPC = true, distance = 5.0, sweetSpot = { 3.0, 4.3 },
            defenderHitbox = 1, attackerHitbox = 1, cSkillMax = 200,
        }) == 0)
        assert(rng.attackDistancePenaltyFromParams({
            isPC = true, distance = 6.3, sweetSpot = { 3.0, 4.3 },
            defenderHitbox = 1, attackerHitbox = 1, cSkillMax = 200,
        }) == 0)
    end)

    it('applies inner and outer interpolations with ceil abs', function()
        -- distance 0: pct -25; abs(ceil((-25/100)*200)) = 50
        assert(rng.attackDistancePenaltyFromParams({
            isPC = true, distance = 0, sweetSpot = { 3.0, 4.3 },
            defenderHitbox = 1, attackerHitbox = 1, cSkillMax = 200,
        }) == 50)

        -- mid-inner distance 2.5, centroidStart 5 → pct -12.5 → abs(ceil(-25)) = 25
        assert(rng.attackDistancePenaltyFromParams({
            isPC = true, distance = 2.5, sweetSpot = { 3.0, 4.3 },
            defenderHitbox = 1, attackerHitbox = 1, cSkillMax = 200,
        }) == 25)

        -- exact half outer: sweet end 5, hitboxes 0, distance 15 → pct 10 → abs(ceil(20)) = 20
        assert(rng.attackDistancePenaltyFromParams({
            isPC = true, distance = 15, sweetSpot = { 0, 5 },
            defenderHitbox = 0, attackerHitbox = 0, cSkillMax = 200,
        }) == 20)

        -- distance 25: pct 20 → abs(ceil(40)) = 40
        assert(rng.attackDistancePenaltyFromParams({
            isPC = true, distance = 25, sweetSpot = { 3.0, 4.3 },
            defenderHitbox = 1, attackerHitbox = 1, cSkillMax = 200,
        }) == 40)

        -- ceil positive fraction: cSkillMax 3 full outer → 0.6 → 1
        assert(rng.attackDistancePenaltyFromParams({
            isPC = true, distance = 25, sweetSpot = { 3.0, 4.3 },
            defenderHitbox = 1, attackerHitbox = 1, cSkillMax = 3,
        }) == 1)
    end)
end)

describe('accuracyDistancePenaltyFromParams', function()
    it('returns 0 for non-PC and inside/at centroidEnd', function()
        assert(rng.accuracyDistancePenaltyFromParams({
            isPC = false, distance = 25, sweetSpot = { 3.0, 4.3 },
            defenderHitbox = 1, attackerHitbox = 1, mainLvl = 75,
        }) == 0)
        assert(rng.accuracyDistancePenaltyFromParams({
            isPC = true, distance = 0, sweetSpot = { 3.0, 4.3 },
            defenderHitbox = 1, attackerHitbox = 1, mainLvl = 75,
        }) == 0)
        assert(rng.accuracyDistancePenaltyFromParams({
            isPC = true, distance = 6.3, sweetSpot = { 3.0, 4.3 },
            defenderHitbox = 1, attackerHitbox = 1, mainLvl = 75,
        }) == 0)
    end)

    it('applies outer floor interpolation', function()
        -- sweet end 5, hitboxes 0, distance 15 → pct 0.5; floor(0.5*75/2)=18
        assert(rng.accuracyDistancePenaltyFromParams({
            isPC = true, distance = 15, sweetSpot = { 0, 5 },
            defenderHitbox = 0, attackerHitbox = 0, mainLvl = 75,
        }) == 18)

        -- distance 25, gun centroidEnd 6.3: floor(1 * 75/2) = 37
        assert(rng.accuracyDistancePenaltyFromParams({
            isPC = true, distance = 25, sweetSpot = { 3.0, 4.3 },
            defenderHitbox = 1, attackerHitbox = 1, mainLvl = 75,
        }) == 37)

        assert(rng.accuracyDistancePenaltyFromParams({
            isPC = true, distance = 25, sweetSpot = { 3.0, 4.3 },
            defenderHitbox = 1, attackerHitbox = 1, mainLvl = 0,
        }) == 0)
    end)
end)

describe('shouldUseAmmoFromParams', function()
    it('non-PC never uses ammo', function()
        local useAmmo, deleteUS = rng.shouldUseAmmoFromParams({
            isPC = false, recycleMod = 0, roll1to100 = 100,
        })
        assert(not useAmmo and not deleteUS)
    end)

    it('compares roll to mod+merit+JP and Unlimited Shot', function()
        local useAmmo, deleteUS = rng.shouldUseAmmoFromParams({
            isPC = true, recycleMod = 25, roll1to100 = 25,
        })
        assert(not useAmmo and not deleteUS)

        useAmmo, deleteUS = rng.shouldUseAmmoFromParams({
            isPC = true, recycleMod = 25, roll1to100 = 26,
        })
        assert(useAmmo and not deleteUS)

        useAmmo, deleteUS = rng.shouldUseAmmoFromParams({
            isPC = true, recycleMod = 10, recycleMerit = 5, ammoConsumptionJP = 3, roll1to100 = 18,
        })
        assert(not useAmmo)

        useAmmo, deleteUS = rng.shouldUseAmmoFromParams({
            isPC = true, recycleMod = 10, recycleMerit = 5, ammoConsumptionJP = 3, roll1to100 = 19,
        })
        assert(useAmmo)

        useAmmo, deleteUS = rng.shouldUseAmmoFromParams({
            isPC = true, recycleMod = 0, hasUnlimitedShot = true, roll1to100 = 100,
        })
        assert(not useAmmo and deleteUS)
    end)
end)
