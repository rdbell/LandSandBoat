-----------------------------------
-- Pure system tests for Bard song and song-ability status-effect scripts.
-----------------------------------

local function stubTarget()
    return {
        mods = {},
        jp = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getJobPointLevel = function(self, jp)
            return self.jp[jp] or 0
        end,
        getHP = function()
            return 100
        end,
        sendReraise = function() end,
    }
end

local function stubEffect(power, subPower, tier)
    return {
        power = power or 0,
        subPower = subPower or 0,
        tier = tier or 0,
        mods = {},
        getPower = function(self)
            return self.power
        end,
        getSubPower = function(self)
            return self.subPower
        end,
        getTier = function(self)
            return self.tier
        end,
        setPower = function(self, v)
            self.power = v
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Bard song status-effect pure plans', function()
    it('core songs apply power and augment subPower stats', function()
        local madrigal = require('scripts/effects/madrigal')
        local target = stubTarget()
        madrigal.onEffectGain(target, stubEffect(40, 5))
        assert(target.mods[xi.mod.ACC] == 40)
        assert(target.mods[xi.mod.DEX] == 5)
        madrigal.onEffectLose(target, stubEffect(40, 5))
        assert(target.mods[xi.mod.ACC] == 0)

        local minuet = require('scripts/effects/minuet')
        target = stubTarget()
        minuet.onEffectGain(target, stubEffect(50, 3))
        assert(target.mods[xi.mod.ATT] == 50)
        assert(target.mods[xi.mod.RATT] == 50)
        assert(target.mods[xi.mod.STR] == 3)

        local march = require('scripts/effects/march')
        target = stubTarget()
        march.onEffectGain(target, stubEffect(200, 2))
        assert(target.mods[xi.mod.HASTE_MAGIC] == 200)

        local ballad = require('scripts/effects/ballad')
        target = stubTarget()
        ballad.onEffectGain(target, stubEffect(4))
        assert(target.mods[xi.mod.REFRESH] == 4)

        local paeon = require('scripts/effects/paeon')
        target = stubTarget()
        paeon.onEffectGain(target, stubEffect(8, 1))
        assert(target.mods[xi.mod.REGEN] == 8)
        assert(target.mods[xi.mod.CHR] == 1)
    end)

    it('carol unpacks element and optional stat buff on effect', function()
        local script = require('scripts/effects/carol')
        local effect = stubEffect(40, 101) -- fire + buff 1
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.FIRE_MEVA] == 40)
        assert(effect.mods[xi.mod.STR] == 1)
        -- dark + buff 3 (308)
        effect = stubEffect(40, 308)
        script.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.DARK_MEVA] == 40)
        assert(effect.mods[xi.mod.MP] == 30)
    end)

    it('etude tier 2 decays power by 1 per tick', function()
        local script = require('scripts/effects/etude')
        local target = stubTarget()
        local effect = stubEffect(12, xi.mod.STR, 2)
        script.onEffectGain(target, effect)
        assert(target.mods[xi.mod.STR] == 12)
        script.onEffectTick(target, effect)
        assert(effect.power == 11)
        assert(target.mods[xi.mod.STR] == 11)
    end)

    it('elegy threnody mazurka and aubade are effect-owned', function()
        local elegy = require('scripts/effects/elegy')
        local effect = stubEffect(3000)
        elegy.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.HASTE_MAGIC] == -3000)

        local threnody = require('scripts/effects/threnody')
        effect = stubEffect(50, xi.mod.FIRE_MEVA)
        threnody.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.FIRE_MEVA] == -50)

        local mazurka = require('scripts/effects/mazurka')
        effect = stubEffect(24, 5)
        mazurka.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.MOVE_SPEED_MAZURKA] == 24)
        assert(effect.mods[xi.mod.AGI] == 5)

        local aubade = require('scripts/effects/aubade')
        effect = stubEffect(15, 3)
        aubade.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.SLEEPRES] == 15)
        assert(effect.mods[xi.mod.CHR] == 3)
    end)

    it('soul voice pianissimo and clarion call apply song casting mods', function()
        local sv = require('scripts/effects/soul_voice')
        local target = stubTarget()
        target.jp[xi.jp.SOUL_VOICE_EFFECT] = 5
        sv.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.SONG_SPELLCASTING_TIME] == 10)

        local pn = require('scripts/effects/pianissimo')
        target = stubTarget()
        target.jp[xi.jp.PIANISSIMO_EFFECT] = 4
        pn.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.SONG_SPELLCASTING_TIME] == 8)

        local cc = require('scripts/effects/clarion_call')
        target = stubTarget()
        cc.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.MAXIMUM_SONGS_BONUS] == 1)
    end)

    it('troubadour nightingale marcato lullaby and aria are empty', function()
        for _, name in ipairs({ 'troubadour', 'nightingale', 'marcato', 'lullaby', 'aria' }) do
            local script = require('scripts/effects/' .. name)
            local target = stubTarget()
            script.onEffectGain(target, stubEffect())
            script.onEffectLose(target, stubEffect())
            local count = 0
            for _ in pairs(target.mods) do
                count = count + 1
            end
            assert(count == 0, name)
        end
    end)
end)
