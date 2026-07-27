require('scripts/globals/abyssea')

describe('Abyssea proc monster', function()
    local function mob(value)
        local triggered = nil
        local terror = nil
        return {
            getLocalVar = function() return value end,
            setLocalVar = function(_, _, v) value = v end,
            weaknessTrigger = function(_, v) triggered = v end,
            addStatusEffect = function(_, _, options) terror = options end,
            value = function() return value end,
            triggered = function() return triggered end,
            terror = function() return terror end,
        }
    end

    it('toggles red proc and uses a pet master as terror origin', function()
        local m = mob(0)
        local master = {}
        local p = { getAllegiance = function() return 1 end, getMaster = function() return master end }
        xi.abyssea.procMonster(m, p, xi.abyssea.triggerType.RED)
        assert(m.value() == 1 and m.triggered() == 2 and m.terror().duration == 30 and m.terror().origin == master)
    end)

    it('rejects non-allied players and unknown trigger types', function()
        local m = mob(0)
        xi.abyssea.procMonster(m, { getAllegiance = function() return 0 end }, xi.abyssea.triggerType.BLUE)
        assert(m.value() == 0 and m.triggered() == nil)
    end)
end)
