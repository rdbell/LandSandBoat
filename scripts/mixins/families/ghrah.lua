-----------------------------------
--- Ghrah Family Mixin
--  https://ffxiclopedia.fandom.com/wiki/Category:Ghrah
--  https://www.bg-wiki.com/ffxi/Category:Ghrah
--  Ghrahs change form to spider (WAR), bird (THF), or human (PLD) and back to ball (BLM) every 60 seconds.
--  Each form has different stats and a unique skill.
-----------------------------------
require('scripts/globals/mixins')
local palaceID = zones[xi.zone.GRAND_PALACE_OF_HUXZOI]
local gardenID = zones[xi.zone.THE_GARDEN_OF_RUHMET]
-----------------------------------
xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.ghrah = xi.mix.ghrah or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

local mobFormLookup = {}

-- Initialize lookup tables
local function initializeLookupTables()
    -- Already initialized
    if next(mobFormLookup) then
        return
    end

    -- Palace bird forms
    if palaceID and palaceID.mob.EOGHRAH_BIRD then
        for _, mobId in pairs(palaceID.mob.EOGHRAH_BIRD) do
            mobFormLookup[mobId] = 3 -- Bird form
        end
    end

    -- Palace spider forms
    if palaceID and palaceID.mob.EOGHRAH_SPIDER then
        for _, mobId in pairs(palaceID.mob.EOGHRAH_SPIDER) do
            mobFormLookup[mobId] = 2 -- Spider form
        end
    end

    -- Garden bird forms
    if gardenID and gardenID.mob.AWGHRAH_BIRD then
        for _, mobId in pairs(gardenID.mob.AWGHRAH_BIRD) do
            mobFormLookup[mobId] = 3 -- Bird form
        end
    end

    -- Garden spider forms
    if gardenID and gardenID.mob.AWGHRAH_SPIDER then
        for _, mobId in pairs(gardenID.mob.AWGHRAH_SPIDER) do
            mobFormLookup[mobId] = 2 -- Spider form
        end
    end

    -- Garden human forms
    if gardenID and gardenID.mob.AWGHRAH_HUMAN then
        for _, mobId in pairs(gardenID.mob.AWGHRAH_HUMAN) do
            mobFormLookup[mobId] = 1 -- Human form
        end
    end
end

local skinConfig = {
    [1161] = { -- Fire
        spellList = 484,
        mods = {
            { xi.mod.FIRE_RES_RANK,     11 },
            { xi.mod.ICE_RES_RANK,      11 },
            { xi.mod.WATER_RES_RANK,    -3 },
            { xi.mod.PARALYZE_RES_RANK, 11 },
            { xi.mod.BIND_RES_RANK,     11 },
            { xi.mod.POISON_RES_RANK,   -3 }
        }
    },
    [1162] = { -- Ice
        spellList = 479,
        mods = {
            { xi.mod.ICE_RES_RANK,      11 },
            { xi.mod.WIND_RES_RANK,     11 },
            { xi.mod.FIRE_RES_RANK,     -3 },
            { xi.mod.SILENCE_RES_RANK,  11 },
            { xi.mod.PARALYZE_RES_RANK, 11 },
            { xi.mod.BIND_RES_RANK,     11 }
        }
    },
    [1163] = { -- Wind
        spellList = 480,
        mods = {
            { xi.mod.WIND_RES_RANK,     11 },
            { xi.mod.EARTH_RES_RANK,    11 },
            { xi.mod.ICE_RES_RANK,      -3 },
            { xi.mod.SLOW_RES_RANK,     11 },
            { xi.mod.SILENCE_RES_RANK,  11 },
            { xi.mod.PARALYZE_RES_RANK, -3 },
            { xi.mod.BIND_RES_RANK,     -3 }
        }
    },
    [1164] = { -- Earth
        spellList = 481,
        mods = {
            { xi.mod.EARTH_RES_RANK,   11 },
            { xi.mod.THUNDER_RES_RANK, 11 },
            { xi.mod.WIND_RES_RANK,    -3 },
            { xi.mod.SLOW_RES_RANK,    11 },
            { xi.mod.SILENCE_RES_RANK, -3 }
        }
    },
    [1165] = { -- Lightning
        spellList = 482,
        mods = {
            { xi.mod.THUNDER_RES_RANK, 11 },
            { xi.mod.WATER_RES_RANK,   11 },
            { xi.mod.EARTH_RES_RANK,   -3 },
            { xi.mod.POISON_RES_RANK,  11 },
            { xi.mod.SLOW_RES_RANK,    -3 }
        }
    },
    [1166] = { -- Water
        spellList = 483,
        mods = {
            { xi.mod.WATER_RES_RANK,   11 },
            { xi.mod.FIRE_RES_RANK,    11 },
            { xi.mod.THUNDER_RES_RANK, -3 },
            { xi.mod.POISON_RES_RANK,  11 }
        }
    },
    [1167] = { -- Light
        spellList = 478,
        mods = {
            { xi.mod.LIGHT_RES_RANK,       11 },
            { xi.mod.DARK_RES_RANK,        -3 },
            { xi.mod.LIGHT_SLEEP_RES_RANK, 11 },
            { xi.mod.DARK_SLEEP_RES_RANK,  -3 },
            { xi.mod.BLIND_RES_RANK,       -3 }
        }
    },
    [1168] = { -- Dark
        spellList = 477,
        mods = {
            { xi.mod.DARK_RES_RANK,        11 },
            { xi.mod.LIGHT_RES_RANK,       -3 },
            { xi.mod.DARK_SLEEP_RES_RANK,  11 },
            { xi.mod.BLIND_RES_RANK,       11 },
            { xi.mod.LIGHT_SLEEP_RES_RANK, -3 },
        }
    }
}

-- Ghrah form determination using lookup table
-- Ball form (0) is default, Human (1), Spider (2), Bird (3)
local function getTargetForm(mob)
    return mobFormLookup[mob:getID()] or 0 -- Default to ball form if not found
end

local function initializeOriginalMods(mob)
    if mob:getLocalVar('originalATT') == 0 then
        mob:setLocalVar('originalATT', mob:getMod(xi.mod.ATT))
        mob:setLocalVar('originalDEF', mob:getMod(xi.mod.DEF))
        mob:setLocalVar('originalEVA', mob:getMod(xi.mod.EVA))
    end
end

xi.mix.ghrah.formPlan = function(form, originalAtt, originalDef, originalEva, mainLevel)
    local plan = {
        baseDamageModifier = 0,
        att                = originalAtt,
        def                = originalDef,
        eva                = originalEva,
        tripleAttack       = 0,
        aggressive         = form ~= 0,
        animationSub       = form,
    }

    if form == 1 then
        plan.def = originalDef * 2 + 60
    elseif form == 2 then
        plan.baseDamageModifier = mainLevel + 2
        plan.att = originalAtt + 11
        plan.def = originalDef + 11
    elseif form == 3 then
        plan.eva = originalEva + 48
        plan.tripleAttack = 5
    end

    return plan
end

xi.mix.ghrah.formChangePlan = function(now, changeTime, currentForm, targetForm)
    if now <= changeTime then
        return nil
    end

    local nextForm = 0
    if currentForm == 0 then
        nextForm = targetForm
    end

    return { form = nextForm, changeTime = now + 60 }
end

local function switchMobForm(mob, form)
    local originalAtt = mob:getLocalVar('originalATT')
    local originalDef = mob:getLocalVar('originalDEF')
    local originalEva = mob:getLocalVar('originalEVA')

    local plan = xi.mix.ghrah.formPlan(form, originalAtt, originalDef, originalEva, mob:getMainLvl())
    mob:setMobMod(xi.mobMod.BASE_DAMAGE_MODIFIER, plan.baseDamageModifier)
    mob:setMod(xi.mod.ATT, plan.att)
    mob:setMod(xi.mod.DEF, plan.def)
    mob:setMod(xi.mod.EVA, plan.eva)
    mob:setMod(xi.mod.TRIPLE_ATTACK, plan.tripleAttack)
    mob:setAggressive(plan.aggressive)
    mob:setAnimationSub(plan.animationSub)
end

-- Consolidated form change logic
local function handleFormChange(mob)
    local changeTime = mob:getLocalVar('changeTime')
    local currentTime = GetSystemTime()
    local currentForm = mob:getAnimationSub()

    local changePlan = xi.mix.ghrah.formChangePlan(currentTime, changeTime, currentForm, mob:getLocalVar('targetForm') or 0)
    if changePlan then
        switchMobForm(mob, changePlan.form)
        mob:setLocalVar('changeTime', changePlan.changeTime)
    end
end

g_mixins.families.ghrah = function(ghrahMob)
    initializeLookupTables() -- Initialize lookup tables once

    ghrahMob:addListener('SPAWN', 'GHRAH_SPAWN', function(mob)
        local skin = math.random(1161, 1168)
        mob:setModelId(skin)
        mob:setAnimationSub(0)
        mob:setAggressive(false)
        mob:setLocalVar('changeTime', GetSystemTime() + math.random(40, 60)) -- Stagger first change
        mob:setLocalVar('targetForm', getTargetForm(mob))
        mob:addMod(xi.mod.MATT, 20) -- Ghrah have innate +20 MATT on top of BLM bonuses
        mob:addMod(xi.mod.DMGMAGIC, -1250)
        mob:addMod(xi.mod.MDEF, 20)
        initializeOriginalMods(mob)

        local config = skinConfig[skin]
        if config then
            mob:setSpellList(config.spellList)
            for _, modData in ipairs(config.mods) do
                mob:setMod(modData[1], modData[2])
            end
        end
    end)

    ghrahMob:addListener('ROAM_TICK', 'GHRAH_TICK', function(mob)
        handleFormChange(mob)
    end)

    ghrahMob:addListener('COMBAT_TICK', 'GHRAH_COMBAT', function(mob)
        if not xi.combat.behavior.isEntityBusy(mob) then
            handleFormChange(mob)
        end
    end)
end

return g_mixins.families.ghrah
