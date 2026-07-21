-----------------------------------
-- Pure system tests for Corsair phantom-roll status-effect scripts.
-----------------------------------

local function stubTarget(opts)
    opts = opts or {}
    return {
        mods = {},
        petMods = {},
        effects = opts.effects or {},
        localVars = opts.localVars or {},
        deletedEffects = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        addPetMod = function(self, mod, delta)
            self.petMods[mod] = (self.petMods[mod] or 0) + delta
        end,
        delPetMod = function(self, mod, delta)
            self.petMods[mod] = (self.petMods[mod] or 0) - delta
        end,
        hasStatusEffect = function(self, effect)
            return self.effects[effect] == true
        end,
        delStatusEffectSilent = function(self, effect)
            table.insert(self.deletedEffects, effect)
        end,
        getLocalVar = function(self, key)
            return self.localVars[key] or 0
        end,
        setLocalVar = function(self, key, value)
            self.localVars[key] = value
        end,
    }
end

local function stubEffect(power, effectType)
    return {
        power = power or 0,
        effectType = effectType or 0,
        mods = {},
        getPower = function(self)
            return self.power
        end,
        getEffectType = function(self)
            return self.effectType
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Corsair roll status-effect pure plans', function()
    it('standard rolls apply effect-owned power to expected mods', function()
        local cases = {
            { 'fighters_roll', xi.mod.DOUBLE_ATTACK },
            { 'monks_roll', xi.mod.SUBTLE_BLOW },
            { 'healers_roll', xi.mod.CURE_POTENCY_RCVD },
            { 'wizards_roll', xi.mod.MATT },
            { 'warlocks_roll', xi.mod.MACC },
            { 'rogues_roll', xi.mod.CRITHITRATE },
            { 'samurai_roll', xi.mod.STORETP },
            { 'ninja_roll', xi.mod.EVA },
            { 'evokers_roll', xi.mod.REFRESH },
            { 'maguss_roll', xi.mod.MDEF },
            { 'dancers_roll', xi.mod.REGEN },
            { 'scholars_roll', xi.mod.CONSERVE_MP },
            { 'bolters_roll', xi.mod.MOVE_SPEED_BOLTERS_ROLL },
            { 'casters_roll', xi.mod.FASTCAST },
            { 'blitzers_roll', xi.mod.DELAYP },
            { 'tacticians_roll', xi.mod.REGAIN },
            { 'allies_roll', xi.mod.SKILLCHAINBONUS },
            { 'misers_roll', xi.mod.SAVETP },
            { 'avengers_roll', xi.mod.COUNTER },
            { 'naturalists_roll', xi.mod.ENH_MAGIC_DURATION },
            { 'runeists_roll', xi.mod.MEVA },
            { 'choral_roll', xi.mod.SPELLINTERRUPT },
        }
        for _, c in ipairs(cases) do
            local script = require('scripts/effects/' .. c[1])
            local effect = stubEffect(11)
            script.onEffectGain(stubTarget(), effect)
            assert(effect.mods[c[2]] == 11, c[1])
        end
    end)

    it('chaos and hunters apply dual mods; corsairs applies exp and capacity', function()
        local chaos = require('scripts/effects/chaos_roll')
        local effect = stubEffect(25)
        chaos.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.ATTP] == 25)
        assert(effect.mods[xi.mod.RATTP] == 25)

        local hunters = require('scripts/effects/hunters_roll')
        effect = stubEffect(15)
        hunters.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.ACC] == 15)
        assert(effect.mods[xi.mod.RACC] == 15)

        local corsairs = require('scripts/effects/corsairs_roll')
        effect = stubEffect(20)
        corsairs.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.EXP_BONUS] == 20)
        assert(effect.mods[xi.mod.CAPACITY_BONUS] == 20)

        -- Gallants reduces damage taken (DMG -power)
        local gallants = require('scripts/effects/gallants_roll')
        effect = stubEffect(11)
        gallants.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.DMG] == -11)

        -- Coursers SNAPSHOT is disabled (TODO) upstream
        local coursers = require('scripts/effects/coursers_roll')
        effect = stubEffect(11)
        coursers.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.SNAPSHOT] == nil)
    end)

    it('beast drachen puppet apply pet mods', function()
        local beast = require('scripts/effects/beast_roll')
        local target = stubTarget()
        beast.onEffectGain(target, stubEffect(30))
        assert(target.petMods[xi.mod.ATTP] == 30)
        assert(target.petMods[xi.mod.RATTP] == 30)
        beast.onEffectLose(target, stubEffect(30))
        assert(target.petMods[xi.mod.ATTP] == 0)

        local drachen = require('scripts/effects/drachen_roll')
        target = stubTarget()
        drachen.onEffectGain(target, stubEffect(20))
        assert(target.petMods[xi.mod.ACC] == 20)
        assert(target.petMods[xi.mod.RACC] == 20)

        local puppet = require('scripts/effects/puppet_roll')
        target = stubTarget()
        puppet.onEffectGain(target, stubEffect(15))
        assert(target.petMods[xi.mod.MATT] == 15)
        assert(target.petMods[xi.mod.MACC] == 15)
    end)

    it('companions roll decodes power into pet regain and regen', function()
        local script = require('scripts/effects/companions_roll')
        -- power 2 → face 2 base: regain 50, regen 20
        local target = stubTarget()
        script.onEffectGain(target, stubEffect(2))
        assert(target.petMods[xi.mod.REGAIN] == 50)
        assert(target.petMods[xi.mod.REGEN] == 20)
        -- power 72 → face 2 + max bonuses: 50+35, 20+14
        target = stubTarget()
        script.onEffectGain(target, stubEffect(72))
        assert(target.petMods[xi.mod.REGAIN] == 85)
        assert(target.petMods[xi.mod.REGEN] == 34)
        -- power 25 fall-through: no pet mods
        target = stubTarget()
        script.onEffectGain(target, stubEffect(25))
        assert(target.petMods[xi.mod.REGAIN] == nil)
    end)

    it('onRollEffectLose clears double-up when matching and not applying', function()
        local corsair = xi.job_utils.corsair
        local target = stubTarget({
            effects = { [xi.effect.DOUBLE_UP_CHANCE] = true },
            localVars = { corsairDuEffect = 123, corsairApplyingRoll = 0 },
        })
        local effect = stubEffect(10, 123)
        corsair.onRollEffectLose(target, effect)
        assert(#target.deletedEffects == 1)
        assert(target.deletedEffects[1] == xi.effect.DOUBLE_UP_CHANCE)
        assert(target.localVars.corsairDuEffect == 0)

        -- skip while applying roll
        target = stubTarget({
            effects = { [xi.effect.DOUBLE_UP_CHANCE] = true },
            localVars = { corsairDuEffect = 123, corsairApplyingRoll = 1 },
        })
        corsair.onRollEffectLose(target, effect)
        assert(#target.deletedEffects == 0)
    end)
end)
