-----------------------------------
-- Tauri NM Mixin
-- Checks if Taurus has used Mortal Ray already and prevents multiple uses
--
-- Usage in mob files:
-- 1. Add mixin: mixins = { require('scripts/mixins/families/tauri') }
--
-- For custom skill selection, you can also manually call:
--   if xi.mix.tauri.canUseRay(mob) then
--     table.insert(tpMoves, xi.mobSkill.MORTAL_RAY_1)
--   end
-----------------------------------
require('scripts/globals/mixins')
xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.tauri = xi.mix.tauri or {}

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi.mix.tauri.canUseRay = function(mob)
    return xi.mix.tauri.canUseRayWithLocalVar(mob:getLocalVar('mortalRayUsed'))
end

xi.mix.tauri.canUseRayWithLocalVar = function(mortalRayUsed)
    -- Normal Tauri only use Mortal Ray once per life.
    return mortalRayUsed ~= 1
end

xi.mix.tauri.shouldRecordRay = function(skillID)
    return skillID == xi.mobSkill.MORTAL_RAY_1
end

g_mixins.families.tauri = function(tauriMob)
    tauriMob:addListener('WEAPONSKILL_USE', 'TAURI_NM_WEAPONSKILL_USE', function(mob, target, skill, tp, action, damage)
        if xi.mix.tauri.shouldRecordRay(skill:getID()) then
            mob:setLocalVar('mortalRayUsed', 1)
        end
    end)
end

return g_mixins.families.tauri
