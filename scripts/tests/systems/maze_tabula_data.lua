-----------------------------------
-- Pure system tests for xi.data.maze tabula/shape/rune catalogs.
-----------------------------------

describe('maze tabula data pure catalogs', function()
    it('tabula M01 has blocked cells at known positions', function()
        local g = xi.data.maze.tabulaGrid[xi.item.MAZE_TABULA_M01]
        assert(g[2][2] == false) -- 1-based row2 col2 blocked
        assert(g[1][1] == 0)
        assert(g[4][4] == false)
    end)

    it('tabula M02 places fire and thunder elements', function()
        local g = xi.data.maze.tabulaGrid[xi.item.MAZE_TABULA_M02]
        assert(g[2][2] == xi.element.FIRE)
        assert(g[2][4] == xi.element.THUNDER)
        assert(g[4][2] == xi.element.WIND)
        assert(g[4][4] == xi.element.LIGHT)
    end)

    it('M03 and R03 share the same layout', function()
        local m = xi.data.maze.tabulaGrid[xi.item.MAZE_TABULA_M03]
        local r = xi.data.maze.tabulaGrid[xi.item.MAZE_TABULA_R03]
        for i = 1, 5 do
            for j = 1, 5 do
                assert(m[i][j] == r[i][j])
            end
        end
    end)

    it('runeShapeData DOMINO and PLUS masks', function()
        local d = xi.data.maze.runeShapeData[xi.maze.runeShape.DOMINO]
        assert(d[1] == 0x8800 and d[2] == 0xC000 and d[3] == 0x8800 and d[4] == 0xC000)
        local p = xi.data.maze.runeShapeData[xi.maze.runeShape.PLUS]
        assert(p[1] == 0x4E40 and p[2] == 0x4E40)
    end)

    it('runeInfo maps FIRE and AQUAN', function()
        local fire = xi.data.maze.runeInfo[xi.maze.rune.FIRE]
        assert(fire.shape == xi.maze.runeShape.CORNER)
        assert(fire.element == xi.element.FIRE)
        local aquan = xi.data.maze.runeInfo[xi.maze.rune.AQUAN]
        assert(aquan.shape == xi.maze.runeShape.DOMINO)
        assert(aquan.element == xi.element.NONE)
    end)
end)
