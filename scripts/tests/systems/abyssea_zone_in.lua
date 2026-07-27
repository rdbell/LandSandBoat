require('scripts/globals/abyssea')

describe('Abyssea zone in', function()
    local function player(gmLevel, visibleGMLevel)
        local effects = {}

        return {
            getGMLevel = function()
                return gmLevel
            end,
            getVisibleGMLevel = function()
                return visibleGMLevel
            end,
            addStatusEffect = function(_, effect, options)
                table.insert(effects, { effect, options })
            end,
            effects = function()
                return effects
            end,
        }
    end

    it('grants Visitant to an active GM', function()
        local p = player(1, 3)
        xi.abyssea.onZoneIn(p)

        assert(#p.effects() == 1)
        assert(p.effects()[1][1] == xi.effect.VISITANT)
        assert(p.effects()[1][2].origin == p)
    end)

    it('does not grant Visitant to a GM without the active level', function()
        local p = player(1, 2)
        xi.abyssea.onZoneIn(p)

        assert(#p.effects() == 0)
    end)

    it('does not grant Visitant to a normal player', function()
        local p = player(0, 3)
        xi.abyssea.onZoneIn(p)

        assert(#p.effects() == 0)
    end)
end)
