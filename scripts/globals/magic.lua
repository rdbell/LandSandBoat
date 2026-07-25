require('scripts/globals/combat/magic_hit_rate')
require('scripts/globals/jobpoints')
require('scripts/globals/spells/damage_spell')
-----------------------------------
xi = xi or {}
xi.magic = xi.magic or {}

-----------------------------------
-- Cure power pure helpers
-- Dual-wired to OmegaXI internal/curepower (slice 6718 / 0867).
-- Source: http://members.shaw.ca/pizza_steve/cure/Cure_Calculator.html
-----------------------------------

xi.magic.curePotencyCap   = 50
xi.magic.curePotencyIICap = 30
xi.magic.divineSealMult   = 2
xi.magic.raptureBase      = 1.5

-- Pure getCurePower once MND/VIT/healing skill inject (isBlueMagic unused).
xi.magic.getCurePowerFromParams = function(params)
    local mnd = params.mnd or 0
    local vit = params.vit or 0
    local skill = params.healingSkill or 0
    return math.floor(mnd / 2) + math.floor(vit / 4) + skill
end

-- Pure getCurePowerOld.
xi.magic.getCurePowerOldFromParams = function(params)
    local mnd = params.mnd or 0
    local vit = params.vit or 0
    local skill = params.healingSkill or 0
    return (3 * mnd) + vit + (3 * math.floor(skill / 5))
end

-- Pure getBaseCure ladder: ((power - basepower) / divisor) + constant
xi.magic.getBaseCureFromParams = function(params)
    return ((params.power or 0) - (params.basepower or 0)) / (params.divisor or 1) + (params.constant or 0)
end

-- Pure getBaseCureOld: (power / 2) / divisor + constant
xi.magic.getBaseCureOldFromParams = function(params)
    return ((params.power or 0) / 2) / (params.divisor or 1) + (params.constant or 0)
end

-- Pure getCureFinal. Returns final, consumeRapture.
-- params: baseCure, minCure, curePotency, curePotencyII, dayWeatherBonus,
--   hasDivineSeal, isBlueMagic, hasRapture, raptureAmountMod
xi.magic.getCureFinalFromParams = function(params)
    local basecure = params.baseCure or 0
    local minCure = params.minCure or 0
    if basecure < minCure then
        basecure = minCure
    end

    local curePot = math.min(params.curePotency or 0, xi.magic.curePotencyCap) / 100
    local curePotII = math.min(params.curePotencyII or 0, xi.magic.curePotencyIICap) / 100
    local potency = 1 + curePot + curePotII

    local dSeal = 1
    if params.hasDivineSeal then
        dSeal = xi.magic.divineSealMult
    end

    local rapture = 1
    local consumeRapture = false
    if not params.isBlueMagic and params.hasRapture then
        rapture = xi.magic.raptureBase + (params.raptureAmountMod or 0) / 100
        consumeRapture = true
    end

    local dayWeatherBonus = params.dayWeatherBonus
    if dayWeatherBonus == nil then
        dayWeatherBonus = 1
    end

    local final = math.floor(basecure)
    final = math.floor(final * potency)
    final = math.floor(final * dayWeatherBonus)
    final = math.floor(final * rapture)
    final = math.floor(final * dSeal)

    return final, consumeRapture
end

-- Pure isValidHealTarget once allegiance and objType inject.
xi.magic.isValidHealTargetFromParams = function(params)
    if (params.casterAllegiance or 0) ~= (params.targetAllegiance or 0) then
        return false
    end

    local t = params.targetObjType or 0
    return t == xi.objType.PC or
        t == xi.objType.MOB or
        t == xi.objType.TRUST or
        t == xi.objType.FELLOW
end

-----------------------------------
-- Entity hosts
-----------------------------------
function getCurePower(caster, isBlueMagic)
    return xi.magic.getCurePowerFromParams({
        mnd          = caster:getStat(xi.mod.MND),
        vit          = caster:getStat(xi.mod.VIT),
        healingSkill = caster:getSkillLevel(xi.skill.HEALING_MAGIC),
    })
end

function getCurePowerOld(caster)
    return xi.magic.getCurePowerOldFromParams({
        mnd          = caster:getStat(xi.mod.MND),
        vit          = caster:getStat(xi.mod.VIT),
        healingSkill = caster:getSkillLevel(xi.skill.HEALING_MAGIC),
    })
end

function getBaseCure(power, divisor, constant, basepower)
    return xi.magic.getBaseCureFromParams({
        power     = power,
        divisor   = divisor,
        constant  = constant,
        basepower = basepower,
    })
end

function getBaseCureOld(power, divisor, constant)
    return xi.magic.getBaseCureOldFromParams({
        power    = power,
        divisor  = divisor,
        constant = constant,
    })
end

function getCureFinal(caster, spell, basecure, minCure, isBlueMagic)
    local final, consumeRapture = xi.magic.getCureFinalFromParams({
        baseCure         = basecure,
        minCure          = minCure,
        curePotency      = caster:getMod(xi.mod.CURE_POTENCY),
        curePotencyII    = caster:getMod(xi.mod.CURE_POTENCY_II),
        dayWeatherBonus  = xi.spells.damage.calculateDayAndWeather(caster, spell:getElement(), false),
        hasDivineSeal    = caster:hasStatusEffect(xi.effect.DIVINE_SEAL),
        isBlueMagic      = isBlueMagic,
        hasRapture       = caster:hasStatusEffect(xi.effect.RAPTURE),
        raptureAmountMod = caster:getMod(xi.mod.RAPTURE_AMOUNT),
    })
    if consumeRapture then
        caster:delStatusEffectSilent(xi.effect.RAPTURE)
    end

    return final
end

function isValidHealTarget(caster, target)
    return xi.magic.isValidHealTargetFromParams({
        casterAllegiance = caster:getAllegiance(),
        targetAllegiance = target:getAllegiance(),
        targetObjType    = target:getObjType(),
    })
end

-- Applies resistance for additional effects
function applyResistanceAddEffect(actor, target, element, bonusMacc)
    return xi.combat.magicHitRate.calculateResistRate(actor, target, 0, xi.skill.NONE, 0, element, 0, 0, bonusMacc)
end

function finalMagicNonSpellAdjustments(caster, target, ele, dmg)
    -- Handles target's HP adjustment and returns SIGNED dmg (negative values on absorb)

    dmg = math.floor(dmg * xi.combat.damage.calculateDamageAdjustment(target, false, true, false, false))
    dmg = math.floor(dmg * xi.spells.damage.calculateAbsorption(target, ele, true))
    dmg = math.floor(dmg * xi.spells.damage.calculateNullification(target, ele, true, false))
    dmg = math.floor(target:handleSevereDamage(dmg, false))

    dmg = utils.handlePhalanx(target, dmg)
    dmg = utils.handleOneForAll(target, dmg)
    dmg = utils.handleStoneskin(target, dmg)

    dmg = utils.clamp(dmg, -99999, 99999)

    if dmg < 0 then
        dmg = -(target:addHP(-dmg))
    else
        target:takeDamage(dmg, caster, xi.attackType.MAGICAL, xi.damageType.ELEMENTAL + ele)
    end

    -- Not updating enmity from damage, as this is primarily used for additional effects (which don't generate emnity)
    --  in the case that updating enmity is needed, do it manually after calling this
    -- target:updateEnmityFromDamage(caster, dmg)

    return dmg
end

function addBonusesAbility(caster, ele, target, dmg, params)
    local affinityBonus = xi.spells.damage.calculateElementalStaffBonus(caster, ele)
    dmg = math.floor(dmg * affinityBonus)

    local magicDefense = xi.combat.damage.magicalElementSDT(target, ele)
    dmg = math.floor(dmg * magicDefense)

    local dayWeatherBonus = xi.spells.damage.calculateDayAndWeather(caster, ele, false)
    dmg = math.floor(dmg * dayWeatherBonus)

    local mab = 1
    local mdefBarBonus = 0
    if
        ele >= xi.element.FIRE and
        ele <= xi.element.WATER and
        target:hasStatusEffect(xi.data.element.getAssociatedBarspellEffect(ele))
    then -- bar- spell magic defense bonus
        mdefBarBonus = target:getStatusEffect(xi.data.element.getAssociatedBarspellEffect(ele)):getSubPower()
    end

    if params ~= nil and params.bonusmab ~= nil and params.includemab then
        mab = (100 + caster:getMod(xi.mod.MATT) + params.bonusmab) / (100 + target:getMod(xi.mod.MDEF) + mdefBarBonus)
    elseif params == nil or (params ~= nil and params.includemab) then
        mab = (100 + caster:getMod(xi.mod.MATT)) / (100 + target:getMod(xi.mod.MDEF) + mdefBarBonus)
    end

    if mab < 0 then
        mab = 0
    end

    dmg = math.floor(dmg * mab)

    return dmg
end
