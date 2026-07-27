require('scripts/globals/mobs')

describe('Mob call pets admission', function()
    it('refuses a busy owner unless told to ignore it', function()
        assert(not xi.mob.callPetAdmitted(true, false, true))
        assert(xi.mob.callPetAdmitted(true, true, true))
        assert(xi.mob.callPetAdmitted(false, false, true))
    end)

    it('requires at least one pet that is not already spawned', function()
        assert(not xi.mob.callPetAdmitted(false, false, false))
        assert(not xi.mob.callPetAdmitted(true, true, false))
    end)
end)

describe('Mob call pets id normalization', function()
    it('wraps a single pet id into a list', function()
        local ids = xi.mob.callPetIds(500, nil)
        assert(#ids == 1 and ids[1] == 500)
    end)

    it('passes an existing list through', function()
        local ids = xi.mob.callPetIds({ 500, 501 }, 999)
        assert(#ids == 2 and ids[1] == 500 and ids[2] == 501)
    end)

    it('falls back to the owner pet when no list is given', function()
        local ids = xi.mob.callPetIds(nil, 700)
        assert(#ids == 1 and ids[1] == 700)
    end)

    it('yields an empty list when the owner has no pet', function()
        assert(#xi.mob.callPetIds(nil, nil) == 0)
    end)
end)

describe('Mob call pets inactive time', function()
    it('collapses sub-second timings to an instant call', function()
        assert(xi.mob.callPetInactiveTime(nil) == 0)
        assert(xi.mob.callPetInactiveTime(0) == 0)
        assert(xi.mob.callPetInactiveTime(999) == 0)
    end)

    it('keeps timings of a second or more', function()
        assert(xi.mob.callPetInactiveTime(1000) == 1000)
        assert(xi.mob.callPetInactiveTime(3000) == 3000)
    end)
end)

describe('Mob call pets spawn cap', function()
    it('defaults the cap to every candidate pet', function()
        assert(xi.mob.callPetMaxSpawns(nil, 3) == 3)
        assert(xi.mob.callPetMaxSpawns(1, 3) == 1)
    end)

    it('selects existing unspawned pets in order up to the cap', function()
        local ids = xi.mob.callPetSpawnIds({
            { id = 500, exists = true,  spawned = true },
            { id = 501, exists = false, spawned = false },
            { id = 502, exists = true,  spawned = false },
            { id = 503, exists = true,  spawned = false },
        }, 1)
        assert(#ids == 1 and ids[1] == 502)

        ids = xi.mob.callPetSpawnIds({
            { id = 502, exists = true, spawned = false },
            { id = 503, exists = true, spawned = false },
        }, 2)
        assert(#ids == 2 and ids[1] == 502 and ids[2] == 503)
    end)

    it('places a summoned pet around the owner using X/Z jitter', function()
        local pos = xi.mob.callPetSpawnPosition({ x = 100, y = 5, z = -20, rot = 77 }, 2, -2)
        assert(pos.x == 102 and pos.y == 5 and pos.z == -22 and pos.rot == 77)
    end)
end)

describe('Mob call pets action packet', function()
    it('announces Call Beast for a beastmaster owner', function()
        local action = xi.mob.callPetActionParams(xi.job.BST, 0, nil)

        assert(action.finishCategory == xi.action.category.MOBABILITY_FINISH)
        assert(action.animationID == 718)
        assert(action.actionID == xi.mobSkill.CALL_BEAST)
        assert(action.messageID == xi.msg.basic.USES)
    end)

    it('announces Call Wyvern for a dragoon owner', function()
        local action = xi.mob.callPetActionParams(xi.job.DRG, 0, nil)

        assert(action.finishCategory == xi.action.category.MOBABILITY_FINISH)
        assert(action.animationID == 438)
        assert(action.actionID == xi.mobSkill.CALL_WYVERN_1)
        assert(action.messageID == xi.msg.basic.USES)
    end)

    it('announces Activate as a job ability for a puppetmaster owner', function()
        local action = xi.mob.callPetActionParams(xi.job.PUP, 0, nil)

        assert(action.finishCategory == xi.action.category.JOBABILITY_FINISH)
        assert(action.animationID == 83)
        assert(action.actionID == xi.jobAbility.ACTIVATE)
        assert(action.messageID == xi.msg.basic.USES_JA)
    end)

    it('falls back to the generic two-hour packet for other jobs', function()
        local action = xi.mob.callPetActionParams(xi.job.WAR, 0, nil)

        assert(action.finishCategory == 11)
        assert(action.animationID == 439)
        assert(action.actionID == 307)
        assert(action.messageID == 0)
        assert(action.param == 0)
    end)

    it('falls back to the generic packet for a delayed call', function()
        local action = xi.mob.callPetActionParams(xi.job.BST, 2000, nil)
        assert(action.animationID == 439)
    end)

    it('falls back to the generic packet when an override is present', function()
        local action = xi.mob.callPetActionParams(xi.job.BST, 0, { animationID = 12 })
        assert(action.animationID == 12)
    end)

    it('applies each generic override field', function()
        local action = xi.mob.callPetActionParams(nil, 0, {
            finishCategory = 6,
            animationID    = 12,
            param          = 34,
        })

        assert(action.finishCategory == 6)
        assert(action.animationID == 12)
        assert(action.param == 34)
    end)

    -- The generic actionID reads params.action.messageID, not actionID, so an
    -- actionID override is ignored while a messageID override drives both
    -- fields. Pinned as-is; it looks like an upstream copy-paste slip.
    it('derives the generic actionID from the messageID override', function()
        local ignored = xi.mob.callPetActionParams(nil, 0, { actionID = 999 })
        assert(ignored.actionID == 307)

        local shared = xi.mob.callPetActionParams(nil, 0, { messageID = 42 })
        assert(shared.actionID == 42 and shared.messageID == 42)
    end)
end)
