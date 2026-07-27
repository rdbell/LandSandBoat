require('scripts/globals/abyssea')

describe('Abyssea cruor prospector option', function()
    it('ignores unsupported option category bits', function()
        local p = { getCurrency = function() return 1000 end }
        assert(xi.abyssea.visionsCruorProspectorOnEventFinish(p, 2002, 7 + 16 * 65536, {}) == nil)
    end)
end)
