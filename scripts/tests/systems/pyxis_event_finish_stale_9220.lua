require('scripts/globals/abyssea/sturdypyxis/npc')

describe('Pyxis event finish stale-chest guard (slice 9220)', function()
    it('rejects a nonzero option for a non-spawned chest', function()
        assert(xi.pyxis.npc.shouldDespawnStale(1, 0))
        assert(xi.pyxis.npc.shouldDespawnStale(4294967295, 0))
    end)

    it('does not reject option zero or a spawned chest', function()
        assert(not xi.pyxis.npc.shouldDespawnStale(0, 0))
        assert(not xi.pyxis.npc.shouldDespawnStale(999, 1))
    end)
end)
