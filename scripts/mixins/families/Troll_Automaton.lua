-----------------------------------
require('scripts/globals/mixins')
-----------------------------------
xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.trollAutomaton = xi.mix.trollAutomaton or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

local automatonTypes =
{
    {
        name = 'Harlequin',
        job = xi.job.RDM,
        modelId = 1977,
        isCaster = true,
        spellList = 567,
    },

    {
        name = 'Valoredge',
        job = xi.job.PLD,
        modelId = 1983,
        doubleAttack = 15,
    },

    {
        name = 'Sharpshot',
        job = xi.job.RNG,
        modelId = 1990,
        isRanged = true,
        standbackHp = 70,
        specialCool = 12,
        specialSkill = xi.mobSkill.RANGED_ATTACK_1,
        rangedAttackRange = 13,
    },

    {
        name = 'Stormwalker',
        job = xi.job.RDM,
        modelId = 1994,
        isCaster = true,
        spellList = 566,
        standbackHp = 70,
    },
}

-- Returns the fully-defaulted setup plan for a frame. magicDelay is sampled by
-- the caller so the deterministic configuration is independently testable.
xi.mix.trollAutomaton.framePlan = function(frameIndex, magicDelay)
    local automatonType = automatonTypes[frameIndex]

    if not automatonType then
        return nil
    end

    return {
        name              = automatonType.name,
        job               = automatonType.job,
        modelId           = automatonType.modelId,
        doubleAttack      = automatonType.doubleAttack or 0,
        magicCool         = 27,
        magicDelay        = magicDelay,
        spellList         = automatonType.spellList or 0,
        magicCasting      = automatonType.isCaster or false,
        standbackHp       = automatonType.standbackHp or 0,
        specialCool       = automatonType.specialCool or 0,
        specialSkill      = automatonType.specialSkill or 0,
        rangedAttackRange = automatonType.rangedAttackRange or 0,
    }
end

xi.mix.trollAutomaton.skillPool = function(modelId)
    if modelId == 1977 then -- Harlequin
        return { xi.mobSkill.SLAPSTICK_AUTOMATON }
    elseif modelId == 1983 then -- Valoredge
        return {
            xi.mobSkill.CHIMERA_RIPPER_AUTOMATON,
            xi.mobSkill.STRING_CLIPPER_AUTOMATON,
            xi.mobSkill.SHIELD_BASH_AUTOMATON,
        }
    elseif modelId == 1990 then -- Sharpshot
        return { xi.mobSkill.SLAPSTICK_AUTOMATON, xi.mobSkill.ARCUBALLISTA_AUTOMATON }
    elseif modelId == 1994 then -- Stormwalker
        return { xi.mobSkill.SLAPSTICK_AUTOMATON }
    end

    return {}
end

g_mixins.families.Troll_Automaton = function(automatonMob)
    automatonMob:addListener('SPAWN', 'TROLL_AUTOMATON_SPAWN', function(mob)
        xi.mix.trollAutomaton.setupAutomaton(mob, xi.mix.trollAutomaton.framePlan(math.random(1, #automatonTypes), math.random(3, 7)))
    end)
end

xi.mix.trollAutomaton.setupAutomaton = function(mob, automatonType)
    mob:setLocalVar('automatonTypeModelId', automatonType.modelId)
    mob:setModelId(automatonType.modelId)
    mob:changeJob(automatonType.job)
    mob:setDelay(270) -- All Frames (Waiting for delay conversion PR to be merged)
    mob:setMod(xi.mod.DOUBLE_ATTACK, automatonType.doubleAttack or 0) -- Valoredge Frame
    mob:setMobMod(xi.mobMod.MAGIC_COOL, automatonType.magicCool) -- Harlequin and Stormwalker
    mob:setMobMod(xi.mobMod.MAGIC_DELAY, automatonType.magicDelay) -- Harlequin and Stormwalker
    mob:setSpellList(automatonType.spellList) -- Harlequin and Stormwalker
    mob:setMagicCastingEnabled(automatonType.magicCasting) -- Harlequin & Stormwalker
    mob:setBehavior(bit.band(mob:getBehavior(), bit.bnot(xi.behavior.STANDBACK))) -- Sharpshot and Stormwalker
    mob:setMobMod(xi.mobMod.HP_STANDBACK, automatonType.standbackHp) -- Sharpshot and Stormwalker
    mob:setMobMod(xi.mobMod.SPECIAL_COOL, automatonType.specialCool) -- Sharpshot Frame
    mob:setMobMod(xi.mobMod.SPECIAL_SKILL, automatonType.specialSkill) -- Sharpshot Frame
    mob:setMobMod(xi.mobMod.RANGED_ATTACK_RANGE, automatonType.rangedAttackRange) -- Sharpshot Frame

    return automatonType
end

xi.mix.trollAutomaton.onMobMobskillChoose = function(mob, target)
    local modelId = mob:getModelId()
    local skillList = xi.mix.trollAutomaton.skillPool(modelId)

    if #skillList == 0 then
        return 0
    end

    return skillList[math.random(1, #skillList)]
end

return g_mixins.families.Troll_Automaton
