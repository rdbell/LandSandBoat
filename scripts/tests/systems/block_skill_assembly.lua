-----------------------------------
-- Pure system tests for block skill assembly injects (slice 6087).
-----------------------------------

describe('block skill assembly pure injects', function()
    local function attackerSkillType(usingH2H, mainWeaponSkillType)
        if usingH2H then
            return xi.skill.HAND_TO_HAND
        end
        return mainWeaponSkillType
    end

    local function nonPCBlockSkillLevel(mainLvl, isTrust)
        if isTrust then
            return math.min(mainLvl, 99)
        end
        return mainLvl
    end

    local function resolvePCShield(hasShield, shieldSize)
        if not hasShield then
            return nil
        end
        return shieldSize
    end

    it('attacker skill type H2H vs main weapon', function()
        assert(attackerSkillType(true, xi.skill.SWORD) == xi.skill.HAND_TO_HAND)
        assert(attackerSkillType(false, xi.skill.SWORD) == xi.skill.SWORD)
    end)

    it('non-PC block skill level trust clamp', function()
        assert(nonPCBlockSkillLevel(50, false) == 50)
        assert(nonPCBlockSkillLevel(120, true) == 99)
        assert(nonPCBlockSkillLevel(75, true) == 75)
    end)

    it('PC shield resolve', function()
        assert(resolvePCShield(false, 3) == nil)
        assert(resolvePCShield(true, 4) == 4)
    end)
end)
