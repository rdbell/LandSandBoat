require('scripts/globals/job_utils/ranger')

describe('Ranger Bounty Shot', function()
    local function useBountyShot(params)
        local th = params.targetTH
        local updates = {}
        local message
        local animation
        local info
        local ammoRemoved = 0
        local claimed = false
        local target = {
            getID = function() return 9 end,
            getTHlevel = function() return th end,
            setTHlevel = function(_, value)
                th = value
                table.insert(updates, value)
            end,
            getMod = function(_, mod)
                if mod == xi.mod.TREASURE_HUNTER_PROC then
                    return params.targetProc or 0
                end
                return 0
            end,
            updateClaim = function(_, player)
                assert(player)
                claimed = true
            end,
        }
        local player = {
            getWeaponSkillType = function(_, slot)
                assert(slot == xi.slot.RANGED)
                return xi.skill.ARCHERY
            end,
            getMod = function(_, mod)
                if mod == xi.mod.BOUNTY_SHOT_TH_BONUS then
                    return params.bountyBonus or 0
                elseif mod == xi.mod.TREASURE_HUNTER then
                    return params.playerTH or 0
                elseif mod == xi.mod.TREASURE_HUNTER_PROC then
                    return params.playerProc or 0
                elseif mod == xi.mod.TREASURE_HUNTER_CAP then
                    return params.thCap or 0
                end
                return 0
            end,
            removeAmmo = function(_, count) ammoRemoved = ammoRemoved + count end,
        }
        local ability = { setMsg = function(_, value) message = value end }
        local action = {
            getAnimation = function() return 10 end,
            setAnimation = function(_, id, value)
                assert(id == 9)
                animation = value
            end,
            info = function(_, id, value) info = { id, value } end,
        }

        local result = xi.job_utils.ranger.useBountyShot(player, target, ability, action)
        return {
            result = result,
            th = th,
            updates = updates,
            message = message,
            animation = animation,
            info = info,
            ammoRemoved = ammoRemoved,
            claimed = claimed,
        }
    end

    it('guarantees the base increase and records its action side effects', function()
        local result = useBountyShot({ targetTH = 1 })

        assert(result.result == 2 and result.th == 2)
        assert(#result.updates == 1 and result.updates[1] == 2)
        assert(result.message == xi.msg.basic.JA_TH_EFFECTIVENESS)
        assert(result.animation == 9 and result.info[1] == 9 and result.info[2] == 1)
        assert(result.ammoRemoved == 1 and result.claimed)
    end)

    it('retains a gear pre-apply when its later proc does not occur', function()
        local random = math.random
        math.random = function() return 1 end
        local result = useBountyShot({ targetTH = 1, playerTH = 4 })
        math.random = random

        assert(result.result == 4 and result.th == 4)
        assert(#result.updates == 1 and result.updates[1] == 4)
        assert(result.message == xi.msg.basic.JA_TH_EFFECTIVENESS)
    end)

    it('uses the injected proc roll below the Treasure Hunter cap', function()
        local random = math.random
        math.random = function() return 0 end
        local result = useBountyShot({ targetTH = 2 })
        math.random = random

        assert(result.result == 3 and result.th == 3)
        assert(#result.updates == 1 and result.updates[1] == 3)
        assert(result.message == xi.msg.basic.JA_TH_EFFECTIVENESS)
    end)

    it('reports no effect without an update at the Treasure Hunter cap', function()
        local result = useBountyShot({ targetTH = 12 })

        assert(result.result == 0 and result.th == 12 and #result.updates == 0)
        assert(result.message == xi.msg.basic.JA_NO_EFFECT_2)
    end)
end)
