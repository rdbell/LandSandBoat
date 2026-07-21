-----------------------------------
-- Pure system tests for Skillchain effect lose and Sengikori WS apply.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        hasSC = opts.hasSC or false,
        effects = opts.effects or {},
        setMod = function(self, mod, value)
            self.mods[mod] = value
        end,
        getMod = function(self, mod)
            return self.mods[mod] or 0
        end,
        hasStatusEffect = function(self, effect)
            if effect == xi.effect.SKILLCHAIN then
                return self.hasSC
            end
            return self.effects[effect] ~= nil
        end,
        getStatusEffect = function(self, effect)
            return self.effects[effect]
        end,
        delStatusEffect = function(self, effect)
            self.effects[effect] = nil
        end,
    }
end

-- Pure mirror of weaponskills.lua Sengikori apply (~990–1011).
local function planSengikoriWSApply(hitsLanded, attacker, defender)
    local sengikoriEffect = attacker:getStatusEffect(xi.effect.SENGIKORI)
    if hitsLanded <= 0 or sengikoriEffect == nil then
        return { apply = false }
    end
    local power = sengikoriEffect:getPower() + attacker:getMod(xi.mod.SENGIKORI_BONUS)
    if defender:hasStatusEffect(xi.effect.SKILLCHAIN) then
        defender:setMod(xi.mod.SENGIKORI_MB_DMG_DEBUFF, power)
    else
        defender:setMod(xi.mod.SENGIKORI_SC_DMG_DEBUFF, power)
    end
    attacker:delStatusEffect(xi.effect.SENGIKORI)
    return { apply = true, power = power }
end

describe('Sengikori and Skillchain status-effect pure plans', function()
    it('skillchain lose zeros both Sengikori debuff mods', function()
        local script = require('scripts/effects/skillchain')
        local target = stubTarget()
        target.mods[xi.mod.SENGIKORI_SC_DMG_DEBUFF] = 30
        target.mods[xi.mod.SENGIKORI_MB_DMG_DEBUFF] = 40
        script.onEffectLose(target, {})
        assert(target.mods[xi.mod.SENGIKORI_SC_DMG_DEBUFF] == 0)
        assert(target.mods[xi.mod.SENGIKORI_MB_DMG_DEBUFF] == 0)
    end)

    it('sengikori WS apply sets SC debuff when no skillchain', function()
        local effect = {
            power = 25,
            getPower = function(self)
                return self.power
            end,
        }
        local attacker = stubTarget({
            effects = { [xi.effect.SENGIKORI] = effect },
        })
        attacker.mods[xi.mod.SENGIKORI_BONUS] = 5
        local defender = stubTarget({ hasSC = false })
        local plan = planSengikoriWSApply(2, attacker, defender)
        assert(plan.apply)
        assert(plan.power == 30)
        assert(defender.mods[xi.mod.SENGIKORI_SC_DMG_DEBUFF] == 30)
        assert(defender.mods[xi.mod.SENGIKORI_MB_DMG_DEBUFF] == nil or defender.mods[xi.mod.SENGIKORI_MB_DMG_DEBUFF] == 0)
        assert(attacker:getStatusEffect(xi.effect.SENGIKORI) == nil)
    end)

    it('sengikori WS apply sets MB debuff when skillchain present', function()
        local effect = {
            power = 25,
            getPower = function(self)
                return self.power
            end,
        }
        local attacker = stubTarget({
            effects = { [xi.effect.SENGIKORI] = effect },
        })
        local defender = stubTarget({ hasSC = true })
        local plan = planSengikoriWSApply(1, attacker, defender)
        assert(plan.apply)
        assert(plan.power == 25)
        assert(defender.mods[xi.mod.SENGIKORI_MB_DMG_DEBUFF] == 25)
        assert(defender.mods[xi.mod.SENGIKORI_SC_DMG_DEBUFF] == nil or defender.mods[xi.mod.SENGIKORI_SC_DMG_DEBUFF] == 0)
    end)

    it('sengikori WS apply skips when no hits or no effect', function()
        local effect = {
            power = 25,
            getPower = function(self)
                return self.power
            end,
        }
        local attacker = stubTarget({
            effects = { [xi.effect.SENGIKORI] = effect },
        })
        local defender = stubTarget()
        assert(not planSengikoriWSApply(0, attacker, defender).apply)
        assert(attacker:getStatusEffect(xi.effect.SENGIKORI) ~= nil)

        attacker = stubTarget() -- no sengikori
        assert(not planSengikoriWSApply(3, attacker, defender).apply)
    end)

    it('effect IDs and mod IDs match enums', function()
        assert(xi.effect.SENGIKORI == 440)
        assert(xi.effect.SKILLCHAIN == 799)
        assert(xi.mod.SENGIKORI_SC_DMG_DEBUFF == 1088)
        assert(xi.mod.SENGIKORI_MB_DMG_DEBUFF == 1089)
        assert(xi.mod.SENGIKORI_BONUS == 1090)
    end)
end)
