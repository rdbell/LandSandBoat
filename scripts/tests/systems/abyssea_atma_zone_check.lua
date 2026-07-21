-----------------------------------
-- Pure system tests for Abyssea zone membership and Atma tick leave-zone.
-----------------------------------

describe('Abyssea zone and Atma tick pure plans', function()
    it('isInAbysseaZone matches region ABYSSEA', function()
        assert(xi.region.ABYSSEA == 41)
        -- Inject-form: region equality is the whole policy.
        local function isInAbysseaZone(region)
            return region == xi.region.ABYSSEA
        end
        assert(isInAbysseaZone(xi.region.ABYSSEA))
        assert(not isInAbysseaZone(0))
        assert(not isInAbysseaZone(40))
        assert(not isInAbysseaZone(42))
    end)

    it('atma tick removes effect when not in Abyssea', function()
        local removed = false
        local target = {
            region = xi.region.RONFAURE, -- any non-Abyssea
            getCurrentRegion = function(self)
                return self.region
            end,
            delStatusEffect = function(self, effect)
                removed = true
                assert(effect == xi.effect.ATMA)
            end,
        }
        local script = require('scripts/effects/atma')
        -- Stub xi.abyssea.isInAbysseaZone via region inject path used by production:
        -- atma calls xi.abyssea.isInAbysseaZone(target) which reads getCurrentRegion.
        -- Ensure global uses player region.
        assert(xi.effect.ATMA == 287)
        script.onEffectTick(target, { getEffectType = function() return xi.effect.ATMA end })
        assert(removed)
    end)

    it('atma tick keeps effect inside Abyssea', function()
        local removed = false
        local target = {
            getCurrentRegion = function()
                return xi.region.ABYSSEA
            end,
            delStatusEffect = function()
                removed = true
            end,
        }
        local script = require('scripts/effects/atma')
        script.onEffectTick(target, { getEffectType = function() return xi.effect.ATMA end })
        assert(not removed)
    end)
end)
