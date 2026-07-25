-----------------------------------
-- Global, independent functions for knockback calculations.
-- Pure inject dual-wired to OmegaXI internal/knockback (slice 6693 / 0911).
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.knockback = xi.combat.knockback or {}
-----------------------------------

xi.combat.knockback.levelNone = xi.action.knockback.NONE
xi.combat.knockback.level7 = xi.action.knockback.LEVEL7

-- Pure knockback level once skill knockback and reduction mod are injected.
-- params: skillKnockback, knockbackReductionMod
---@return xi.action.knockback
xi.combat.knockback.calculateFromParams = function(params)
    local skillKnockback = params.skillKnockback or 0
    local knockbackReductionMod = params.knockbackReductionMod or 0

    return utils.clamp(
        skillKnockback - knockbackReductionMod,
        xi.combat.knockback.levelNone,
        xi.combat.knockback.level7
    )
end

---@param target CBaseEntity
---@param attacker CBaseEntity
---@param skillOrSpell CMobSkill | CSpell
---@param action CAction
---@return xi.action.knockback
xi.combat.knockback.calculate = function(target, attacker, skillOrSpell, action)
    return xi.combat.knockback.calculateFromParams({
        skillKnockback         = skillOrSpell:getKnockback(),
        knockbackReductionMod  = target:getMod(xi.mod.KNOCKBACK_REDUCTION),
    })
end
