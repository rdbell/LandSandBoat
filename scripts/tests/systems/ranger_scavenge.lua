require('scripts/globals/job_utils/ranger')

describe('Ranger Scavenge', function()
    local function useScavenge(params)
        local added
        local cleared
        local messages = {}
        local additional
        local effectParam
        local oldQuestItem = xi.job_utils.ranger.tryScavengeQuestItem
        xi.job_utils.ranger.tryScavengeQuestItem = function() return params.questItem or false end
        local player = {
            getLocalVar = function() return params.arrowsUsed or 0 end,
            getMainLvl = function() return params.mainLevel or 0 end,
            getMod = function() return params.scavengeMod or 0 end,
            getMerit = function() return params.scavengeMerit or 0 end,
            addItem = function(_, item, count) added = { item, count } end,
            setLocalVar = function(_, name, value) cleared = { name, value } end,
        }
        local target = { getID = function() return 9 end }
        local action = {
            messageID = function(_, id, message) table.insert(messages, { id, message }) end,
            additionalEffect = function(_, id, value) additional = { id, value } end,
            addEffectParam = function(_, id, value) effectParam = { id, value } end,
        }
        local result = xi.job_utils.ranger.useScavenge(player, target, {}, action)
        xi.job_utils.ranger.tryScavengeQuestItem = oldQuestItem
        return { result = result, added = added, cleared = cleared, messages = messages, additional = additional, effectParam = effectParam }
    end

    it('short-circuits after the quest item and leaves recovery state untouched', function()
        local result = useScavenge({ questItem = true, arrowsUsed = 1113 * 10000 + 100, mainLevel = 100 })
        assert(result.result == nil and result.added == nil and result.cleared == nil and #result.messages == 0)
    end)

    it('reports no recovery without clearing arrows used', function()
        local result = useScavenge({ arrowsUsed = 1113 * 10000 + 1 })
        assert(result.result == nil and result.added == nil and result.cleared == nil)
        assert(#result.messages == 1 and result.messages[1][1] == 9 and result.messages[1][2] == xi.msg.basic.SCAVENGE_FIND_NOTHING)
    end)

    it('returns one arrow and clears the local variable without an additional effect', function()
        local result = useScavenge({ arrowsUsed = 1113 * 10000 + 2, mainLevel = 100 })
        assert(result.result == 1113 and result.added[1] == 1113 and result.added[2] == 1)
        assert(result.cleared[1] == 'ArrowsUsed' and result.cleared[2] == 0 and result.additional == nil and result.effectParam == nil)
        assert(result.messages[1][2] == xi.msg.basic.SCAVENGE_FIND_ITEM)
    end)

    it('adds the plural recovery effect and count', function()
        local result = useScavenge({ arrowsUsed = 1113 * 10000 + 4, mainLevel = 100 })
        assert(result.result == 1113 and result.added[2] == 2 and result.messages[1][2] == xi.msg.basic.SCAVENGE_FIND_ITEMS)
        assert(result.additional[1] == 9 and result.additional[2] == 1 and result.effectParam[1] == 9 and result.effectParam[2] == 2)
    end)
end)
