-----------------------------------
-- Pure system tests for isParried post-success plan injects (slice 6099).
-- Composes PARRY_HP_RECOVERY, Tactical Parry TP, and skill-up admission.
-----------------------------------

describe('parry success plan pure injects', function()
    local function parryHPRecovery(parried, mod, hasCurseII)
        if not parried or mod <= 0 or hasCurseII then
            return 0
        end
        return mod
    end

    local function tacticalParryTP(parried, isPC, hasTrait, mod)
        if not parried or not isPC or not hasTrait then
            return 0
        end
        return mod
    end

    local function shouldTrySkillUp(isPC, parried, oldStyle)
        return isPC and (parried or not oldStyle)
    end

    ---@return table plan { hp, tp, trySkillUp }
    local function plan(p)
        return {
            hp = parryHPRecovery(p.parried, p.parryHPMod or 0, p.hasCurseII),
            tp = tacticalParryTP(p.parried, p.isPC, p.hasTacticalParry, p.tacticalParryMod or 0),
            trySkillUp = shouldTrySkillUp(p.isPC, p.parried, p.oldStyle),
        }
    end

    it('failed parry: no HP/TP; skill-up only when new style PC', function()
        local r = plan({ parried = false, isPC = true, parryHPMod = 50, hasTacticalParry = true, tacticalParryMod = 100, oldStyle = true })
        assert(r.hp == 0 and r.tp == 0 and r.trySkillUp == false)
        r = plan({ parried = false, isPC = true, oldStyle = false })
        assert(r.trySkillUp == true)
    end)

    it('success PC full stack', function()
        local r = plan({
            parried = true,
            isPC = true,
            parryHPMod = 40,
            hasCurseII = false,
            hasTacticalParry = true,
            tacticalParryMod = 200,
            oldStyle = true,
        })
        assert(r.hp == 40)
        assert(r.tp == 200)
        assert(r.trySkillUp == true)
    end)

    it('success non-PC: HP only, no TP or skill-up', function()
        local r = plan({
            parried = true,
            isPC = false,
            parryHPMod = 30,
            hasTacticalParry = true,
            tacticalParryMod = 99,
            oldStyle = false,
        })
        assert(r.hp == 30)
        assert(r.tp == 0)
        assert(r.trySkillUp == false)
    end)

    it('curse II blocks HP but not TP', function()
        local r = plan({
            parried = true,
            isPC = true,
            parryHPMod = 50,
            hasCurseII = true,
            hasTacticalParry = true,
            tacticalParryMod = 10,
            oldStyle = false,
        })
        assert(r.hp == 0)
        assert(r.tp == 10)
        assert(r.trySkillUp == true)
    end)
end)
