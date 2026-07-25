-----------------------------------
-- Pure system tests for Avatars Favor dual-wire (slice 6725).
-- Calls production xi.avatarsFavor pure exports.
-- Goldens match internal/avatarsfavor (0874).
-----------------------------------

require('scripts/globals/avatars_favor')

local a = xi.avatarsFavor

describe('Avatars Favor pins', function()
    it('skill levels bounds and catalog size', function()
        assert(a.skillLevels[1] == 316)
        assert(a.skillLevels[2] == 381)
        assert(a.skillLevels[3] == 446)
        assert(a.skillLevels[4] == 511)
        assert(a.skillLevels[5] == 574)
        assert(a.skillLevels[6] == 669)
        assert(a.skillLevels[7] == 670)
        assert(a.maxSkillRank == 7)
        assert(a.maxTickPower == 11)
        assert(a.overSkillCap == 670)
        assert(a.catalogSize() == 11)
    end)
end)

describe('shouldAvatarsFavorBeApplied', function()
    it('core avatars and extended table', function()
        for id = xi.petId.CARBUNCLE, xi.petId.DIABOLOS do
            assert(a.shouldAvatarsFavorBeApplied(id))
        end

        assert(a.shouldAvatarsFavorBeApplied(xi.petId.CAIT_SITH))
        assert(a.shouldAvatarsFavorBeApplied(xi.petId.SIREN))
    end)

    it('rejects spirits gods jugs', function()
        for _, id in ipairs({ 0, 1, 7, 17, 18, 19, 21, 48, 69, 75, 77, 127, 255 }) do
            assert(not a.shouldAvatarsFavorBeApplied(id))
        end
    end)
end)

describe('lookupAvatarsFavor / scalingFromParams', function()
    it('returns effect and ladder cells', function()
        local r = a.lookupAvatarsFavor(xi.petId.CARBUNCLE)
        assert(r and r.effect == xi.effect.CARBUNCLES_FAVOR)
        assert(a.scalingFromParams({ petId = xi.petId.CARBUNCLE, power = 1 }) == 12)
        assert(a.scalingFromParams({ petId = xi.petId.CARBUNCLE, power = 7 }) == 24)
        assert(a.scalingFromParams({ petId = xi.petId.CARBUNCLE, power = 11 }) == 29)

        assert(a.scalingFromParams({ petId = xi.petId.TITAN, power = 11 }) == 107)
        assert(a.scalingFromParams({ petId = xi.petId.SIREN, power = 10 }) == 23)
        assert(a.scalingFromParams({ petId = xi.petId.SIREN, power = 11 }) == nil)
        assert(a.scalingFromParams({ petId = xi.petId.CARBUNCLE, power = 0 }) == nil)
        assert(a.scalingFromParams({ petId = 17, power = 1 }) == nil)
        assert(a.lookupAvatarsFavor(17) == nil)
    end)

    it('exhaustive published ladders', function()
        local want =
        {
            [xi.petId.CARBUNCLE] = { 12, 14, 16, 18, 20, 21, 24, 26, 27, 28, 29 },
            [xi.petId.FENRIR]    = { 1, 2, 3, 4, 5, 7, 9, 12, 15, 18, 21 },
            [xi.petId.IFRIT]     = { 12, 12, 15, 15, 23, 23, 24, 24, 25, 25, 26 },
            [xi.petId.TITAN]     = { 57, 62, 67, 72, 77, 82, 87, 92, 97, 102, 107 },
            [xi.petId.LEVIATHAN] = { 1, 2, 3, 4, 5, 7, 9, 12, 15, 18, 21 },
            [xi.petId.GARUDA]    = { 12, 15, 18, 22, 25, 28, 31, 34, 37, 40, 43 },
            [xi.petId.SHIVA]     = { 15, 18, 21, 24, 27, 30, 33, 36, 39, 42, 45 },
            [xi.petId.RAMUH]     = { 12, 14, 16, 18, 19, 21, 21, 23, 23, 24, 24 },
            [xi.petId.DIABOLOS]  = { 3, 4, 4, 5, 5, 5, 6, 7, 7, 8, 8 },
            [xi.petId.CAIT_SITH] = { 10, 12, 14, 16, 18, 20, 22, 24, 26, 27, 28 },
            [xi.petId.SIREN]     = { 9, 11, 13, 15, 17, 19, 20, 21, 22, 23 },
        }

        for petId, ladder in pairs(want) do
            for i, w in ipairs(ladder) do
                assert(a.scalingFromParams({ petId = petId, power = i }) == w)
            end
        end
    end)
end)

describe('clampPowerToSkillFromParams', function()
    it('skill rank caps', function()
        assert(a.clampPowerToSkillFromParams({ power = 5, summoningSkill = 316 }) == 1)
        assert(a.clampPowerToSkillFromParams({ power = 5, summoningSkill = 317 }) == 2)
        assert(a.clampPowerToSkillFromParams({ power = 10, summoningSkill = 446 }) == 3)
        assert(a.clampPowerToSkillFromParams({ power = 9, summoningSkill = 400 }) == 3)
        assert(a.clampPowerToSkillFromParams({ power = 7, summoningSkill = 670 }) == 7)
        assert(a.clampPowerToSkillFromParams({ power = 11, summoningSkill = 670 }) == 7)
        assert(a.clampPowerToSkillFromParams({ power = 11, summoningSkill = 671 }) == 7)
        assert(a.clampPowerToSkillFromParams({ power = 12, summoningSkill = 999 }) == 7)
        assert(a.clampPowerToSkillFromParams({ power = 1, summoningSkill = 0 }) == 1)
        assert(a.clampPowerToSkillFromParams({ power = 4, summoningSkill = 500 }) == 4)
        assert(a.clampPowerToSkillFromParams({ power = 8, summoningSkill = 574 }) == 5)
        assert(a.clampPowerToSkillFromParams({ power = 8, summoningSkill = 575 }) == 6)
    end)
end)

describe('tickPowerFromParams', function()
    it('increment clamp and gear', function()
        -- Fresh effect power 1, low skill, no gear: +1 then clamp to 1.
        assert(a.tickPowerFromParams({ currentPower = 1, summoningSkill = 100, gearEnhance = 0 }) == 1)
        -- Power 1, skill allows rank 7: 1+1 = 2.
        assert(a.tickPowerFromParams({ currentPower = 1, summoningSkill = 999, gearEnhance = 0 }) == 2)

        local power = 1
        for _ = 1, 20 do
            power = a.tickPowerFromParams({ currentPower = power, summoningSkill = 999, gearEnhance = 0 })
        end
        assert(power == 7)

        -- current 7 → +1 → 8 → clamp 7 → +2 gear = 9
        assert(a.tickPowerFromParams({ currentPower = 7, summoningSkill = 999, gearEnhance = 2 }) == 9)
        -- 11 → +1 → 12 → clamp 7 → +4 = 11
        assert(a.tickPowerFromParams({ currentPower = 11, summoningSkill = 999, gearEnhance = 4 }) == 11)
        -- 12 no-inc → clamp 7
        assert(a.tickPowerFromParams({ currentPower = 12, summoningSkill = 999, gearEnhance = 0 }) == 7)
        -- mid skill gear: 3+1=4 → clamp 3 → +1 = 4
        assert(a.tickPowerFromParams({ currentPower = 3, summoningSkill = 400, gearEnhance = 1 }) == 4)
    end)
end)

describe('preSOADebuffs', function()
    it('MATT ATTP ACC DEFP magnitudes', function()
        local matt, attp, acc, defp = a.preSOADebuffs()
        assert(matt == 20 and attp == 20 and acc == 10 and defp == 10)
    end)
end)
