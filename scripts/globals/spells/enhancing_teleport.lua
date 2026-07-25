-----------------------------------
-- Teleport Spell Utilities
-- Dual-wired pure inject forms (slice 6722 / 0884):
--   lookupTeleportSpell, checkTeleportSpellFromParams,
--   useTeleportSpellFromParams, teleportSpellCatalogSize
-- Parity: internal/teleportspell
-----------------------------------
require('scripts/globals/teleports')
-----------------------------------
xi = xi or {}
xi.spells = xi.spells or {}
xi.spells.enhancing = xi.spells.enhancing or {}
-----------------------------------
-- Message / check pins (internal/teleportspell).
xi.spells.enhancing.teleportMsgMagicTeleport = 93 -- xi.msg.basic.MAGIC_TELEPORT
xi.spells.enhancing.teleportMsgNone          = 0  -- xi.msg.basic.NONE
xi.spells.enhancing.teleportCheckMsgOK       = 0
xi.spells.enhancing.teleportCheckMsgCannot   = 48
xi.spells.enhancing.teleportEffectID         = 797 -- xi.effect.TELEPORT

-- Table variables.
local column =
{
    TELEPORT_ID       = 1,
    TELEPORT_KEY_ITEM = 2,
    TELEPORT_DURATION = 3,
    TELEPORT_CAMPAIGN = 4,
}

local pTable =
{
-- Structure:       [spellId] = { Teleport, Key_Item, duration, campaign },
    [xi.magic.spell.ESCAPE        ] = { xi.teleport.id.ESCAPE,  0,                              4, false },
    [xi.magic.spell.RECALL_JUGNER ] = { xi.teleport.id.JUGNER,  xi.ki.JUGNER_GATE_CRYSTAL,      4, false },
    [xi.magic.spell.RECALL_MERIPH ] = { xi.teleport.id.MERIPH,  xi.ki.MERIPHATAUD_GATE_CRYSTAL, 4, false },
    [xi.magic.spell.RECALL_PASHH  ] = { xi.teleport.id.PASHH,   xi.ki.PASHHOW_GATE_CRYSTAL,     4, false },
    [xi.magic.spell.RETRACE       ] = { xi.teleport.id.RETRACE, 0,                              3, true  },
    [xi.magic.spell.TELEPORT_ALTEP] = { xi.teleport.id.ALTEP,   xi.ki.ALTEPA_GATE_CRYSTAL,      4, false },
    [xi.magic.spell.TELEPORT_DEM  ] = { xi.teleport.id.DEM,     xi.ki.DEM_GATE_CRYSTAL,         4, false },
    [xi.magic.spell.TELEPORT_HOLLA] = { xi.teleport.id.HOLLA,   xi.ki.HOLLA_GATE_CRYSTAL,       4, false },
    [xi.magic.spell.TELEPORT_MEA  ] = { xi.teleport.id.MEA,     xi.ki.MEA_GATE_CRYSTAL,         4, false },
    [xi.magic.spell.TELEPORT_VAHZL] = { xi.teleport.id.VAHZL,   xi.ki.VAHZL_GATE_CRYSTAL,       4, false },
    [xi.magic.spell.TELEPORT_YHOAT] = { xi.teleport.id.YHOAT,   xi.ki.YHOATOR_GATE_CRYSTAL,     4, false },
    [xi.magic.spell.WARP          ] = { xi.teleport.id.WARP,    0,                              3, false },
    [xi.magic.spell.WARP_II       ] = { xi.teleport.id.WARP,    0,                              3, false },
}

-- Pure catalog lookup (internal/teleportspell.Lookup).
-- returns: { teleportId, keyItem, duration, campaign } or nil
xi.spells.enhancing.lookupTeleportSpell = function(spellId)
    local row = pTable[spellId or 0]
    if not row then
        return nil
    end

    return {
        teleportId = row[column.TELEPORT_ID],
        keyItem    = row[column.TELEPORT_KEY_ITEM],
        duration   = row[column.TELEPORT_DURATION],
        campaign   = row[column.TELEPORT_CAMPAIGN],
    }
end

xi.spells.enhancing.teleportSpellCatalogSize = function()
    local n = 0
    for _ in pairs(pTable) do
        n = n + 1
    end

    return n
end

-- Pure Retrace campaign gate (internal/teleportspell.CheckRetrace).
-- params: campaignAllegiance
xi.spells.enhancing.checkTeleportSpellFromParams = function(params)
    params = params or {}
    if (params.campaignAllegiance or 0) > 0 then
        return xi.spells.enhancing.teleportCheckMsgOK
    end

    return xi.spells.enhancing.teleportCheckMsgCannot
end

-- Pure useTeleportSpell after PC / KI / campaign injects
-- (internal/teleportspell.Use).
-- params: spellId, targetIsPC, hasKeyItem, campaignAllegiance
-- returns: { apply, teleportId, duration, msg, campaignRequired, keyItem }
xi.spells.enhancing.useTeleportSpellFromParams = function(params)
    params = params or {}
    local row = xi.spells.enhancing.lookupTeleportSpell(params.spellId)
    if not row then
        return {
            apply            = false,
            teleportId       = 0,
            duration         = 0,
            msg              = xi.spells.enhancing.teleportMsgNone,
            campaignRequired = false,
            keyItem          = 0,
        }
    end

    local hasKI      = row.keyItem == 0 or params.hasKeyItem
    local campaignOK = not row.campaign or (params.campaignAllegiance or 0) > 0

    if params.targetIsPC and hasKI and campaignOK then
        return {
            apply            = true,
            teleportId       = row.teleportId,
            duration         = row.duration,
            msg              = xi.spells.enhancing.teleportMsgMagicTeleport,
            campaignRequired = row.campaign,
            keyItem          = row.keyItem,
        }
    end

    return {
        apply            = false,
        teleportId       = row.teleportId,
        duration         = row.duration,
        msg              = xi.spells.enhancing.teleportMsgNone,
        campaignRequired = row.campaign,
        keyItem          = row.keyItem,
    }
end

-- Check for "Retrace" Spell (host → pure).
xi.spells.enhancing.checkTeleportSpell = function(caster, target, spell)
    return xi.spells.enhancing.checkTeleportSpellFromParams({
        campaignAllegiance = target:getCampaignAllegiance(),
    })
end

-- Main function for Teleport / Warp / etc. Spells (host → pure).
xi.spells.enhancing.useTeleportSpell = function(caster, target, spell)
    local spellId = spell:getID()
    local row     = xi.spells.enhancing.lookupTeleportSpell(spellId)
    if not row then
        spell:setMsg(xi.msg.basic.NONE)
        return 0
    end

    local hasKeyItem = row.keyItem == 0 or (row.keyItem > 0 and target:hasKeyItem(row.keyItem))
    local result     = xi.spells.enhancing.useTeleportSpellFromParams({
        spellId            = spellId,
        targetIsPC         = target:getObjType() == xi.objType.PC,
        hasKeyItem         = hasKeyItem,
        campaignAllegiance = target:getCampaignAllegiance(),
    })

    if result.apply then
        target:addStatusEffect(xi.effect.TELEPORT, {
            power    = result.teleportId,
            duration = result.duration,
            origin   = caster,
            icon     = 0,
        })
        spell:setMsg(xi.msg.basic.MAGIC_TELEPORT)
    else
        spell:setMsg(xi.msg.basic.NONE)
    end

    return 0
end
