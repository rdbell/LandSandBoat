require('scripts/globals/abyssea')

describe('Abyssea after zone in', function()
    local function player(hasVisitant, icon)
        local added = {}
        local messages = {}

        return {
            getZoneID = function()
                return xi.zone.ABYSSEA_ALTEPA
            end,
            hasStatusEffect = function()
                return hasVisitant
            end,
            addStatusEffect = function(_, effect, options)
                table.insert(added, { effect, options })
            end,
            getStatusEffect = function()
                return icon and { getIcon = function() return icon end } or nil
            end,
            messageName = function(_, ...)
                table.insert(messages, { ... })
            end,
            added = function() return added end,
            messages = function() return messages end,
        }
    end

    it('adds hidden Visitant then announces it', function()
        local p = player(false, 0)
        xi.abyssea.afterZoneIn(p)
        assert(#p.added() == 1 and p.added()[1][1] == xi.effect.VISITANT)
        assert(p.added()[1][2].duration == 304 and p.added()[1][2].tick == 3 and p.added()[1][2].icon == 0)
        assert(p.messages()[1][1] == 7352 and p.messages()[1][3] == 5)
    end)

    it('does not add or announce a regular Visitant effect', function()
        local p = player(true, 1)
        xi.abyssea.afterZoneIn(p)
        assert(#p.added() == 0 and #p.messages() == 0)
    end)

    it('does not announce when the effect cannot be found', function()
        local p = player(true, nil)
        xi.abyssea.afterZoneIn(p)
        assert(#p.added() == 0 and #p.messages() == 0)
    end)
end)
