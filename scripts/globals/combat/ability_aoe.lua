-----------------------------------
-- Global file for ability AoE type and radius calculations.
-- Pure inject dual-wired to OmegaXI internal/abilityaoe (slice 6695 / 0846).
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.abilityAoE = xi.combat.abilityAoE or {}

xi.combat.abilityAoE.overrideRadius = 10

-- Pure calculateTypeAndRadius once ability fields and caster mods are injected.
-- params: abilityID, recastID, baseType, baseRadius,
--   liementExtendsToArea, hasContradance, rollRange
---@return [xi.aoeType, number]
xi.combat.abilityAoE.calculateTypeAndRadiusFromParams = function(params)
    local abilityID = params.abilityID or 0
    local recastID = params.recastID or 0
    local baseType = params.baseType or xi.aoeType.NONE
    local baseRadius = params.baseRadius or 0

    -- Epeolatry makes Liement a 10y AoE
    if
        abilityID == xi.jobAbility.LIEMENT and
        (params.liementExtendsToArea or 0) > 0
    then
        return { xi.aoeType.ROUND, xi.combat.abilityAoE.overrideRadius }
    end

    -- Contradance makes Healing Waltz a 10y AoE
    if
        abilityID == xi.jobAbility.HEALING_WALTZ and
        params.hasContradance
    then
        return { xi.aoeType.ROUND, xi.combat.abilityAoE.overrideRadius }
    end

    -- Luzaf's Ring increases COR roll radius (8y -> 16y)
    if
        recastID == xi.recastID.PHANTOM_ROLL or
        recastID == xi.recastID.DOUBLE_UP
    then
        return { xi.aoeType.ROUND, baseRadius + (params.rollRange or 0) }
    end

    return { baseType, baseRadius }
end

---Calculate ability AoE type and radius based on caster modifiers.
---@param caster CBaseEntity
---@param ability CAbility
---@return [xi.aoeType, number]
xi.combat.abilityAoE.calculateTypeAndRadius = function(caster, ability)
    return xi.combat.abilityAoE.calculateTypeAndRadiusFromParams({
        abilityID            = ability:getID(),
        recastID             = ability:getRecastID(),
        baseType             = ability:getAOE(),
        baseRadius           = ability:getRadius(),
        liementExtendsToArea = caster:getMod(xi.mod.LIEMENT_EXTENDS_TO_AREA),
        hasContradance       = caster:hasStatusEffect(xi.effect.CONTRADANCE),
        rollRange            = caster:getMod(xi.mod.ROLL_RANGE),
    })
end
