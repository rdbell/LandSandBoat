-----------------------------------
-- Pure system tests for magic AoE dual-wire helpers (slice 6696).
-- Calls production xi.combat.magicAoE pure exports.
-- Goldens match internal/magicaoe (0849 / 1581).
-----------------------------------

require('scripts/globals/combat/magic_aoe')

local aoe = xi.combat.magicAoE
local ma = xi.magic.aoe
local fam = xi.magic.spellFamily
local grp = xi.magic.spellGroup

describe('magic AoE constants', function()
    it('pins override radius and AOE ordinals', function()
        assert(aoe.overrideRadius == 10)
        assert(ma.NONE == 0 and ma.RADIAL == 1 and ma.CONAL == 2)
        assert(ma.RADIAL_MANI == 3 and ma.RADIAL_ACCE == 4)
        assert(ma.PIANISSIMO == 5 and ma.DIFFUSION == 6)
        assert(fam.CURE == 1 and fam.NA == 4 and fam.PROTECT == 10)
        assert(fam.UTSUSEMI == 96 and fam.FIRA == 148 and fam.WATERA == 153)
        assert(xi.magic.spell.ERASE == 143)
        assert(grp.SONG == 1 and grp.BLUE == 3)
        assert(xi.job.BRD == 10 and xi.skill.STRING_INSTRUMENT == 41)
    end)
end)

describe('songRadiusFromParams', function()
    it('returns 0 for NONE or Pianissimo+effect', function()
        assert(aoe.songRadiusFromParams({ baseType = ma.NONE, baseRadius = 10 }) == 0)
        assert(aoe.songRadiusFromParams({
            baseType = ma.PIANISSIMO, baseRadius = 10, hasPianissimo = true,
        }) == 0)
    end)

    it('returns base when non-BRD / non-string / non-song', function()
        local base = {
            baseType = ma.RADIAL, baseRadius = 12, mainJob = xi.job.BRD,
            rangedSkillType = xi.skill.STRING_INSTRUMENT, spellGroup = grp.SONG,
            stringSkill = 300, skillCap = 150,
        }
        local p = {}
        for k, v in pairs(base) do p[k] = v end
        p.mainJob = xi.job.WAR
        assert(aoe.songRadiusFromParams(p) == 12)

        p = {}
        for k, v in pairs(base) do p[k] = v end
        p.rangedSkillType = 25
        assert(aoe.songRadiusFromParams(p) == 12)

        p = {}
        for k, v in pairs(base) do p[k] = v end
        p.spellGroup = grp.BLUE
        assert(aoe.songRadiusFromParams(p) == 12)
    end)

    it('clamps string skill multiplier 1.0..2.0 and floors', function()
        local mk = function(skill, cap)
            return {
                baseType = ma.RADIAL, baseRadius = 10, mainJob = xi.job.BRD,
                rangedSkillType = xi.skill.STRING_INSTRUMENT, spellGroup = grp.SONG,
                stringSkill = skill, skillCap = cap,
            }
        end
        assert(aoe.songRadiusFromParams(mk(100, 100)) == 10)
        assert(aoe.songRadiusFromParams(mk(50, 100)) == 10)
        assert(aoe.songRadiusFromParams(mk(150, 100)) == 15)
        assert(aoe.songRadiusFromParams(mk(300, 100)) == 20)
        assert(aoe.songRadiusFromParams(mk(125, 100)) == 12) -- floor 12.5
        assert(aoe.songRadiusFromParams(mk(100, 0)) == 10)   -- skillCap 0 defensive
    end)
end)

describe('mobAoEFromParams', function()
    it('collapses stratagem/diffusion and promotes Pianissimo', function()
        local got = aoe.mobAoEFromParams({ baseType = ma.RADIAL_MANI, baseRadius = 10 })
        assert(got[1] == ma.NONE and got[2] == 0)
        got = aoe.mobAoEFromParams({ baseType = ma.RADIAL_ACCE, baseRadius = 10 })
        assert(got[1] == ma.NONE and got[2] == 0)
        got = aoe.mobAoEFromParams({ baseType = ma.DIFFUSION, baseRadius = 10 })
        assert(got[1] == ma.NONE and got[2] == 0)
        got = aoe.mobAoEFromParams({ baseType = ma.PIANISSIMO, baseRadius = 7 })
        assert(got[1] == ma.RADIAL and got[2] == 7)
        got = aoe.mobAoEFromParams({ baseType = ma.RADIAL, baseRadius = 5 })
        assert(got[1] == ma.RADIAL and got[2] == 5)
        got = aoe.mobAoEFromParams({ baseType = ma.CONAL, baseRadius = 8 })
        assert(got[1] == ma.CONAL and got[2] == 8)
    end)
end)

describe('calculateTypeAndRadiusFromParams', function()
    it('uses mob path for non-PC non-Trust', function()
        local got = aoe.calculateTypeAndRadiusFromParams({
            baseType = ma.RADIAL_MANI, baseRadius = 10,
            isPC = false, isTrust = false, hasManifestation = true,
        })
        assert(got[1] == ma.NONE and got[2] == 0 and not got[3])
    end)

    it('applies Majesty on Cure/Protect', function()
        local got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, hasMajesty = true, spellFamily = fam.CURE,
            baseType = ma.NONE, baseRadius = 0,
        })
        assert(got[1] == ma.RADIAL and got[2] == 10)

        got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, hasMajesty = true, spellFamily = fam.PROTECT,
            baseType = ma.NONE, baseRadius = 0,
        })
        assert(got[1] == ma.RADIAL and got[2] == 10)

        got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, hasMajesty = true, spellFamily = fam.NA,
            baseType = ma.NONE, baseRadius = 0,
        })
        assert(got[1] == ma.NONE and got[2] == 0)
    end)

    it('handles Accession and Divine Veil on RADIAL_ACCE', function()
        local got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, baseType = ma.RADIAL_ACCE, hasAccession = true,
        })
        assert(got[1] == ma.RADIAL and got[2] == 10)

        got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, baseType = ma.RADIAL_ACCE, spellFamily = fam.NA, divineVeilProc = true,
        })
        assert(got[1] == ma.RADIAL and got[2] == 10)

        got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, baseType = ma.RADIAL_ACCE, spellID = xi.magic.spell.ERASE, divineVeilProc = true,
        })
        assert(got[1] == ma.RADIAL and got[2] == 10)

        got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, baseType = ma.RADIAL_ACCE, spellFamily = fam.NA, divineVeilProc = false,
        })
        assert(got[1] == ma.NONE and got[2] == 0)
    end)

    it('applies Manifestation, Diffusion, Theurgic Focus', function()
        local got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, baseType = ma.RADIAL_MANI, hasManifestation = true,
        })
        assert(got[1] == ma.RADIAL and got[2] == 10)

        got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, baseType = ma.DIFFUSION, hasDiffusion = true,
        })
        assert(got[1] == ma.RADIAL and got[2] == 10)

        got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, hasTheurgicFocus = true, spellFamily = fam.FIRA,
            baseType = ma.RADIAL, baseRadius = 12,
        })
        assert(got[1] == ma.RADIAL and got[2] == 6)

        got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, hasTheurgicFocus = true, spellFamily = fam.WATERA,
            baseType = ma.RADIAL, baseRadius = 11,
        })
        assert(got[1] == ma.RADIAL and got[2] == 5)
    end)

    it('handles songs including Pianissimo consume flag', function()
        local got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, spellGroup = grp.SONG, baseType = ma.NONE, baseRadius = 0,
        })
        assert(got[1] == ma.NONE and got[2] == 0 and not got[3])

        got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, spellGroup = grp.SONG, baseType = ma.PIANISSIMO,
            baseRadius = 10, hasPianissimo = true,
        })
        assert(got[1] == ma.NONE and got[2] == 0 and got[3] == true)

        got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, spellGroup = grp.SONG, baseType = ma.RADIAL, baseRadius = 10,
            mainJob = xi.job.BRD, rangedSkillType = xi.skill.STRING_INSTRUMENT,
            stringSkill = 200, skillCap = 100,
        })
        assert(got[1] == ma.RADIAL and got[2] == 20 and not got[3])
    end)

    it('applies Convergence, Utsusemi AOE, and passthrough', function()
        local got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, hasConvergence = true, spellGroup = grp.BLUE, element = 1,
            baseType = ma.RADIAL, baseRadius = 10,
        })
        assert(got[1] == ma.NONE and got[2] == 0)

        got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, hasConvergence = true, spellGroup = grp.BLUE, element = 0,
            baseType = ma.RADIAL, baseRadius = 10,
        })
        assert(got[1] == ma.RADIAL and got[2] == 10)

        got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, spellFamily = fam.UTSUSEMI, utsusemiAOEMod = 1,
            baseType = ma.NONE, baseRadius = 0,
        })
        assert(got[1] == ma.RADIAL and got[2] == 10)

        got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, baseType = ma.CONAL, baseRadius = 8,
        })
        assert(got[1] == ma.CONAL and got[2] == 8 and not got[3])
    end)

    it('prioritizes Majesty before Accession on Cure RADIAL_ACCE', function()
        local got = aoe.calculateTypeAndRadiusFromParams({
            isPC = true, hasMajesty = true, hasAccession = false,
            spellFamily = fam.CURE, baseType = ma.RADIAL_ACCE, baseRadius = 0,
        })
        assert(got[1] == ma.RADIAL and got[2] == 10)
    end)
end)
