-----------------------------------
-- Pure system tests for skill level cap catalog.
-----------------------------------

describe('Skill level cap pure plans', function()
    it('rank enum pins A+ through G', function()
        assert(xi.skillRank.A_PLUS == 1)
        assert(xi.skillRank.G == 12)
    end)

    it('table corners at levels 0, 75, 99', function()
        assert(xi.data.skillLevel.getSkillCap(0, xi.skillRank.A_PLUS) == 3)
        assert(xi.data.skillLevel.getSkillCap(0, xi.skillRank.G) == 1)
        assert(xi.data.skillLevel.getSkillCap(75, xi.skillRank.A_PLUS) == 276)
        assert(xi.data.skillLevel.getSkillCap(75, xi.skillRank.E) == 200)
        assert(xi.data.skillLevel.getSkillCap(99, xi.skillRank.A_PLUS) == 424)
        assert(xi.data.skillLevel.getSkillCap(99, xi.skillRank.G) == 228)
        assert(xi.data.skillLevel.getSkillCap(50, xi.skillRank.B) == 147)
    end)

    it('master level adds clamp(level-99, 0, 156)', function()
        assert(xi.data.skillLevel.getSkillCap(100, xi.skillRank.A_PLUS) == 425)
        assert(xi.data.skillLevel.getSkillCap(99 + 156, xi.skillRank.A_PLUS) == 424 + 156)
        assert(xi.data.skillLevel.getSkillCap(99 + 200, xi.skillRank.A_PLUS) == 424 + 156)
    end)

    it('clamps level and rank into table bounds', function()
        -- level < 0 → 0
        assert(xi.data.skillLevel.getSkillCap(-5, xi.skillRank.A_PLUS) == 3)
        -- rank out of range clamps 1..12
        assert(xi.data.skillLevel.getSkillCap(99, 0) == xi.data.skillLevel.getSkillCap(99, xi.skillRank.A_PLUS))
        assert(xi.data.skillLevel.getSkillCap(99, 99) == xi.data.skillLevel.getSkillCap(99, xi.skillRank.G))
        -- default nil rank becomes G (sanitize defaultIfNil)
        assert(xi.data.skillLevel.getSkillCap(99, nil) == 228)
        -- default nil level becomes 0
        assert(xi.data.skillLevel.getSkillCap(nil, xi.skillRank.A_PLUS) == 3)
    end)

    it('A+ is never below G at same level', function()
        for lvl = 0, 99, 10 do
            assert(xi.data.skillLevel.getSkillCap(lvl, xi.skillRank.A_PLUS) >= xi.data.skillLevel.getSkillCap(lvl, xi.skillRank.G))
        end
    end)
end)
