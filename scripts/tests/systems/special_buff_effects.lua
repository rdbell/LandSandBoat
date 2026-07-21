-----------------------------------
-- Pure system tests for special buff/debuff status-effect scripts.
-----------------------------------

local function stubTarget()
    return {
        mods = {},
        anim = 0,
        debil = 0,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        setAnimationSub = function(self, v)
            self.anim = v
        end,
        setStatDebilitation = function(self, v)
            self.debil = v
        end,
    }
end

local function stubEffect(opts)
    opts = opts or {}
    return {
        power = opts.power or 0,
        subPower = opts.subPower or 0,
        duration = opts.duration or 60,
        tick = opts.tick or 3,
        tickCount = opts.tickCount or 0,
        mods = {},
        getPower = function(self)
            return self.power
        end,
        getSubPower = function(self)
            return self.subPower
        end,
        setPower = function(self, v)
            self.power = v
        end,
        setSubPower = function(self, v)
            self.subPower = v
        end,
        getDuration = function(self)
            return self.duration
        end,
        getTick = function(self)
            return self.tick
        end,
        getTickCount = function(self)
            return self.tickCount
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Special buff status-effect pure plans', function()
    it('transcendency applies fixed HP/MP/stat catalog', function()
        local script = require('scripts/effects/transcendency')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.HP] == 9000)
        assert(target.mods[xi.mod.MP] == 9000)
        assert(target.mods[xi.mod.STR] == 900)
        assert(target.mods[xi.mod.ATT] == 9000)
        assert(target.mods[xi.mod.RATT] == 9000)
        script.onEffectLose(target, stubEffect())
        assert(target.mods[xi.mod.HP] == 0)
    end)

    it('super buff applies effect mods and animation sub', function()
        local script = require('scripts/effects/super_buff')
        local target = stubTarget()
        local effect = stubEffect()
        script.onEffectGain(target, effect)
        assert(effect.mods[xi.mod.ATTP] == 25)
        assert(effect.mods[xi.mod.DMGPHYS] == -5000)
        assert(effect.mods[xi.mod.EVA] == 378)
        assert(target.anim == 2)
        script.onEffectLose(target, effect)
        assert(target.anim == 0)
    end)

    it('perfect defense applies UDMG and status MEVA; ticks decay after half', function()
        local script = require('scripts/effects/perfect_defense')
        local target = stubTarget()
        local effect = stubEffect({ power = 3000, subPower = 30 })
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.UDMGPHYS] == -3000)
        assert(target.mods[xi.mod.UDMGBREATH] == -3000)
        assert(target.mods[xi.mod.SLEEP_MEVA] == 30)
        assert(target.mods[xi.mod.VIRUS_MEVA] == 30)
        assert(target.mods[xi.mod.LULLABY_MEVA] == nil)

        -- Before half: duration 60 tick 3 → threshold 10
        effect.tickCount = 9
        script.onEffectTick(target, effect)
        assert(effect.power == 3000)

        effect.tickCount = 10
        script.onEffectTick(target, effect)
        assert(effect.power == 2400)
        assert(effect.subPower == 24)
        -- delMod(UDMGPHYS, -600) subtracts -600 → adds 600 to current -3000 → -2400
        assert(target.mods[xi.mod.UDMGPHYS] == -2400)
        assert(target.mods[xi.mod.SLEEP_MEVA] == 24)
    end)

    it('debilitation applies bitfield main-stat and HPP/MPP penalties', function()
        local script = require('scripts/effects/debilitation')
        local target = stubTarget()
        -- STR + HPP bits
        script.onEffectGain(target, stubEffect({ power = 0x081 }))
        assert(target.mods[xi.mod.STR] == -30)
        assert(target.mods[xi.mod.HPP] == -40)
        assert(target.debil == 0x081)
        script.onEffectLose(target, stubEffect({ power = 0x081 }))
        assert(target.mods[xi.mod.STR] == 0)
        assert(target.debil == 0)
    end)

    it('elemental resistance down, gestation, and shining ruby', function()
        local erd = require('scripts/effects/elemental_resistance_down')
        local target = stubTarget()
        erd.onEffectGain(target, stubEffect({ power = 20 }))
        assert(target.mods[xi.mod.FIRE_MEVA] == -20)
        assert(target.mods[xi.mod.DARK_MEVA] == -20)

        local gest = require('scripts/effects/gestation')
        local effect = stubEffect()
        gest.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.MOVE_SPEED_STACKABLE] == 50)

        local ruby = require('scripts/effects/shining_ruby')
        target = stubTarget()
        ruby.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.DEFP] == 10)
        assert(target.mods[xi.mod.MDEF] == 4)
    end)
end)
