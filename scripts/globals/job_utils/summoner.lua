-----------------------------------
-- Summoner Job Utilities
-- Dual-wired pure inject forms (slice 6744 / 0903):
--   base MP cost catalog/Astral Flow/Apogee, Blood Boon MP cost,
--   Mana Cede TP product, Soothing Ruby power/erase count
-- Parity: internal/summoner
-----------------------------------
require('scripts/globals/ability')
require('scripts/globals/jobpoints')
require('scripts/globals/combat/tp')
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.summoner = xi.job_utils.summoner or {}
-----------------------------------

-- sort of a misnomer, as if Apogee is up, the 'base' mp cost rises.
-----------------------------------
-- Pure inject pins (internal/summoner)
-----------------------------------
xi.job_utils.summoner.baseMPCostMissing           = 9999
xi.job_utils.summoner.apogeeMPCostMultiplier      = 1.5
xi.job_utils.summoner.bloodBoonConserveMin        = 8
xi.job_utils.summoner.bloodBoonConserveMax        = 15
xi.job_utils.summoner.bloodBoonConserveDiv        = 16
xi.job_utils.summoner.manaCedeMPCost              = 100
xi.job_utils.summoner.manaCedeBonusTPBase         = 1000
xi.job_utils.summoner.manaCedeJPPerLevel          = 50
xi.job_utils.summoner.manaCedeTPMin               = 1000
xi.job_utils.summoner.manaCedeTPMax               = 3000
xi.job_utils.summoner.soothingRubyPowerMin        = 1
xi.job_utils.summoner.soothingRubyPowerMax        = 6
xi.job_utils.summoner.baseMPCostPactCount         = 100

-- Pure: BaseMPCost
-- params: abilityId, mainLvl, isAstralFlow, hasApogee, catalogCost (nil if missing)
-- When isAstralFlow, catalogCost is ignored. When not AF and catalogCost is nil → 9999.
xi.job_utils.summoner.baseMPCostFromParams = function(params)
    params = params or {}
    local base
    if params.isAstralFlow then
        base = (params.mainLvl or 0) * 2
    else
        if params.catalogCost == nil then
            return xi.job_utils.summoner.baseMPCostMissing
        end

        base = params.catalogCost
    end

    if params.hasApogee then
        base = base * xi.job_utils.summoner.apogeeMPCostMultiplier
    end

    return base
end

-- Pure: BloodBoonMPCost
-- params: base, bloodBoonRate, roll1to100, conserveRoll8to15, isAstralFlow
xi.job_utils.summoner.bloodBoonMPCostFromParams = function(params)
    params = params or {}
    local base = params.base or 0
    if params.isAstralFlow then
        return base
    end

    if (params.roll1to100 or 0) > (params.bloodBoonRate or 0) then
        return base
    end

    return base * (params.conserveRoll8to15 or 0) / xi.job_utils.summoner.bloodBoonConserveDiv
end

-- Pure: ManaCedeTP
-- params: avatarTP, manaCedeJP, enhancesManaCede
xi.job_utils.summoner.manaCedeTPFromParams = function(params)
    params = params or {}
    local bonusTP = xi.job_utils.summoner.manaCedeBonusTPBase
        + (params.manaCedeJP or 0) * xi.job_utils.summoner.manaCedeJPPerLevel
    local manaCedeBonus = (100 + (params.enhancesManaCede or 0)) / 100
    local newTP = (params.avatarTP or 0) + bonusTP * manaCedeBonus
    if newTP < xi.job_utils.summoner.manaCedeTPMin then
        newTP = xi.job_utils.summoner.manaCedeTPMin
    end

    if newTP > xi.job_utils.summoner.manaCedeTPMax then
        newTP = xi.job_utils.summoner.manaCedeTPMax
    end

    return math.floor(newTP)
end

-- Pure: SoothingRubyPower
xi.job_utils.summoner.soothingRubyPowerFromParams = function(summoningSkill)
    local factor = math.floor(((summoningSkill or 0) + 99) / 100)
    if factor < xi.job_utils.summoner.soothingRubyPowerMin then
        return xi.job_utils.summoner.soothingRubyPowerMin
    end

    if factor > xi.job_utils.summoner.soothingRubyPowerMax then
        return xi.job_utils.summoner.soothingRubyPowerMax
    end

    return factor
end

-- Pure: SoothingRubyEffectsErased
xi.job_utils.summoner.soothingRubyEffectsErasedFromParams = function(params)
    params = params or {}
    local power = params.power or 0
    local erasableCount = params.erasableCount or 0
    if power < 0 then
        power = 0
    end

    if erasableCount < 0 then
        erasableCount = 0
    end

    if erasableCount < power then
        return erasableCount
    end

    return power
end

-- sort of a misnomer, as if Apogee is up, the 'base' mp cost rises.
local function getBaseMPCost(player, ability)
    local baseMPCostMap =
    {
        -- Carbuncle
        [xi.jobAbility.HEALING_RUBY]     =   6,
        [xi.jobAbility.POISON_NAILS]     =  11,
        [xi.jobAbility.SHINING_RUBY]     =  44,
        [xi.jobAbility.GLITTERING_RUBY]  =  62,
        [xi.jobAbility.SOOTHING_RUBY]    =  74,
        [xi.jobAbility.PACIFYING_RUBY]   =  83,
        [xi.jobAbility.METEORITE]        = 108,
        [xi.jobAbility.HEALING_RUBY_II]  = 124,
        [xi.jobAbility.HOLY_MIST]        = 152,
        -- Leviathan
        [xi.jobAbility.BARRACUDA_DIVE]   =   8,
        [xi.jobAbility.WATER_II]         =  24,
        [xi.jobAbility.SLOWGA]           =  48,
        [xi.jobAbility.TAIL_WHIP]        =  49,
        [xi.jobAbility.SOOTHING_CURRENT] =  95,
        [xi.jobAbility.SPRING_WATER]     =  99,
        [xi.jobAbility.WATER_IV]         = 118,
        [xi.jobAbility.TIDAL_ROAR]       = 138,
        [xi.jobAbility.SPINNING_DIVE]    = 164,
        [xi.jobAbility.GRAND_FALL]       = 182,
        -- Garuda
        [xi.jobAbility.CLAW]             =   7,
        [xi.jobAbility.AERO_II]          =  24,
        [xi.jobAbility.AERIAL_ARMOR]     =  92,
        [xi.jobAbility.FLEET_WIND]       = 114,
        [xi.jobAbility.AERO_IV]          = 118,
        [xi.jobAbility.WHISPERING_WIND]  = 119,
        [xi.jobAbility.HASTEGA]          = 129,
        [xi.jobAbility.PREDATOR_CLAWS]   = 164,
        [xi.jobAbility.WIND_BLADE]       = 182,
        [xi.jobAbility.HASTEGA_II]       = 248,
        -- Titan
        [xi.jobAbility.ROCK_THROW]       =  10,
        [xi.jobAbility.STONE_II]         =  24,
        [xi.jobAbility.ROCK_BUSTER]      =  39,
        [xi.jobAbility.MEGALITH_THROW]   =  62,
        [xi.jobAbility.EARTHEN_WARD]     =  92,
        [xi.jobAbility.STONE_IV]         = 118,
        [xi.jobAbility.CRAG_THROW]       = 124,
        [xi.jobAbility.EARTHEN_ARMOR]    = 156,
        [xi.jobAbility.MOUNTAIN_BUSTER]  = 164,
        [xi.jobAbility.GEOCRUSH]         = 182,
        -- Titan
        [xi.jobAbility.PUNCH]            =   9,
        [xi.jobAbility.FIRE_II]          =  24,
        [xi.jobAbility.BURNING_STRIKE]   =  48,
        [xi.jobAbility.DOUBLE_PUNCH]     =  56,
        [xi.jobAbility.INFERNO_HOWL]     =  72,
        [xi.jobAbility.CRIMSON_HOWL]     =  84,
        [xi.jobAbility.FIRE_IV]          = 118,
        [xi.jobAbility.CONFLAG_STRIKE]   = 141,
        [xi.jobAbility.FLAMING_CRUSH]    = 164,
        [xi.jobAbility.METEOR_STRIKE]    = 182,
        -- Fenrir
        [xi.jobAbility.MOONLIT_CHARGE]   =  17,
        [xi.jobAbility.CRESCENT_FANG]    =  19,
        [xi.jobAbility.LUNAR_ROAR]       =  27,
        [xi.jobAbility.LUNAR_CRY]        =  41,
        [xi.jobAbility.ECLIPTIC_GROWL]   =  46,
        [xi.jobAbility.ECLIPTIC_HOWL]    =  57,
        [xi.jobAbility.HEAVENWARD_HOWL]  =  96,
        [xi.jobAbility.ECLIPSE_BITE]     = 109,
        [xi.jobAbility.LUNAR_BAY]        = 174,
        [xi.jobAbility.IMPACT]           = 222,
        -- Shiva
        [xi.jobAbility.AXE_KICK]         =  10,
        [xi.jobAbility.BLIZZARD_II]      =  24,
        [xi.jobAbility.SLEEPGA]          =  56,
        [xi.jobAbility.FROST_ARMOR]      =  63,
        [xi.jobAbility.DOUBLE_SLAP]      =  96,
        [xi.jobAbility.BLIZZARD_IV]      = 118,
        [xi.jobAbility.DIAMOND_STORM]    = 138,
        [xi.jobAbility.RUSH]             = 164,
        [xi.jobAbility.HEAVENLY_STRIKE]  = 182,
        [xi.jobAbility.CRYSTAL_BLESSING] = 201,
        -- Ramuh
        [xi.jobAbility.SHOCK_STRIKE]     =   6,
        [xi.jobAbility.THUNDER_II]       =  24,
        [xi.jobAbility.THUNDERSPARK]     =  38,
        [xi.jobAbility.ROLLING_THUNDER]  =  52,
        [xi.jobAbility.SHOCK_SQUALL]     =  67,
        [xi.jobAbility.LIGHTNING_ARMOR]  =  91,
        [xi.jobAbility.THUNDER_IV]       = 118,
        [xi.jobAbility.CHAOTIC_STRIKE]   = 164,
        [xi.jobAbility.THUNDERSTORM]     = 182,
        [xi.jobAbility.VOLT_STRIKE]      = 229,
        -- Diabolos
        [xi.jobAbility.CAMISADO]         =  20,
        [xi.jobAbility.ULTIMATE_TERROR]  =  27,
        [xi.jobAbility.SOMNOLENCE]       =  30,
        [xi.jobAbility.NIGHTMARE]        =  42,
        [xi.jobAbility.NOCTOSHIELD]      =  92,
        [xi.jobAbility.NETHER_BLAST]     = 109,
        [xi.jobAbility.DREAM_SHROUD]     = 121,
        [xi.jobAbility.BLINDSIDE]        = 147,
        [xi.jobAbility.NIGHT_TERROR]     = 177,
        [xi.jobAbility.PAVOR_NOCTURNUS]  = 246,
        -- Cait Sith
        [xi.jobAbility.REGAL_SCRATCH]    = 5,
        [xi.jobAbility.MEWING_LULLABY]   = 61,
        [xi.jobAbility.EARIE_EYE]        = 134,
        [xi.jobAbility.LEVEL_QM_HOLY]    = 235,
        [xi.jobAbility.RAISE_II]         = 160,
        [xi.jobAbility.RERAISE_II]       = 80,
        -- Siren
        [xi.jobAbility.WELT]             =   9,
        [xi.jobAbility.ROUNDHOUSE]       =  52,
        [xi.jobAbility.SONIC_BUFFET]     = 164,
        [xi.jobAbility.TORNADO_II]       = 182,
        [xi.jobAbility.HYSTERIC_ASSAULT] = 222,
    }

    local isAstralFlow = false
    local catalogCost  = nil
    local abilityId    = 0

    if ability then
        abilityId = ability:getID()
        isAstralFlow = ability:getAddType() == xi.addType.ADDTYPE_ASTRAL_FLOW
        if not isAstralFlow then
            catalogCost = baseMPCostMap[abilityId]
        end
    end

    if not isAstralFlow and catalogCost == nil then
        printf('[warning] scripts/globals/job_utils/summoner.lua::getBaseMPCost(): MP cost for xi.jobAbility with id %d not implemented.', abilityId)
    end

    -- https://www.bg-wiki.com/ffxi/Apogee
    -- Apogee, 1.5x MP cost, don't delete effect here because we need to reset BP: Ward/Rage timer upon use
    return xi.job_utils.summoner.baseMPCostFromParams({
        abilityId    = abilityId,
        mainLvl      = player:getMainLvl(),
        isAstralFlow = isAstralFlow,
        hasApogee    = player:hasStatusEffect(xi.effect.APOGEE),
        catalogCost  = catalogCost,
    })
end

local function getMPCost(baseMPCost, player, petskill)
    local isAstralFlow = petskill:getAddType() == xi.addType.ADDTYPE_ASTRAL_FLOW
    local bloodBoonRate = player:getMod(xi.mod.BLOOD_BOON)
    -- assuming it works like Conserve MP... https://www.bg-wiki.com/ffxi/Conserve_MP
    -- Inject RNG rolls into pure form (host rolls once).
    local roll1to100 = math.random(1, 100)
    local conserveRoll = math.random(
        xi.job_utils.summoner.bloodBoonConserveMin,
        xi.job_utils.summoner.bloodBoonConserveMax
    )

    return xi.job_utils.summoner.bloodBoonMPCostFromParams({
        base               = baseMPCost,
        bloodBoonRate      = bloodBoonRate,
        roll1to100         = roll1to100,
        conserveRoll8to15  = conserveRoll,
        isAstralFlow       = isAstralFlow,
    })
end

-- Bloodpact Delay is handled in charentity.cpp
xi.job_utils.summoner.canUseBloodPact = function(player, pet, target, petAbility)
    -- The distance checks are performed in core but should be returned here when possible.
    -- To activate a Blood Pact, the following conditions must be met:
    -- 1 - The summoner is within "Blood Pact: Rage/Ward" range (20y + hitboxes)
    -- 2 - The avatar is within actual Blood Pact range (varies + hitboxes)
    if pet ~= nil then
        local petAction = pet:getCurrentAction()

        -- check if avatar is under status effect
        if
            petAction == xi.action.category.SLEEP or
            petAction == xi.action.category.STUN
        then
            return xi.msg.basic.PET_CANNOT_DO_ACTION, 0 -- TODO: verify exact message in packet.
        end

        -- check if avatar is using a move already
        if petAction == xi.action.category.PET_MOBABILITY_FINISH then
            return 0, 0
        end

        local baseMPCost = getBaseMPCost(player, petAbility)

        if player:getMP() < baseMPCost then
            return xi.msg.basic.UNABLE_TO_USE_JA2, 0 -- TODO: verify exact message in packet.
        end

        return 0, 0
    end

    return xi.msg.basic.UNABLE_TO_USE_JA2, 0 -- TODO: verify exact message in packet.
end

xi.job_utils.summoner.onUseBloodPact = function(target, petskill, summoner, action)
    local bloodPactAbility = GetAbility(petskill:getID()) -- Player abilities and Avatar abilities are mapped 1:1
    if not bloodPactAbility then
        return
    end

    local baseMPCost       = getBaseMPCost(summoner, bloodPactAbility)
    local mpCost           = getMPCost(baseMPCost, summoner, bloodPactAbility)
    local bloodPactRecast  = math.max(0, summoner:getLocalVar('bpRecastTime'))

    if target:getID() == action:getPrimaryTargetID() then
        -- MP and Cooldown is only consumed if the ability goes off
        summoner:delMP(mpCost)

        if target:isMob() then
            target:addBaseEnmity(summoner)
        end

        if summoner:hasStatusEffect(xi.effect.APOGEE) then
            summoner:resetRecast(xi.recast.ABILITY, bloodPactAbility:getRecastID())
            summoner:delStatusEffect(xi.effect.APOGEE)
        else
            if xi.settings.map.BLOOD_PACT_SHARED_TIMER then
                summoner:addRecast(xi.recast.ABILITY, xi.recastID.BLOODPACT_RAGE, bloodPactRecast)
                summoner:addRecast(xi.recast.ABILITY, xi.recastID.BLOODPACT_WARD, bloodPactRecast)
            else
                summoner:addRecast(xi.recast.ABILITY, bloodPactAbility:getRecastID(), bloodPactRecast)
            end
        end
    end
end

-- to be removed once damage is overhauled
xi.job_utils.summoner.calculateTPReturn = function(avatar, target, damage, numHits)
    if damage ~= 0 and numHits > 0 then -- absorbed hits still give TP, though we can't know how many hits actually connected in the current avatar damage formulas
        local tpReturn = xi.combat.tp.getSingleMeleeHitTPReturn(avatar, false)
        tpReturn = tpReturn + 10 * (numHits - 1) -- extra hits give 10 TP each
        avatar:setTP(tpReturn)
    else
        avatar:setTP(0)
    end
end

xi.job_utils.summoner.useManaCede = function(player, ability, action)
    local avatar = player:getPet()

    if avatar ~= nil then
        local avatarNewTP = xi.job_utils.summoner.manaCedeTPFromParams({
            avatarTP         = avatar:getTP(),
            manaCedeJP       = player:getJobPointLevel(xi.jp.MANA_CEDE_EFFECT),
            enhancesManaCede = player:getMod(xi.mod.ENHANCES_MANA_CEDE),
        })

        action:ID(player:getID(), avatar:getID())
        avatar:setTP(avatarNewTP)
        player:delMP(xi.job_utils.summoner.manaCedeMPCost)
    end
end

xi.job_utils.summoner.useSoothingRuby = function(target, pet, petskill, summoner, action)
    local targetEffectTable = target:getStatusEffects()

    -- Generate table with erasable effects from target effect table.
    local erasableEffectTable        = {}
    local additionalRemovableEffects =
    set{
        xi.effect.POISON,
        xi.effect.BLINDNESS,
        xi.effect.PARALYSIS,
        xi.effect.SILENCE,
        xi.effect.CURSE_I,
        xi.effect.PLAGUE,
        xi.effect.DISEASE
    }

    for _, effect in pairs(targetEffectTable) do
        local id = effect:getEffectType()
        if
            bit.band(effect:getEffectFlags(), xi.effectFlag.ERASABLE) == xi.effectFlag.ERASABLE or
            additionalRemovableEffects[id]
        then
            table.insert(erasableEffectTable, id)
        end
    end

    -- Calculate the ammount of effects this skill can potentialy erase.
    local soothingRubyPower = xi.job_utils.summoner.soothingRubyPowerFromParams(
        summoner:getSkillLevel(xi.skill.SUMMONING_MAGIC)
    )

    -- Erase effects.
    local effectsErased = xi.job_utils.summoner.soothingRubyEffectsErasedFromParams({
        power         = soothingRubyPower,
        erasableCount = #erasableEffectTable,
    })

    if effectsErased > 0 then
        for i = 1, effectsErased do
            local index = math.random(1, #erasableEffectTable)

            target:delStatusEffect(erasableEffectTable[index])
            table.remove(erasableEffectTable, index)
        end

        petskill:setMsg(xi.msg.basic.MAGIC_REMOVE_EFFECT_2)
    else
        petskill:setMsg(xi.msg.basic.JA_NO_EFFECT_2)
    end

    return effectsErased
end
