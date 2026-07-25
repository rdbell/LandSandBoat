-----------------------------------
-- Pure system tests for isEntityBusy dual-wire helpers (slice 6700).
-- Calls production xi.combat.behavior pure exports.
-- Goldens match internal/entitybusy (0907).
-----------------------------------

require('scripts/globals/combat/entity_behavior')

local beh = xi.combat.behavior
local cat = xi.action.category

describe('entity busy constants', function()
    it('pins free categories and local-var key', function()
        assert(beh.isBusyLocalVarKey == 'isBusy')
        assert(cat.NONE == 0)
        assert(cat.BASIC_ATTACK == 1)
        assert(cat.ROAMING == 16)
        assert(xi.effect.SLEEP_I == 2)
        assert(xi.effect.PETRIFICATION == 7)
        assert(xi.effect.STUN == 10)
        assert(xi.effect.SLEEP_II == 19)
        assert(xi.effect.TERROR == 28)
        assert(xi.effect.LULLABY == 193)
    end)
end)

describe('isFreeActionCategory', function()
    it('allows NONE, BASIC_ATTACK, ROAMING only', function()
        assert(beh.isFreeActionCategory(cat.NONE))
        assert(beh.isFreeActionCategory(cat.BASIC_ATTACK))
        assert(beh.isFreeActionCategory(cat.ROAMING))
        assert(not beh.isFreeActionCategory(2))  -- RANGED_FINISH
        assert(not beh.isFreeActionCategory(8))  -- MAGIC_START
        assert(not beh.isFreeActionCategory(33)) -- MOBABILITY_START
    end)
end)

describe('isEntityBusyFromParams', function()
    local function free(overrides)
        local p = {
            currentAction = cat.NONE, isPC = true, actionQueueEmpty = true,
            isBusyLocalVar = 0,
        }
        if overrides then
            for k, v in pairs(overrides) do
                p[k] = v
            end
        end
        return p
    end

    it('returns false for idle free baseline', function()
        assert(not beh.isEntityBusyFromParams(free()))
        assert(not beh.isEntityBusyFromParams(free({ currentAction = cat.BASIC_ATTACK })))
        assert(not beh.isEntityBusyFromParams(free({ currentAction = cat.ROAMING })))
    end)

    it('returns true for non-free action categories', function()
        assert(beh.isEntityBusyFromParams(free({ currentAction = 2 })))
        assert(beh.isEntityBusyFromParams(free({ currentAction = 8 })))
        assert(beh.isEntityBusyFromParams(free({ currentAction = 33 })))
    end)

    it('checks action queue only for non-PC', function()
        assert(beh.isEntityBusyFromParams(free({
            isPC = false, actionQueueEmpty = false,
        })))
        assert(not beh.isEntityBusyFromParams(free({
            isPC = false, actionQueueEmpty = true,
        })))
        -- PC ignores queue
        assert(not beh.isEntityBusyFromParams(free({
            isPC = true, actionQueueEmpty = false,
        })))
    end)

    it('returns true for each control status', function()
        assert(beh.isEntityBusyFromParams(free({ hasSleepI = true })))
        assert(beh.isEntityBusyFromParams(free({ hasSleepII = true })))
        assert(beh.isEntityBusyFromParams(free({ hasLullaby = true })))
        assert(beh.isEntityBusyFromParams(free({ hasStun = true })))
        assert(beh.isEntityBusyFromParams(free({ hasTerror = true })))
        assert(beh.isEntityBusyFromParams(free({ hasPetrification = true })))
    end)

    it('returns true when isBusy local var > 0', function()
        assert(not beh.isEntityBusyFromParams(free({ isBusyLocalVar = 0 })))
        assert(beh.isEntityBusyFromParams(free({ isBusyLocalVar = 1 })))
        assert(beh.isEntityBusyFromParams(free({ isBusyLocalVar = 99 })))
        assert(not beh.isEntityBusyFromParams(free({ isBusyLocalVar = -1 })))
    end)

    it('short-circuits on busy action first', function()
        assert(beh.isEntityBusyFromParams(free({
            currentAction = 8, actionQueueEmpty = true, isBusyLocalVar = 0,
        })))
        assert(beh.isEntityBusyFromParams(free({
            currentAction = cat.BASIC_ATTACK, hasStun = true,
        })))
        assert(beh.isEntityBusyFromParams(free({
            currentAction = cat.ROAMING, isBusyLocalVar = 1,
        })))
    end)
end)
