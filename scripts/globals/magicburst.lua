-----------------------------------
-- Magic burst helpers.
-- Pure injects dual-wired to OmegaXI internal/magicburstlua (slice 6704 / 0994).
-----------------------------------
xi = xi or {}
xi.magicburst = xi.magicburst or {}

-----------------------------------
-- Pure injects
-----------------------------------

-- Pure: action element matches skillchain property (skillchainElementTable).
-- SCProp is resonance power / SC type 1..16 (column index); SCProp 0 → false.
xi.magicburst.doesElementMatchWeaponskill = function(actionElement, SCProp)
    if not SCProp or SCProp == 0 then
        return false
    end

    if
        not actionElement or
        actionElement < xi.element.FIRE or
        actionElement > xi.element.DARK
    then
        return false
    end

    local row = xi.data.element.skillchainElementTable[actionElement]
    if not row then
        return false
    end

    return (row[SCProp] or 0) > 0
end

-- Pure formMagicBurst once SKILLCHAIN status fields are injected.
-- params: actionElement, hasResonance, resonanceTier, resonancePower, resonanceSubPower
-- returns resonanceTier, skillchainCount (both 0 when no burst)
xi.magicburst.formMagicBurstFromParams = function(params)
    local actionElement = params.actionElement

    if not actionElement then
        return 0, 0
    end

    if actionElement <= xi.element.NONE or actionElement > xi.element.DARK then
        return 0, 0
    end

    if not params.hasResonance then
        return 0, 0
    end

    local resonanceTier = params.resonanceTier or 0
    if resonanceTier <= 0 then
        return 0, 0
    end

    if not xi.magicburst.doesElementMatchWeaponskill(actionElement, params.resonancePower or 0) then
        return 0, 0
    end

    return resonanceTier, params.resonanceSubPower or 0
end

-----------------------------------
-- Entity host
-----------------------------------

---@param target CBaseEntity
---@param actionElement number
---@return number, number
xi.magicburst.formMagicBurst = function(target, actionElement)
    if not target then
        return 0, 0
    end

    if not actionElement then
        return 0, 0
    end

    local resonance = target:getStatusEffect(xi.effect.SKILLCHAIN)
    if not resonance then
        return xi.magicburst.formMagicBurstFromParams({
            actionElement = actionElement,
            hasResonance  = false,
        })
    end

    return xi.magicburst.formMagicBurstFromParams({
        actionElement     = actionElement,
        hasResonance      = true,
        resonanceTier     = resonance:getTier(),
        resonancePower    = resonance:getPower(),
        resonanceSubPower = resonance:getSubPower(),
    })
end
