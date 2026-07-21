-----------------------------------
-- Pure system tests for canParry / canGuard / canBlock injects (slice 6211).
-----------------------------------

describe('physical canParry/canGuard/canBlock pure plan', function()
    local H2H = 1
    local MNK, PUP = 2, 18

    local function canParry(p)
        if not p.facing or not p.engaged or p.prevent then
            return false
        end
        if p.isPC then
            if p.parryRank == 0 then return false end
            if not p.hasMain then return false end
            return p.mainSkill ~= H2H
        end
        return p.canParryMod > 0
    end

    local function canGuard(p)
        if not p.facing or not p.engaged or p.prevent then
            return false
        end
        if p.isPC then
            if p.guardRank <= 0 then return false end
            return (not p.hasMain) or p.mainSkill == H2H
        end
        if not p.mobPetTrust then return false end
        if p.mainJob ~= MNK and p.mainJob ~= PUP then return false end
        return p.cannotGuard == 0
    end

    local function canBlock(p)
        if not p.facing or p.prevent then
            return false
        end
        if p.isPC then
            if p.shieldRank <= 0 then return false end
            return p.hasSub and p.subIsShield
        end
        if not p.mobPetTrust then return false end
        return p.canShieldBlock > 0
    end

    it('canParry requires facing, engaged, not prevent', function()
        local base = { facing = true, engaged = true, prevent = false, isPC = true, parryRank = 1, hasMain = true, mainSkill = 3 }
        assert(canParry(base))
        base.facing = false
        assert(not canParry(base))
        base = { facing = true, engaged = false, prevent = false, isPC = true, parryRank = 1, hasMain = true, mainSkill = 3 }
        assert(not canParry(base))
        base = { facing = true, engaged = true, prevent = true, isPC = true, parryRank = 1, hasMain = true, mainSkill = 3 }
        assert(not canParry(base))
    end)

    it('canParry PC excludes H2H and rank 0', function()
        assert(canParry({ facing = true, engaged = true, prevent = false, isPC = true, parryRank = 1, hasMain = true, mainSkill = 3 }))
        assert(not canParry({ facing = true, engaged = true, prevent = false, isPC = true, parryRank = 0, hasMain = true, mainSkill = 3 }))
        assert(not canParry({ facing = true, engaged = true, prevent = false, isPC = true, parryRank = 1, hasMain = false, mainSkill = 3 }))
        assert(not canParry({ facing = true, engaged = true, prevent = false, isPC = true, parryRank = 1, hasMain = true, mainSkill = H2H }))
        assert(canParry({ facing = true, engaged = true, prevent = false, isPC = false, canParryMod = 1 }))
        assert(not canParry({ facing = true, engaged = true, prevent = false, isPC = false, canParryMod = 0 }))
    end)

    it('canGuard PC is bare/H2H; non-PC MNK/PUP without CANNOT_GUARD', function()
        assert(canGuard({ facing = true, engaged = true, prevent = false, isPC = true, guardRank = 1, hasMain = false, mainSkill = 0 }))
        assert(canGuard({ facing = true, engaged = true, prevent = false, isPC = true, guardRank = 1, hasMain = true, mainSkill = H2H }))
        assert(not canGuard({ facing = true, engaged = true, prevent = false, isPC = true, guardRank = 1, hasMain = true, mainSkill = 3 }))
        assert(canGuard({ facing = true, engaged = true, prevent = false, isPC = false, mobPetTrust = true, mainJob = MNK, cannotGuard = 0 }))
        assert(canGuard({ facing = true, engaged = true, prevent = false, isPC = false, mobPetTrust = true, mainJob = PUP, cannotGuard = 0 }))
        assert(not canGuard({ facing = true, engaged = true, prevent = false, isPC = false, mobPetTrust = true, mainJob = 1, cannotGuard = 0 }))
        assert(not canGuard({ facing = true, engaged = true, prevent = false, isPC = false, mobPetTrust = true, mainJob = MNK, cannotGuard = 1 }))
    end)

    it('canBlock does not require engaged; shield or CAN_SHIELD_BLOCK', function()
        assert(canBlock({ facing = true, prevent = false, isPC = true, shieldRank = 1, hasSub = true, subIsShield = true }))
        assert(not canBlock({ facing = true, prevent = false, isPC = true, shieldRank = 0, hasSub = true, subIsShield = true }))
        assert(not canBlock({ facing = true, prevent = false, isPC = true, shieldRank = 1, hasSub = true, subIsShield = false }))
        assert(canBlock({ facing = true, prevent = false, isPC = false, mobPetTrust = true, canShieldBlock = 1 }))
        assert(not canBlock({ facing = true, prevent = false, isPC = false, mobPetTrust = true, canShieldBlock = 0 }))
    end)
end)
