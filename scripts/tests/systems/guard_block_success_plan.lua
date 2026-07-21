-----------------------------------
-- Pure system tests for isGuarded / isBlocked post-roll plans (slice 6100).
-----------------------------------

describe('guard and block success plan pure injects', function()
    local SKILL_GUARD  = 28
    local SKILL_SHIELD = 30
    local SKILL_PARRY  = 31

    local function shouldTrySkillUp(isPC, success, oldStyle)
        return isPC and (success or not oldStyle)
    end

    local function tacticalTP(success, isPC, hasTrait, mod)
        if not success or not isPC or not hasTrait then
            return 0
        end
        return mod
    end

    local function guardPlan(p)
        local try = shouldTrySkillUp(p.isPC, p.guarded, p.oldStyle)
        return {
            tp = tacticalTP(p.guarded, p.isPC, p.hasTacticalGuard, p.tacticalGuardMod or 0),
            trySkillUp = try,
            skillId = try and SKILL_GUARD or 0,
        }
    end

    local function blockPlan(p)
        local try = shouldTrySkillUp(p.isPC, p.blocked, p.oldStyle)
        return {
            trySkillUp = try,
            skillId = try and SKILL_SHIELD or 0,
        }
    end

    it('skill ID pins', function()
        assert(SKILL_GUARD == 28)
        assert(SKILL_SHIELD == 30)
        assert(SKILL_PARRY == 31)
    end)

    it('guard success PC full stack', function()
        local r = guardPlan({
            guarded = true,
            isPC = true,
            hasTacticalGuard = true,
            tacticalGuardMod = 150,
            oldStyle = true,
        })
        assert(r.tp == 150)
        assert(r.trySkillUp == true)
        assert(r.skillId == SKILL_GUARD)
    end)

    it('guard fail new style still skills up', function()
        local r = guardPlan({
            guarded = false,
            isPC = true,
            hasTacticalGuard = true,
            tacticalGuardMod = 99,
            oldStyle = false,
        })
        assert(r.tp == 0)
        assert(r.trySkillUp == true)
        assert(r.skillId == SKILL_GUARD)
    end)

    it('guard non-PC never TP or skill-up', function()
        local r = guardPlan({
            guarded = true,
            isPC = false,
            hasTacticalGuard = true,
            tacticalGuardMod = 50,
            oldStyle = false,
        })
        assert(r.tp == 0 and r.trySkillUp == false and r.skillId == 0)
    end)

    it('block skill-up only', function()
        local r = blockPlan({ blocked = true, isPC = true, oldStyle = true })
        assert(r.trySkillUp == true and r.skillId == SKILL_SHIELD)
        r = blockPlan({ blocked = false, isPC = true, oldStyle = true })
        assert(r.trySkillUp == false and r.skillId == 0)
        r = blockPlan({ blocked = false, isPC = true, oldStyle = false })
        assert(r.trySkillUp == true and r.skillId == SKILL_SHIELD)
        r = blockPlan({ blocked = true, isPC = false, oldStyle = false })
        assert(r.trySkillUp == false and r.skillId == 0)
    end)
end)
