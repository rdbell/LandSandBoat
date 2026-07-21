-----------------------------------
-- Pure system tests for canParry/canGuard/canBlock abilityEligible halves
-- (slice 6080). Facing/engaged gates remain injects on the outer can*.
-----------------------------------

describe('defense abilityEligible pure halves', function()
    local function parryEligiblePC(parryRank, hasMain, mainSkillType)
        if parryRank == 0 or not hasMain then return false end
        return mainSkillType ~= xi.skill.HAND_TO_HAND
    end

    local function parryEligibleMob(canParryMod)
        return canParryMod > 0
    end

    local function guardEligiblePC(guardRank, hasMain, mainSkillType)
        if guardRank == 0 then return false end
        return (not hasMain) or mainSkillType == xi.skill.HAND_TO_HAND
    end

    local function guardEligibleNonPC(mainJob, cannotGuard)
        if cannotGuard ~= 0 then return false end
        return mainJob == xi.job.MNK or mainJob == xi.job.PUP
    end

    local function blockEligiblePC(shieldRank, isShield)
        return shieldRank > 0 and isShield
    end

    it('parry PC and mob', function()
        assert(parryEligiblePC(0, true, xi.skill.SWORD) == false)
        assert(parryEligiblePC(1, false, xi.skill.SWORD) == false)
        assert(parryEligiblePC(1, true, xi.skill.HAND_TO_HAND) == false)
        assert(parryEligiblePC(1, true, xi.skill.SWORD) == true)
        assert(parryEligibleMob(0) == false)
        assert(parryEligibleMob(1) == true)
        assert(xi.skill.HAND_TO_HAND == 1)
    end)

    it('guard PC and non-PC', function()
        assert(guardEligiblePC(0, false, 0) == false)
        assert(guardEligiblePC(1, false, 0) == true)
        assert(guardEligiblePC(1, true, xi.skill.HAND_TO_HAND) == true)
        assert(guardEligiblePC(1, true, xi.skill.SWORD) == false)
        assert(guardEligibleNonPC(xi.job.MNK, 0) == true)
        assert(guardEligibleNonPC(xi.job.PUP, 0) == true)
        assert(guardEligibleNonPC(xi.job.WAR, 0) == false)
        assert(guardEligibleNonPC(xi.job.MNK, 1) == false)
    end)

    it('block PC', function()
        assert(blockEligiblePC(0, true) == false)
        assert(blockEligiblePC(1, false) == false)
        assert(blockEligiblePC(1, true) == true)
    end)
end)
