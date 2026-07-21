-----------------------------------
-- Pure system tests for executeImmunobreak inject halves (slice 6089).
-----------------------------------

describe('immunobreak pure injects', function()
    local SKILL_ENFEEBLING = xi.skill.ENFEEBLING_MAGIC

    local function immunobreakEligible(p)
        if not p.enabled or not p.casterIsPC or not p.targetIsMob then
            return false
        end
        if p.skillType ~= SKILL_ENFEEBLING then
            return false
        end
        if (p.immunobreakModId or 0) == 0 then
            return false
        end
        if (p.baseResistanceRank or 0) < 6 then
            return false
        end
        local finalRank = p.baseResistanceRank - (p.immunobreakValue or 0)
        if finalRank <= 4 then
            return false
        end
        return true
    end

    local function immunobreakChance(merit, immunobreakValue)
        return merit + 20 / (immunobreakValue + 1)
    end

    it('eligibility gates', function()
        assert(immunobreakEligible({
            enabled = true, casterIsPC = true, targetIsMob = true,
            skillType = SKILL_ENFEEBLING, immunobreakModId = 100,
            baseResistanceRank = 6, immunobreakValue = 0,
        }))
        assert(not immunobreakEligible({
            enabled = false, casterIsPC = true, targetIsMob = true,
            skillType = SKILL_ENFEEBLING, immunobreakModId = 100,
            baseResistanceRank = 6, immunobreakValue = 0,
        }))
        assert(not immunobreakEligible({
            enabled = true, casterIsPC = true, targetIsMob = true,
            skillType = SKILL_ENFEEBLING, immunobreakModId = 100,
            baseResistanceRank = 6, immunobreakValue = 2,
        }))
        assert(immunobreakEligible({
            enabled = true, casterIsPC = true, targetIsMob = true,
            skillType = SKILL_ENFEEBLING, immunobreakModId = 100,
            baseResistanceRank = 7, immunobreakValue = 2,
        }))
    end)

    it('chance formula', function()
        assert(immunobreakChance(0, 0) == 20)
        assert(immunobreakChance(10, 1) == 20)
        assert(immunobreakChance(0, 3) == 5)
    end)
end)
