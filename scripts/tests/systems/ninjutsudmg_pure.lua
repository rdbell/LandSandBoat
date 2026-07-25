-----------------------------------
-- Pure system tests for ninjutsu damage mult dual-wire (slice 6712).
-- Calls production xi.spells.damage pure exports.
-- Goldens match internal/ninjutsudmg (0856).
-----------------------------------

require('scripts/globals/spells/damage_spell')

local dmg = xi.spells.damage
local skill = xi.skill
local job = xi.job

local function almost(a, b)
    return math.abs(a - b) < 1e-12
end

describe('Ninjutsu dmg constants and spell tier', function()
    it('pins skill caps and futae base', function()
        assert(dmg.ninSkillCaps[1][1] == 50 and dmg.ninSkillCaps[1][2] == 250)
        assert(dmg.ninSkillCaps[2][1] == 125 and dmg.ninSkillCaps[2][2] == 350)
        assert(dmg.ninSkillCaps[3][1] == 275 and dmg.ninSkillCaps[3][2] == 500)
        assert(almost(dmg.ninFutaeBase, 1.5))
    end)

    it('maps spell id to Ichi/Ni/San tier', function()
        -- Ichi: %3 == 2
        assert(dmg.ninSpellTier(320) == 1)
        assert(dmg.ninSpellTier(323) == 1)
        assert(dmg.ninSpellTier(335) == 1)
        -- Ni: %3 == 0
        assert(dmg.ninSpellTier(321) == 2)
        assert(dmg.ninSpellTier(324) == 2)
        -- San: %3 == 1
        assert(dmg.ninSpellTier(322) == 3)
        assert(dmg.ninSpellTier(325) == 3)
        assert(dmg.ninSpellTier(0) == 2)
        assert(dmg.ninSpellTier(1) == 3)
        assert(dmg.ninSpellTier(2) == 1)
    end)
end)

describe('calculateNinSkillBonusFromParams', function()
    it('gates non-NIN and non-ninjutsu skill', function()
        assert(dmg.calculateNinSkillBonusFromParams({
            mainJob = job.BLM, skillType = skill.NINJUTSU, spellId = 320, ninjutsuSkill = 250,
        }) == 1)
        assert(dmg.calculateNinSkillBonusFromParams({
            mainJob = job.NIN, skillType = skill.ELEMENTAL_MAGIC, spellId = 320, ninjutsuSkill = 250,
        }) == 1)
    end)

    it('Ichi ladder clamp and scale', function()
        assert(almost(dmg.calculateNinSkillBonusFromParams({
            mainJob = job.NIN, skillType = skill.NINJUTSU, spellId = 320, ninjutsuSkill = 0,
        }), 1.0))
        assert(almost(dmg.calculateNinSkillBonusFromParams({
            mainJob = job.NIN, skillType = skill.NINJUTSU, spellId = 320, ninjutsuSkill = 50,
        }), 1.0))
        assert(almost(dmg.calculateNinSkillBonusFromParams({
            mainJob = job.NIN, skillType = skill.NINJUTSU, spellId = 320, ninjutsuSkill = 150,
        }), 1.5))
        assert(almost(dmg.calculateNinSkillBonusFromParams({
            mainJob = job.NIN, skillType = skill.NINJUTSU, spellId = 320, ninjutsuSkill = 250,
        }), 2.0))
        assert(almost(dmg.calculateNinSkillBonusFromParams({
            mainJob = job.NIN, skillType = skill.NINJUTSU, spellId = 320, ninjutsuSkill = 999,
        }), 2.0))
    end)

    it('Ni and San caps', function()
        assert(almost(dmg.calculateNinSkillBonusFromParams({
            mainJob = job.NIN, skillType = skill.NINJUTSU, spellId = 321, ninjutsuSkill = 0,
        }), 1.0))
        assert(almost(dmg.calculateNinSkillBonusFromParams({
            mainJob = job.NIN, skillType = skill.NINJUTSU, spellId = 321, ninjutsuSkill = 350,
        }), 1 + 225 / 200))
        assert(almost(dmg.calculateNinSkillBonusFromParams({
            mainJob = job.NIN, skillType = skill.NINJUTSU, spellId = 322, ninjutsuSkill = 275,
        }), 1.0))
        assert(almost(dmg.calculateNinSkillBonusFromParams({
            mainJob = job.NIN, skillType = skill.NINJUTSU, spellId = 322, ninjutsuSkill = 375,
        }), 1.5))
    end)
end)

describe('calculateNinFutaeBonusFromParams', function()
    it('gates status and skill; consume on apply', function()
        local m, c = dmg.calculateNinFutaeBonusFromParams({})
        assert(m == 1 and not c)
        m, c = dmg.calculateNinFutaeBonusFromParams({
            hasFutae = true, skillType = skill.ELEMENTAL_MAGIC,
        })
        assert(m == 1 and not c)
        m, c = dmg.calculateNinFutaeBonusFromParams({
            hasFutae = true, skillType = skill.NINJUTSU,
        })
        assert(almost(m, 1.5) and c)
        m, c = dmg.calculateNinFutaeBonusFromParams({
            hasFutae = true, skillType = skill.NINJUTSU,
            enhancesFutaeMod = 20, futaeJP = 10,
        })
        -- 1.5 + 0.20 + 0.50 = 2.20
        assert(almost(m, 2.20) and c)
    end)
end)

describe('calculateNinjutsuMultiplierFromParams', function()
    it('Innin behind ninjutsu only', function()
        assert(dmg.calculateNinjutsuMultiplierFromParams({}) == 1)
        assert(dmg.calculateNinjutsuMultiplierFromParams({
            hasInnin = true, isBehind = false, skillType = skill.NINJUTSU, ninNukeBonusInnin = 30,
        }) == 1)
        assert(dmg.calculateNinjutsuMultiplierFromParams({
            hasInnin = true, isBehind = true, skillType = skill.ELEMENTAL_MAGIC, ninNukeBonusInnin = 30,
        }) == 1)
        assert(almost(dmg.calculateNinjutsuMultiplierFromParams({
            hasInnin = true, isBehind = true, skillType = skill.NINJUTSU, ninNukeBonusInnin = 30,
        }), 1.30))
        assert(almost(dmg.calculateNinjutsuMultiplierFromParams({
            hasInnin = true, isBehind = true, skillType = skill.NINJUTSU, ninNukeBonusInnin = 0,
        }), 1.0))
    end)
end)
