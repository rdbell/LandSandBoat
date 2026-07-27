require('scripts/globals/abyssea')

describe('Abyssea cruor prospector trigger', function()
    it('starts event 2002 with cruor, demilune mask, and summer cipher flag', function()
        local started
        local p = {
            getCurrency = function() return 123 end,
            hasKeyItem = function() return false end,
            startEvent = function(_, ...) started = { ... } end,
        }
        xi.abyssea.visionsCruorProspectorOnTrigger(p, {})
        assert(started[1] == 2002 and started[2] == 123 and started[3] == 0)
    end)
end)
