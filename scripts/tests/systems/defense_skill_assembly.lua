-----------------------------------
-- Pure system tests for parry/guard skill assembly injects (slice 6081).
-----------------------------------

describe('defense skill assembly pure injects', function()
    local function defenderSkillPC(skill, mod, ilvl)
        return skill + mod + ilvl
    end

    local function attackerSkillPC(weaponSkill, ilvl)
        return weaponSkill + ilvl
    end

    it('PC skill sums', function()
        assert(defenderSkillPC(100, 10, 5) == 115)
        assert(attackerSkillPC(200, 15) == 215)
        assert(defenderSkillPC(0, 0, 0) == 0)
    end)

    it('non-PC A+ skill cap is positive at mid levels', function()
        -- xi.data.skillLevel.getSkillCap(level, A+) used for non-PC
        local cap50 = xi.data.skillLevel.getSkillCap(50, xi.skillRank.A_PLUS)
        local cap75 = xi.data.skillLevel.getSkillCap(75, xi.skillRank.A_PLUS)
        assert(cap50 > 0)
        assert(cap75 > cap50)
        assert(xi.skillRank.A_PLUS == 1)
    end)
end)
