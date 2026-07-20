-----------------------------------
-- Pure system tests for Summoner ability scripts outside job_utils.
-----------------------------------

local function stubPlayer(opts)
    opts = opts or {}
    local effects = opts.effects or {}
    return {
        effects = effects,
        mods = opts.mods or {},
        jp = opts.jp or {},
        skill = opts.skill or 0,
        mp = opts.mp or 0,
        maxMP = opts.maxMP or 1000,
        pet = opts.pet,
        added = nil,
        mpAdded = 0,
        hasStatusEffect = function(self, effect)
            return self.effects[effect] ~= nil
        end,
        getMod = function(self, mod)
            return self.mods[mod] or 0
        end,
        getJobPointLevel = function(self, jpId)
            return self.jp[jpId] or 0
        end,
        getSkillLevel = function(self, skill)
            return self.skill
        end,
        getMP = function(self)
            return self.mp
        end,
        getMaxMP = function(self)
            return self.maxMP
        end,
        addMP = function(self, amount)
            self.mpAdded = amount
            self.mp = self.mp + amount
            return amount
        end,
        getPet = function(self)
            return self.pet
        end,
        addStatusEffect = function(self, effect, params)
            self.added = { effect = effect, params = params }
            self.effects[effect] = true
        end,
    }
end

local function stubAbility()
    local recast = 3600
    return {
        getRecast = function()
            return recast
        end,
        setRecast = function(_, r)
            recast = r
        end,
        _getRecast = function()
            return recast
        end,
    }
end

local function stubSpirit(petID, mp)
    return {
        getPetID = function()
            return petID
        end,
        getMP = function()
            return mp
        end,
        delMP = function(self, amount)
            self._del = amount
        end,
        _del = 0,
    }
end

describe('Summoner job ability pure plans', function()
    it('apogee refuses when active and applies power 1 duration 60', function()
        local skill = require('scripts/actions/abilities/apogee')
        local active = stubPlayer({ effects = { [xi.effect.APOGEE] = true } })
        assert(skill.onAbilityCheck(active, active, stubAbility()) == xi.msg.basic.EFFECT_ALREADY_ACTIVE)
        local player = stubPlayer()
        assert(skill.onAbilityCheck(player, player, stubAbility()) == 0)
        assert(skill.onUseAbility(player, player, stubAbility()) == xi.effect.APOGEE)
        assert(player.added.params.power == 1 and player.added.params.duration == 60)
    end)

    it('astral flow reduces one-hour recast and lasts 180s', function()
        local skill = require('scripts/actions/abilities/astral_flow')
        local player = stubPlayer({ mods = { [xi.mod.ONE_HOUR_RECAST] = 2 } })
        local ability = stubAbility()
        assert(skill.onAbilityCheck(player, player, ability) == 0)
        assert(ability._getRecast() == 3480)
        assert(skill.onUseAbility(player, player, ability) == xi.effect.ASTRAL_FLOW)
        assert(player.added.params.power == 1 and player.added.params.duration == 180)
    end)

    it('astral conduit applies power 15 duration 30 tick 1', function()
        local skill = require('scripts/actions/abilities/astral_conduit')
        local player = stubPlayer()
        skill.onUseAbility(player, player, stubAbility())
        assert(player.added.effect == xi.effect.ASTRAL_CONDUIT)
        assert(player.added.params.power == 15)
        assert(player.added.params.duration == 30)
        assert(player.added.params.tick == 1)
    end)

    it('avatars favor applies power 1 duration 7200 tick 10', function()
        local skill = require('scripts/actions/abilities/avatars_favor')
        local player = stubPlayer()
        skill.onUseAbility(player, player, stubAbility())
        assert(player.added.effect == xi.effect.AVATARS_FAVOR)
        assert(player.added.params.power == 1)
        assert(player.added.params.duration == 7200)
        assert(player.added.params.tick == 10)
    end)

    it('elemental siphon requires an elemental spirit pet', function()
        local skill = require('scripts/actions/abilities/elemental_siphon')
        local none = stubPlayer()
        assert(skill.onAbilityCheck(none, none, stubAbility()) == xi.msg.basic.UNABLE_TO_USE_JA)
        local avatar = stubPlayer({ pet = stubSpirit(20, 100) }) -- not a spirit
        assert(skill.onAbilityCheck(avatar, avatar, stubAbility()) == xi.msg.basic.UNABLE_TO_USE_JA)
        local spirit = stubPlayer({ pet = stubSpirit(xi.petId.FIRE_SPIRIT, 100) })
        assert(skill.onAbilityCheck(spirit, spirit, stubAbility()) == 0)
    end)

    it('elemental siphon power product and MP clamps', function()
        local skill = require('scripts/actions/abilities/elemental_siphon')
        -- Stub day/weather to identity so potency is deterministic.
        local orig = xi.spells.damage.calculateDayAndWeather
        xi.spells.damage.calculateDayAndWeather = function()
            return 1.0
        end

        local spirit = stubSpirit(xi.petId.FIRE_SPIRIT, 50)
        local player = stubPlayer({
            pet = spirit,
            skill = 100,
            mods = { [xi.mod.ENHANCES_ELEMENTAL_SIPHON] = 20 },
            jp = { [xi.jp.ELEMENTAL_SIPHON_EFFECT] = 0 },
            mp = 990,
            maxMP = 1000,
        })
        -- power = floor(100*1.05 + 20 - 55) = 70; clamp spirit 50, room 10 → 10
        local ret = skill.onUseAbility(player, player, stubAbility())
        assert(ret == 10)
        assert(spirit._del == 10)
        assert(player.mpAdded == 10)

        xi.spells.damage.calculateDayAndWeather = orig
    end)
end)
