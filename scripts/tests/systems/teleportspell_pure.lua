-----------------------------------
-- Pure system tests for teleport spell dual-wire (slice 6722).
-- Calls production xi.spells.enhancing pure exports.
-- Goldens match internal/teleportspell (0884).
-----------------------------------

require('scripts/globals/spells/enhancing_teleport')

local e = xi.spells.enhancing

describe('Teleport spell constants', function()
    it('pins messages checks effect and catalog size', function()
        assert(e.teleportMsgMagicTeleport == 93)
        assert(e.teleportMsgNone == 0)
        assert(e.teleportCheckMsgOK == 0)
        assert(e.teleportCheckMsgCannot == 48)
        assert(e.teleportEffectID == 797)
        assert(e.teleportSpellCatalogSize() == 13)
    end)
end)

describe('lookupTeleportSpell', function()
    it('returns known catalog rows', function()
        local r = e.lookupTeleportSpell(xi.magic.spell.ESCAPE)
        assert(r and r.teleportId == xi.teleport.id.ESCAPE and r.keyItem == 0 and r.duration == 4 and not r.campaign)

        r = e.lookupTeleportSpell(xi.magic.spell.WARP)
        assert(r and r.teleportId == xi.teleport.id.WARP and r.duration == 3)

        r = e.lookupTeleportSpell(xi.magic.spell.WARP_II)
        assert(r and r.teleportId == xi.teleport.id.WARP and r.duration == 3)

        r = e.lookupTeleportSpell(xi.magic.spell.RETRACE)
        assert(r and r.teleportId == xi.teleport.id.RETRACE and r.duration == 3 and r.campaign)

        r = e.lookupTeleportSpell(xi.magic.spell.TELEPORT_HOLLA)
        assert(r and r.teleportId == xi.teleport.id.HOLLA and r.keyItem == xi.ki.HOLLA_GATE_CRYSTAL and r.duration == 4)

        r = e.lookupTeleportSpell(xi.magic.spell.RECALL_JUGNER)
        assert(r and r.teleportId == xi.teleport.id.JUGNER and r.keyItem == xi.ki.JUGNER_GATE_CRYSTAL)
    end)

    it('returns nil for unknown spell', function()
        assert(e.lookupTeleportSpell(0) == nil)
        assert(e.lookupTeleportSpell(9999) == nil)
    end)
end)

describe('checkTeleportSpellFromParams', function()
    it('campaign allegiance gate', function()
        assert(e.checkTeleportSpellFromParams({ campaignAllegiance = 0 }) == 48)
        assert(e.checkTeleportSpellFromParams({ campaignAllegiance = -1 }) == 48)
        assert(e.checkTeleportSpellFromParams({ campaignAllegiance = 1 }) == 0)
        assert(e.checkTeleportSpellFromParams({ campaignAllegiance = 3 }) == 0)
        assert(e.checkTeleportSpellFromParams({}) == 48)
    end)
end)

describe('useTeleportSpellFromParams', function()
    it('success paths', function()
        local r = e.useTeleportSpellFromParams({
            spellId = xi.magic.spell.WARP, targetIsPC = true, hasKeyItem = true,
        })
        assert(r.apply and r.teleportId == xi.teleport.id.WARP and r.duration == 3 and r.msg == 93)

        -- HasKeyItem false still ok when catalog KI is 0.
        r = e.useTeleportSpellFromParams({
            spellId = xi.magic.spell.WARP, targetIsPC = true, hasKeyItem = false,
        })
        assert(r.apply)

        r = e.useTeleportSpellFromParams({
            spellId = xi.magic.spell.ESCAPE, targetIsPC = true,
        })
        assert(r.apply and r.teleportId == xi.teleport.id.ESCAPE and r.duration == 4)

        r = e.useTeleportSpellFromParams({
            spellId = xi.magic.spell.TELEPORT_HOLLA, targetIsPC = true, hasKeyItem = true,
        })
        assert(r.apply and r.teleportId == xi.teleport.id.HOLLA and r.keyItem == xi.ki.HOLLA_GATE_CRYSTAL)

        r = e.useTeleportSpellFromParams({
            spellId = xi.magic.spell.RETRACE, targetIsPC = true, campaignAllegiance = 2,
        })
        assert(r.apply and r.teleportId == xi.teleport.id.RETRACE and r.duration == 3 and r.campaignRequired)
    end)

    it('failure paths', function()
        local r = e.useTeleportSpellFromParams({
            spellId = xi.magic.spell.WARP, targetIsPC = false, hasKeyItem = true,
        })
        assert(not r.apply and r.msg == 0)

        r = e.useTeleportSpellFromParams({
            spellId = xi.magic.spell.TELEPORT_HOLLA, targetIsPC = true, hasKeyItem = false,
        })
        assert(not r.apply and r.msg == 0 and r.keyItem == xi.ki.HOLLA_GATE_CRYSTAL)

        r = e.useTeleportSpellFromParams({
            spellId = xi.magic.spell.RETRACE, targetIsPC = true, campaignAllegiance = 0,
        })
        assert(not r.apply and r.msg == 0 and r.campaignRequired)

        r = e.useTeleportSpellFromParams({
            spellId = 1, targetIsPC = true, hasKeyItem = true,
        })
        assert(not r.apply and r.msg == 0)
    end)

    it('recall and teleport KI matrix', function()
        local pairs = {
            { xi.magic.spell.RECALL_JUGNER,  xi.ki.JUGNER_GATE_CRYSTAL,      xi.teleport.id.JUGNER },
            { xi.magic.spell.RECALL_PASHH,   xi.ki.PASHHOW_GATE_CRYSTAL,     xi.teleport.id.PASHH },
            { xi.magic.spell.RECALL_MERIPH,  xi.ki.MERIPHATAUD_GATE_CRYSTAL, xi.teleport.id.MERIPH },
            { xi.magic.spell.TELEPORT_ALTEP, xi.ki.ALTEPA_GATE_CRYSTAL,      xi.teleport.id.ALTEP },
            { xi.magic.spell.TELEPORT_DEM,   xi.ki.DEM_GATE_CRYSTAL,         xi.teleport.id.DEM },
            { xi.magic.spell.TELEPORT_HOLLA, xi.ki.HOLLA_GATE_CRYSTAL,       xi.teleport.id.HOLLA },
            { xi.magic.spell.TELEPORT_MEA,   xi.ki.MEA_GATE_CRYSTAL,         xi.teleport.id.MEA },
            { xi.magic.spell.TELEPORT_VAHZL, xi.ki.VAHZL_GATE_CRYSTAL,       xi.teleport.id.VAHZL },
            { xi.magic.spell.TELEPORT_YHOAT, xi.ki.YHOATOR_GATE_CRYSTAL,     xi.teleport.id.YHOAT },
        }

        for _, tc in ipairs(pairs) do
            local ok = e.useTeleportSpellFromParams({
                spellId = tc[1], targetIsPC = true, hasKeyItem = true,
            })
            assert(ok.apply and ok.teleportId == tc[3] and ok.keyItem == tc[2] and ok.duration == 4)

            local fail = e.useTeleportSpellFromParams({
                spellId = tc[1], targetIsPC = true, hasKeyItem = false,
            })
            assert(not fail.apply and fail.msg == 0)
        end
    end)
end)
