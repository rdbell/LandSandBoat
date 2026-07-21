-----------------------------------
-- Pure system tests for Ranger shot and FoV food status-effect scripts.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        jp = opts.jp or {},
        merits = opts.merits or {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getJobPointLevel = function(self, jp)
            return self.jp[jp] or 0
        end,
        getMerit = function(self, merit)
            return self.merits[merit] or 0
        end,
    }
end

local function stubEffect(power, sourceTypeParam)
    return {
        power = power or 0,
        sourceTypeParam = sourceTypeParam or 0,
        mods = {},
        getPower = function(self)
            return self.power
        end,
        getSourceTypeParam = function(self)
            return self.sourceTypeParam
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Ranger shot and FoV food status-effect pure plans', function()
    it('double and triple shot apply rate power plus JP', function()
        local d = require('scripts/effects/double_shot')
        local target = stubTarget({ jp = { [xi.jp.DOUBLE_SHOT_EFFECT] = 5 } })
        d.onEffectGain(target, stubEffect(40))
        assert(target.mods[xi.mod.DOUBLE_SHOT_RATE] == 45)
        local tr = require('scripts/effects/triple_shot')
        target = stubTarget({ jp = { [xi.jp.TRIPLE_SHOT_EFFECT] = 3 } })
        tr.onEffectGain(target, stubEffect(30))
        assert(target.mods[xi.mod.TRIPLE_SHOT_RATE] == 33)
    end)

    it('velocity shot applies effect-owned RATT trade-offs', function()
        local script = require('scripts/effects/velocity_shot')
        local target = stubTarget({ jp = { [xi.jp.VELOCITY_SHOT_EFFECT] = 4 } })
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.RATT] == 8)
        assert(effect.mods[xi.mod.ATTP] == -15)
        assert(effect.mods[xi.mod.HASTE_ABILITY] == -1500)
        assert(effect.mods[xi.mod.RATTP] == 15)
    end)

    it('unlimited shot reduces enmity and retains miss flag', function()
        local script = require('scripts/effects/unlimited_shot')
        local target = stubTarget({ jp = { [xi.jp.UNLIMITED_SHOT_EFFECT] = 3 } })
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.ENMITY] == -6)
        assert(effect.mods[xi.mod.RETAIN_UNLIMITED_SHOT] == 1)
    end)

    it('sharpshot stealth flashy and bounty apply expected mods', function()
        local sharp = require('scripts/effects/sharpshot')
        local target = stubTarget({ jp = { [xi.jp.SHARPSHOT_EFFECT] = 5 } })
        local effect = stubEffect(40)
        sharp.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.RACC] == 40)
        assert(effect.mods[xi.mod.RATT] == 10)

        local stealth = require('scripts/effects/stealth_shot')
        effect = stubEffect()
        target = stubTarget({ merits = { [xi.merit.STEALTH_SHOT] = 15 } })
        stealth.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.ENMITY] == -15)

        local flashy = require('scripts/effects/flashy_shot')
        effect = stubEffect()
        target = stubTarget({ merits = { [xi.merit.FLASHY_SHOT] = 10 } })
        flashy.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.RATTP] == 10)
        assert(effect.mods[xi.mod.ENMITY] == 50)

        local bounty = require('scripts/effects/bounty_shot')
        target = stubTarget()
        bounty.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.TREASURE_HUNTER] == 2)
    end)

    it('FoV food catalog applies when sourceTypeParam is zero', function()
        local script = require('scripts/effects/food')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect(1, 0)) -- Dried Meat
        assert(target.mods[xi.mod.STR] == 4)
        assert(target.mods[xi.mod.FOOD_ATTP] == 22)
        -- Item food: no mods
        target = stubTarget()
        script.onEffectGain(target, stubEffect(1, 4376))
        assert(target.mods[xi.mod.STR] == nil)
        -- Instant Noodles
        target = stubTarget()
        script.onEffectGain(target, stubEffect(4, 0))
        assert(target.mods[xi.mod.FOOD_HPP] == 27)
        assert(target.mods[xi.mod.STORETP] == 5)
        -- Seed Goblin
        target = stubTarget()
        script.onEffectGain(target, stubEffect(255, 0))
        assert(target.mods[xi.mod.STR] == -10)
        assert(target.mods[xi.mod.CHR] == -10)
    end)

    it('eagle eye and decoy shot are empty', function()
        for _, name in ipairs({ 'eagle_eye_shot', 'decoy_shot' }) do
            local script = require('scripts/effects/' .. name)
            local target = stubTarget()
            script.onEffectGain(target, stubEffect())
            local count = 0
            for _ in pairs(target.mods) do
                count = count + 1
            end
            assert(count == 0, name)
        end
    end)
end)
