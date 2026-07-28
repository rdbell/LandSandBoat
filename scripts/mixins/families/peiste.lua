-- Peiste family mixin
-- Gaze-Aura effects: certain skills will cause the mob to have glowy eyes, which then afflicts effects in a gaze during the duration of the glowy eyes
--     Blind will short-circuit the gaze animation and effects

require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.peiste = xi.mix.peiste or {}

-- Maps mobskill IDs to animation sub
local gazeAnimationSubs =
{
    [xi.mobSkill.GRIM_GLOWER] = 1,
    [xi.mobSkill.OPPRESSIVE_GLARE] = 5,
}

-- Maps animation sub to gaze-aura effects
-- Will default to power = 1 and duration = 15
local gazeEffects =
{
    -- red eyes
    [1] =
    {
        [xi.effect.PETRIFICATION] =
        {
            duration = 15,
        },
    },

    -- Orange eyes
    -- TODO verify this data for Oppressive Glare when skill is coded
    [5] =
    {
        [xi.effect.CURSE_II] =
        {
            power = 0,
            duration = 30,
        },
        [xi.effect.TERROR] =
        {
            duration = 30,
        },
    },
}

xi.mix.peiste.gazePlan = function(skillID, isBlinded, duration)
    local animationSub = gazeAnimationSubs[skillID]
    if animationSub and not isBlinded then
        return { animationSub = animationSub, duration = duration }
    end
    return nil
end

xi.mix.peiste.effectsFor = function(animationSub)
    local effects = gazeEffects[animationSub]
    if not effects then
        return nil
    end

    local resolved = {}
    for effect, info in pairs(effects) do
        resolved[effect] = { power = info.power or 1, duration = info.duration or 15 }
    end
    return resolved
end

g_mixins.families.peiste = function(peisteMob)
    peisteMob:addListener('WEAPONSKILL_USE', 'PEISTE_MIXIN_WS_USE', function(mob, target, skill, tp, action, damage)
        local plan = xi.mix.peiste.gazePlan(skill:getID(), mob:getStatusEffect(xi.effect.BLINDNESS), 0)
        if plan then
            -- could be moved to mobskill luas with skill:setFinalAnimationSub(), but leaving here due to reliance on the mapping table
            -- Sets glowy eyes, which triggers the combat tick aura gaze
            mob:setAnimationSub(plan.animationSub)
            mob:timer(math.random(30, 45) * 1000, function(mobArg)
                mobArg:setAnimationSub(0)
            end)
        end
    end)

    peisteMob:addListener('COMBAT_TICK', 'PEISTE_MIXIN_CTICK', function(mob, target)
        local gazeData = xi.mix.peiste.effectsFor(mob:getAnimationSub())
        if gazeData then
            if mob:getStatusEffect(xi.effect.BLINDNESS) then
                -- Note that testing this manually will break the animation for the game client
                -- (i.e. setAnimationSub(1) with blindness up will correctly remove the animation sub immediately, but the game client won't render the update)
                mob:setAnimationSub(0)
            else
                for _, entityObj in pairs(mob:getEnmityList()) do
                    local entity = entityObj['entity']
                    -- TODO verify exact range of gaze
                    if mob:checkDistance(entity) < 10 then
                        for gazeEffect, gazeEffectInfo in pairs(gazeData) do
                            if not entity:getStatusEffect(gazeEffect) then
                                xi.mobskills.mobGazeMove(mob, entity, gazeEffect, gazeEffectInfo.power, 3, gazeEffectInfo.duration)
                            end
                        end
                    end
                end
            end
        end
    end)
end

return g_mixins.families.peiste
