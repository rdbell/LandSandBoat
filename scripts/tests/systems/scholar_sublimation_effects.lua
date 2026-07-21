-----------------------------------
-- Pure system tests for Scholar Sublimation status-effect scripts.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = opts.mods or {},
        effects = opts.effects or {},
        deletedEffects = {},
        mainJob = opts.mainJob or xi.job.SCH,
        mainLvl = opts.mainLvl or 75,
        subLvl = opts.subLvl or 0,
        hpp = opts.hpp or 100,
        baseHP = opts.baseHP or 1000,
        merits = opts.merits or {},
        jp = opts.jp or {},
        damageTaken = 0,
        delStatusEffect = function(self, effect)
            table.insert(self.deletedEffects, effect)
            self.effects[effect] = nil
        end,
        delStatusEffectSilent = function(self, effect)
            table.insert(self.deletedEffects, effect)
            self.effects[effect] = nil
        end,
        hasStatusEffect = function(self, effect)
            return self.effects[effect] == true
        end,
        addStatusEffect = function(self, effect, params)
            self.effects[effect] = true
            self.lastAdd = { effect = effect, params = params }
        end,
        getMainJob = function(self)
            return self.mainJob
        end,
        getMainLvl = function(self)
            return self.mainLvl
        end,
        getSubLvl = function(self)
            return self.subLvl
        end,
        getMod = function(self, mod)
            return self.mods[mod] or 0
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getHPP = function(self)
            return self.hpp
        end,
        getBaseHP = function(self)
            return self.baseHP
        end,
        getMerit = function(self, merit)
            return self.merits[merit] or 0
        end,
        getJobPointLevel = function(self, jp)
            return self.jp[jp] or 0
        end,
        takeDamage = function(self, dmg)
            self.damageTaken = self.damageTaken + dmg
            -- crude HPP drop for tests: 1 dmg ≈ 1% when base 100
            self.hpp = math.max(0, self.hpp - dmg)
        end,
    }
end

local function stubEffect(power)
    return {
        power = power or 0,
        getPower = function(self)
            return self.power
        end,
        setPower = function(self, v)
            self.power = v
        end,
    }
end

describe('Scholar sublimation status-effect pure plans', function()
    it('activated gain clears refresh', function()
        local script = require('scripts/effects/sublimation_activated')
        local target = stubTarget({ effects = { [xi.effect.REFRESH] = true } })
        script.onEffectGain(target, stubEffect())
        assert(target.deletedEffects[1] == xi.effect.REFRESH)
    end)

    it('complete gain clears refresh', function()
        local script = require('scripts/effects/sublimation_complete')
        local target = stubTarget({ effects = { [xi.effect.REFRESH] = true } })
        script.onEffectGain(target, stubEffect())
        assert(target.deletedEffects[1] == xi.effect.REFRESH)
    end)

    it('activated tick stores MP and damages HP while above 50%', function()
        local script = require('scripts/effects/sublimation_activated')
        local target = stubTarget({
            mainLvl = 75,
            hpp = 80,
            baseHP = 1000,
            mods = { [xi.mod.SUBLIMATION_BONUS] = 0 },
        })
        local effect = stubEffect(10)
        script.onEffectTick(target, effect)
        -- basemp floor(60/10)=6; store 10+6=16; dmg 2
        assert(effect.power == 16)
        assert(target.damageTaken == 2)
        assert(target.effects[xi.effect.SUBLIMATION_COMPLETE] == nil)
    end)

    it('activated tick completes when store reaches limit', function()
        local script = require('scripts/effects/sublimation_activated')
        local target = stubTarget({
            mainLvl = 75,
            hpp = 90,
            baseHP = 1000, -- limit 500
        })
        -- store = 495 + 6 = 501 → clamp 500 complete
        local effect = stubEffect(495)
        script.onEffectTick(target, effect)
        assert(target.lastAdd ~= nil)
        assert(target.lastAdd.effect == xi.effect.SUBLIMATION_COMPLETE)
        assert(target.lastAdd.params.power == 500)
        assert(target.lastAdd.params.duration == 7200)
    end)

    it('activated tick completes immediately when HPP under 51', function()
        local script = require('scripts/effects/sublimation_activated')
        local target = stubTarget({ mainLvl = 75, hpp = 40, baseHP = 1000 })
        local effect = stubEffect(10)
        script.onEffectTick(target, effect)
        assert(target.damageTaken == 0)
        assert(target.lastAdd.effect == xi.effect.SUBLIMATION_COMPLETE)
        assert(target.lastAdd.params.power == 16)
    end)

    it('activated tick absorbs damage with stoneskin', function()
        local script = require('scripts/effects/sublimation_activated')
        local target = stubTarget({
            mainLvl = 75,
            hpp = 90,
            baseHP = 1000,
            effects = { [xi.effect.STONESKIN] = true },
            mods = { [xi.mod.STONESKIN] = 10, [xi.mod.SUBLIMATION_BONUS] = 0 },
        })
        local effect = stubEffect(0)
        script.onEffectTick(target, effect)
        assert(target.damageTaken == 0)
        assert(target.mods[xi.mod.STONESKIN] == 8) -- 10 - 2
        assert(effect.power == 6)
    end)
end)
