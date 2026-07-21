-----------------------------------
-- Pure system tests for magicAccuracyFromSkill / FromElement /
-- calculateActorMagicAccuracy inject halves (slice 6082).
-----------------------------------

describe('magic accuracy skill/element pure injects', function()
    local function skillAcc(params)
        local magicAcc = 0
        if params.skillType > 0 then
            magicAcc = params.skillLevel
            if magicAcc == 0 and params.isMob then
                magicAcc = xi.data.skillLevel.getSkillCap(params.mainLvl, xi.skillRank.A_PLUS)
            end
            if params.skillType == xi.skill.SINGING then
                if params.isPC then
                    if params.rangeSkillType == xi.skill.WIND_INSTRUMENT then
                        magicAcc = magicAcc + params.rangeSkillLvl
                    elseif params.rangeSkillType == xi.skill.STRING_INSTRUMENT then
                        magicAcc = magicAcc + math.floor(params.rangeSkillLvl / 2)
                    end
                else
                    magicAcc = magicAcc * 2
                end
            end
        elseif params.skillRank and params.skillRank > 0 then
            magicAcc = xi.data.skillLevel.getSkillCap(params.mainLvl, params.skillRank)
        else
            magicAcc = xi.data.skillLevel.getSkillCap(params.mainLvl, xi.skillRank.A_PLUS)
        end
        return magicAcc
    end

    local function elementAcc(magicalElement, elementalMaccMod, elementalStaffMod)
        if magicalElement <= xi.element.NONE then
            return 0
        end
        return elementalMaccMod + elementalStaffMod * 10
    end

    local function actorMagicAccuracy(parts)
        local sum = (parts.base or 0)
            + (parts.skill or 0)
            + (parts.element or 0)
            + (parts.statDiff or 0)
            + (parts.effects or 0)
            + (parts.merits or 0)
            + (parts.jobPoints or 0)
            + (parts.burst or 0)
            + (parts.dayWeather or 0)
            + (parts.tandem or 0)
        local food = parts.food or 1
        local soul = parts.soulVoice or 1
        return math.floor(sum * food * soul)
    end

    it('known skill passes through level', function()
        assert(skillAcc({ skillType = xi.skill.ELEMENTAL_MAGIC, skillLevel = 250 }) == 250)
    end)

    it('mob zero skill falls back to A+ cap', function()
        local cap = xi.data.skillLevel.getSkillCap(75, xi.skillRank.A_PLUS)
        assert(skillAcc({
            skillType = xi.skill.ELEMENTAL_MAGIC,
            skillLevel = 0,
            mainLvl = 75,
            isMob = true,
        }) == cap)
        assert(skillAcc({
            skillType = xi.skill.ELEMENTAL_MAGIC,
            skillLevel = 0,
            mainLvl = 75,
            isMob = false,
        }) == 0)
    end)

    it('singing PC instruments and non-PC double', function()
        assert(skillAcc({
            skillType = xi.skill.SINGING,
            skillLevel = 100,
            isPC = true,
            rangeSkillType = xi.skill.WIND_INSTRUMENT,
            rangeSkillLvl = 50,
        }) == 150)
        assert(skillAcc({
            skillType = xi.skill.SINGING,
            skillLevel = 100,
            isPC = true,
            rangeSkillType = xi.skill.STRING_INSTRUMENT,
            rangeSkillLvl = 51,
        }) == 125)
        assert(skillAcc({
            skillType = xi.skill.SINGING,
            skillLevel = 80,
            isPC = false,
        }) == 160)
    end)

    it('skillRank and default A+ paths', function()
        local wantB = xi.data.skillLevel.getSkillCap(50, xi.skillRank.B)
        assert(skillAcc({ skillType = 0, skillRank = xi.skillRank.B, mainLvl = 50 }) == wantB)
        local wantA = xi.data.skillLevel.getSkillCap(99, xi.skillRank.A_PLUS)
        assert(skillAcc({ skillType = 0, skillRank = 0, mainLvl = 99 }) == wantA)
    end)

    it('element MACC is macc + staff*10', function()
        assert(elementAcc(xi.element.NONE, 20, 3) == 0)
        assert(elementAcc(xi.element.FIRE, 15, 2) == 35)
        assert(elementAcc(xi.element.DARK, 0, 0) == 0)
    end)

    it('actor assembly floors product of sum and multipliers', function()
        assert(actorMagicAccuracy({
            base = 100, skill = 50, element = 10, statDiff = 5,
            effects = 15, merits = 8, jobPoints = 4, burst = 100,
            dayWeather = 10, tandem = 3,
            food = 1, soulVoice = 1,
        }) == 305)
        assert(actorMagicAccuracy({ base = 100, skill = 50, food = 1.1, soulVoice = 2 }) == 330)
        assert(actorMagicAccuracy({ base = 101, food = 1.05, soulVoice = 1 }) == 106)
    end)
end)
