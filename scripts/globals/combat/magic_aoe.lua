-----------------------------------
-- Global file for spells AoE type and radius calculations.
-- Pure injects dual-wired to OmegaXI internal/magicaoe (slice 6696 / 0849).
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.magicAoE = xi.combat.magicAoE or {}

xi.combat.magicAoE.overrideRadius = 10

-----------------------------------
-- Pure injects
-----------------------------------

-- Pure song radius once spell/caster fields and skill cap are injected.
-- params: baseRadius, baseType, hasPianissimo, mainJob, rangedSkillType,
--   spellGroup, stringSkill, skillCap
---@return integer
xi.combat.magicAoE.songRadiusFromParams = function(params)
    local baseRadius = params.baseRadius or 0
    local baseType = params.baseType or xi.magic.aoe.NONE

    -- Spell is not AoE or caster is under Pianissimo
    if
        baseType == xi.magic.aoe.NONE or
        (baseType == xi.magic.aoe.PIANISSIMO and params.hasPianissimo)
    then
        return 0
    end

    -- Caster is not BRD or does not have a String instrument equipped
    if
        params.mainJob ~= xi.job.BRD or
        params.rangedSkillType ~= xi.skill.STRING_INSTRUMENT or
        params.spellGroup ~= xi.magic.spellGroup.SONG
    then
        return baseRadius
    end

    local skillCap = params.skillCap or 0
    if skillCap <= 0 then
        return baseRadius
    end

    -- Range scales from 1.0x to 2.0x based on string skill vs song level skill cap
    -- Range is always whole, no partial yalm bonus range.
    local stringSkill = params.stringSkill or 0
    local multiplier = utils.clamp(stringSkill / skillCap, 1.0, 2.0)

    return math.floor(baseRadius * multiplier)
end

-- Pure mob/non-PC AoE collapse (formerly local calculateMobAoE).
-- params: baseType, baseRadius
---@return [xi.magic.aoe, number]
xi.combat.magicAoE.mobAoEFromParams = function(params)
    local baseType = params.baseType or xi.magic.aoe.NONE
    local baseRadius = params.baseRadius or 0

    if
        baseType == xi.magic.aoe.RADIAL_MANI or
        baseType == xi.magic.aoe.RADIAL_ACCE or
        baseType == xi.magic.aoe.DIFFUSION
    then
        return { xi.magic.aoe.NONE, 0 }
    end

    if baseType == xi.magic.aoe.PIANISSIMO then
        return { xi.magic.aoe.RADIAL, baseRadius }
    end

    return { baseType, baseRadius }
end

-- Pure calculateTypeAndRadius once spell/caster fields are injected.
-- DivineVeilProc is the resolved trait+(Divine Seal or AOE_NA roll) outcome.
-- Returns { aoeType, radius, consumePianissimo } — host applies delStatusEffect.
-- params: baseType, baseRadius, spellFamily, spellGroup, spellID, element,
--   isPC, isTrust, hasMajesty, hasAccession, hasManifestation, hasTheurgicFocus,
--   hasPianissimo, hasDiffusion, hasConvergence, divineVeilProc, utsusemiAOEMod,
--   mainJob, rangedSkillType, stringSkill, skillCap
xi.combat.magicAoE.calculateTypeAndRadiusFromParams = function(params)
    local baseType = params.baseType or xi.magic.aoe.NONE
    local baseRadius = params.baseRadius or 0
    local spellFamily = params.spellFamily or 0
    local spellGroup = params.spellGroup or 0

    -- Until proven otherwise, these effects only apply to players and trusts.
    if not params.isPC and not params.isTrust then
        local mob = xi.combat.magicAoE.mobAoEFromParams({
            baseType = baseType, baseRadius = baseRadius,
        })

        return { mob[1], mob[2], false }
    end

    -- Majesty converts Cure and Protect spells to 10y AoE
    if params.hasMajesty then
        if
            spellFamily == xi.magic.spellFamily.CURE or
            spellFamily == xi.magic.spellFamily.PROTECT
        then
            return { xi.magic.aoe.RADIAL, xi.combat.magicAoE.overrideRadius, false }
        end
    end

    -- Accession / Divine Veil converts eligible spells to 10y AoE
    if baseType == xi.magic.aoe.RADIAL_ACCE then
        if params.hasAccession then
            return { xi.magic.aoe.RADIAL, xi.combat.magicAoE.overrideRadius, false }
        end

        -- Divine Veil: -na / Erase with pre-resolved proc inject
        if
            (spellFamily == xi.magic.spellFamily.NA or params.spellID == xi.magic.spell.ERASE) and
            params.divineVeilProc
        then
            return { xi.magic.aoe.RADIAL, xi.combat.magicAoE.overrideRadius, false }
        end

        return { xi.magic.aoe.NONE, 0, false }
    end

    -- Manifestation converts eligible spells to 10y AoE
    if
        params.hasManifestation and
        baseType == xi.magic.aoe.RADIAL_MANI
    then
        return { xi.magic.aoe.RADIAL, xi.combat.magicAoE.overrideRadius, false }
    end

    -- Theurgic Focus halves the AoE radius of -ra spells
    if
        params.hasTheurgicFocus and
        spellFamily >= xi.magic.spellFamily.FIRA and
        spellFamily <= xi.magic.spellFamily.WATERA
    then
        return { xi.magic.aoe.RADIAL, math.floor(baseRadius / 2), false }
    end

    -- Songs: Pianissimo forces single target, otherwise calculate BRD radius bonus
    if spellGroup == xi.magic.spellGroup.SONG then
        if baseType == xi.magic.aoe.NONE then
            return { xi.magic.aoe.NONE, 0, false }
        end

        if
            params.hasPianissimo and
            baseType == xi.magic.aoe.PIANISSIMO
        then
            return { xi.magic.aoe.NONE, 0, true }
        end

        local songRadius = xi.combat.magicAoE.songRadiusFromParams({
            baseRadius      = baseRadius,
            baseType        = baseType,
            hasPianissimo   = params.hasPianissimo,
            mainJob         = params.mainJob,
            rangedSkillType = params.rangedSkillType,
            spellGroup      = spellGroup,
            stringSkill     = params.stringSkill,
            skillCap        = params.skillCap,
        })

        return { xi.magic.aoe.RADIAL, songRadius, false }
    end

    -- Diffusion converts eligible spells to 10y AoE
    if
        params.hasDiffusion and
        baseType == xi.magic.aoe.DIFFUSION
    then
        return { xi.magic.aoe.RADIAL, xi.combat.magicAoE.overrideRadius, false }
    end

    -- Convergence forces BLU offensive magic spells to be single target
    if
        params.hasConvergence and
        spellGroup == xi.magic.spellGroup.BLUE and
        (params.element or xi.element.NONE) ~= xi.element.NONE
    then
        return { xi.magic.aoe.NONE, 0, false }
    end

    -- Certain equipment convert Utsusemi spells to 10y AoE
    if
        (params.utsusemiAOEMod or 0) ~= 0 and
        spellFamily == xi.magic.spellFamily.UTSUSEMI
    then
        return { xi.magic.aoe.RADIAL, xi.combat.magicAoE.overrideRadius, false }
    end

    return { baseType, baseRadius, false }
end

-----------------------------------
-- Entity hosts (inject → pure)
-----------------------------------

---Return total song radius after factoring String equipment bonus
---@param caster CBaseEntity
---@param spell CSpell
---@return integer
xi.combat.magicAoE.calculateSongRadius = function(caster, spell)
    local songLevel = spell:getLevel(xi.job.BRD)

    return xi.combat.magicAoE.songRadiusFromParams({
        baseRadius      = spell:getRadius(),
        baseType        = spell:isAoE(),
        hasPianissimo   = caster:hasStatusEffect(xi.effect.PIANISSIMO),
        mainJob         = caster:getMainJob(),
        rangedSkillType = caster:getWeaponSkillType(xi.slot.RANGED),
        spellGroup      = spell:getSpellGroup(),
        stringSkill     = caster:getSkillLevel(xi.skill.STRING_INSTRUMENT),
        skillCap        = xi.data.skillLevel.getSkillCap(songLevel, xi.skillRank.C),
    })
end

---Calculate spell AoE type and radius based on caster modifiers.
---@param caster CBaseEntity
---@param spell CSpell
---@return [xi.magic.aoe, number]
xi.combat.magicAoE.calculateTypeAndRadius = function(caster, spell)
    local baseType = spell:isAoE()
    local baseRadius = spell:getRadius()
    local isPC = caster:isPC()
    local isTrust = caster:isTrust()

    -- Mob path only needs base type/radius (matches former early return; keeps
    -- lightweight smoke harnesses from needing job/spell enums).
    if not isPC and not isTrust then
        local mob = xi.combat.magicAoE.mobAoEFromParams({
            baseType = baseType, baseRadius = baseRadius,
        })

        return { mob[1], mob[2] }
    end

    local spellFamily = spell:getSpellFamily()
    local spellGroup = spell:getSpellGroup()
    local spellID = spell:getID()

    -- Divine Veil proc inject: trait + (Divine Seal or AOE_NA d100)
    local divineVeilProc = false
    if
        baseType == xi.magic.aoe.RADIAL_ACCE and
        not caster:hasStatusEffect(xi.effect.ACCESSION) and
        caster:hasTrait(xi.trait.DIVINE_VEIL) and
        (spellFamily == xi.magic.spellFamily.NA or spellID == xi.magic.spell.ERASE)
    then
        divineVeilProc =
            caster:hasStatusEffect(xi.effect.DIVINE_SEAL) or
            math.random(100) <= caster:getMod(xi.mod.AOE_NA)
    end

    -- Song skill-cap inject only when the pure path will need it.
    local mainJob = 0
    local rangedSkillType = 0
    local stringSkill = 0
    local skillCap = 0
    if spellGroup == xi.magic.spellGroup.SONG then
        mainJob = caster:getMainJob()
        rangedSkillType = caster:getWeaponSkillType(xi.slot.RANGED)
        stringSkill = caster:getSkillLevel(xi.skill.STRING_INSTRUMENT)
        skillCap = xi.data.skillLevel.getSkillCap(spell:getLevel(xi.job.BRD), xi.skillRank.C)
    end

    local result = xi.combat.magicAoE.calculateTypeAndRadiusFromParams({
        baseType         = baseType,
        baseRadius       = baseRadius,
        spellFamily      = spellFamily,
        spellGroup       = spellGroup,
        spellID          = spellID,
        element          = spell:getElement(),
        isPC             = isPC,
        isTrust          = isTrust,
        hasMajesty       = caster:hasStatusEffect(xi.effect.MAJESTY),
        hasAccession     = caster:hasStatusEffect(xi.effect.ACCESSION),
        hasManifestation = caster:hasStatusEffect(xi.effect.MANIFESTATION),
        hasTheurgicFocus = caster:hasStatusEffect(xi.effect.THEURGIC_FOCUS),
        hasPianissimo    = caster:hasStatusEffect(xi.effect.PIANISSIMO),
        hasDiffusion     = caster:hasStatusEffect(xi.effect.DIFFUSION),
        hasConvergence   = caster:hasStatusEffect(xi.effect.CONVERGENCE),
        divineVeilProc   = divineVeilProc,
        utsusemiAOEMod   = caster:getMod(xi.mod.UTSUSEMI_AOE),
        mainJob          = mainJob,
        rangedSkillType  = rangedSkillType,
        stringSkill      = stringSkill,
        skillCap         = skillCap,
    })

    -- Pianissimo host residual: delete after pure marks consume
    if result[3] then
        caster:delStatusEffect(xi.effect.PIANISSIMO)
    end

    return { result[1], result[2] }
end
