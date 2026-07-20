-----------------------------------
-- Pure system tests for remaining simple job ability scripts:
-- Embolden, Elemental Sforzo, Entrust, Crooked Cards, Triple Shot,
-- Caper Emissarius, Libra.
-----------------------------------

local function stubPlayer(opts)
    opts = opts or {}
    return {
        mods = opts.mods or {},
        id = opts.id or 1,
        isPCFlag = opts.isPC ~= false,
        added = nil,
        getMod = function(self, mod)
            return self.mods[mod] or 0
        end,
        getID = function(self)
            return self.id
        end,
        isPC = function(self)
            return self.isPCFlag
        end,
        addStatusEffect = function(self, effect, params)
            self.added = { effect = effect, params = params }
        end,
        transferEnmity = function(self, actor, pct, range)
            self.transfer = { actor = actor, pct = pct, range = range }
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

describe('Misc job ability pure plans', function()
    it('embolden applies duration 60', function()
        local skill = require('scripts/actions/abilities/embolden')
        local player = stubPlayer()
        skill.onUseAbility(player, player, stubAbility(), {})
        assert(player.added.effect == xi.effect.EMBOLDEN)
        assert(player.added.params.duration == 60)
    end)

    it('elemental sforzo reduces one-hour recast and lasts 30s', function()
        local skill = require('scripts/actions/abilities/elemental_sforzo')
        local player = stubPlayer({ mods = { [xi.mod.ONE_HOUR_RECAST] = 1 } })
        local ability = stubAbility()
        assert(skill.onAbilityCheck(player, player, ability) == 0)
        assert(ability._getRecast() == 3540)
        assert(skill.onUseAbility(player, player, ability) == xi.effect.ELEMENTAL_SFORZO)
        assert(player.added.params.power == 1 and player.added.params.duration == 30)
    end)

    it('entrust applies power 1 duration 60', function()
        local skill = require('scripts/actions/abilities/entrust')
        local player = stubPlayer()
        skill.onUseAbility(player, player, stubAbility())
        assert(player.added.effect == xi.effect.ENTRUST)
        assert(player.added.params.power == 1 and player.added.params.duration == 60)
    end)

    it('crooked cards applies power 20 duration 60', function()
        local skill = require('scripts/actions/abilities/crooked_cards')
        local player = stubPlayer()
        skill.onUseAbility(player, player, stubAbility())
        assert(player.added.effect == xi.effect.CROOKED_CARDS)
        assert(player.added.params.power == 20 and player.added.params.duration == 60)
    end)

    it('triple shot applies power 40 duration 90', function()
        local skill = require('scripts/actions/abilities/triple_shot')
        local player = stubPlayer()
        assert(skill.onUseAbility(player, player, stubAbility()) == xi.effect.TRIPLE_SHOT)
        assert(player.added.params.power == 40 and player.added.params.duration == 90)
    end)

    it('caper emissarius rejects self/non-PC and transfers enmity', function()
        local skill = require('scripts/actions/abilities/caper_emissarius')
        local actor = stubPlayer({ id = 1 })
        local selfT = stubPlayer({ id = 1 })
        assert(skill.onAbilityCheck(actor, selfT, stubAbility()) == xi.msg.basic.CANNOT_ON_THAT_TARG)
        local npc = stubPlayer({ id = 2, isPC = false })
        assert(skill.onAbilityCheck(actor, npc, stubAbility()) == xi.msg.basic.CANNOT_ON_THAT_TARG)
        local ally = stubPlayer({ id = 2, isPC = true })
        local ability = stubAbility()
        actor.mods[xi.mod.ONE_HOUR_RECAST] = 2
        assert(skill.onAbilityCheck(actor, ally, ability) == 0)
        assert(ability._getRecast() == 3480)
        skill.onUseAbility(actor, ally, ability)
        assert(ally.transfer.pct == 99 and ally.transfer.range == 20.6)
        assert(ally.transfer.actor == actor)
    end)

    it('libra is a pure no-op (TODO effect upstream)', function()
        local skill = require('scripts/actions/abilities/libra')
        local player = stubPlayer()
        assert(skill.onAbilityCheck(player, player, stubAbility()) == 0)
        skill.onUseAbility(player, player, stubAbility())
        assert(player.added == nil)
    end)
end)
