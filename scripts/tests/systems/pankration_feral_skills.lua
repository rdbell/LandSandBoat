-----------------------------------
-- Pure system tests for xi.data.pankration.feralSkills catalog.
-----------------------------------

describe('pankration feralSkills pure catalog', function()
    it('MAIN_JOB_WARRIOR has fp 8 and cannot level', function()
        local e = xi.data.pankration.feralSkills[xi.pankration.feralSkill.MAIN_JOB_WARRIOR]
        assert(e.fp == 8)
        assert(e.canLevel == false)
    end)

    it('INFLICT_AMNESIA is uncaptured fp 99', function()
        local e = xi.data.pankration.feralSkills[xi.pankration.feralSkill.INFLICT_AMNESIA]
        assert(e.fp == 99)
        assert(e.canLevel == false)
    end)

    it('equippable filter is 0 < fp < 99', function()
        local equippable = 0
        local total = 0
        for _, e in pairs(xi.data.pankration.feralSkills) do
            total = total + 1
            if e.fp > 0 and e.fp < 99 then
                equippable = equippable + 1
            end
        end
        assert(total == 1629)
        assert(equippable > 0)
        assert(equippable < total)
    end)
end)
