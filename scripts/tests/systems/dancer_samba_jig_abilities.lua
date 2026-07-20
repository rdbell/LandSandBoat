-----------------------------------
-- Pure system tests for Dancer samba/jig/dance/flourish ability scripts
-- (scripts/actions/abilities/* outside job_utils/dancer.lua).
-----------------------------------

local function stubPlayer(opts)
    opts = opts or {}
    local effects = opts.effects or {}
    return {
        effects = effects,
        tp = opts.tp or 0,
        mods = opts.mods or {},
        merits = opts.merits or {},
        jp = opts.jp or {},
        dels = {},
        delTPAmount = 0,
        added = {},
        hasStatusEffect = function(self, effect)
            return self.effects[effect] ~= nil
        end,
        getTP = function(self)
            return self.tp
        end,
        getMod = function(self, mod)
            return self.mods[mod] or 0
        end,
        getMerit = function(self, merit)
            return self.merits[merit] or 0
        end,
        getJobPointLevel = function(self, jpId)
            return self.jp[jpId] or 0
        end,
        delTP = function(self, amount)
            self.delTPAmount = amount
            self.tp = self.tp - amount
        end,
        delStatusEffect = function(self, effect)
            table.insert(self.dels, effect)
            self.effects[effect] = nil
        end,
        addStatusEffect = function(self, effect, params)
            table.insert(self.added, { effect = effect, params = params })
            self.effects[effect] = true
        end,
        addTP = function(self, amount)
            self.tp = self.tp + amount
        end,
    }
end

local function stubAbility()
    local recast = 3600
    local msg = nil
    return {
        getRecast = function()
            return recast
        end,
        setRecast = function(_, r)
            recast = r
        end,
        setMsg = function(_, m)
            msg = m
        end,
        getMsg = function()
            return msg
        end,
        _getRecast = function()
            return recast
        end,
    }
end

local function contains(list, value)
    for _, v in ipairs(list) do
        if v == value then
            return true
        end
    end
    return false
end

describe('Dancer samba/jig/dance pure plans', function()
    it('drain samba refuses fan dance and low TP; applies power 1 duration product', function()
        local skill = require('scripts/actions/abilities/drain_samba')
        local ability = stubAbility()
        local fan = stubPlayer({ effects = { [xi.effect.FAN_DANCE] = true }, tp = 999 })
        assert(skill.onAbilityCheck(fan, fan, ability) == xi.msg.basic.UNABLE_TO_USE_JA2)
        local low = stubPlayer({ tp = 50 })
        assert(skill.onAbilityCheck(low, low, ability) == xi.msg.basic.NOT_ENOUGH_TP)

        local player = stubPlayer({
            tp = 200,
            mods = { [xi.mod.SAMBA_DURATION] = 10, [xi.mod.SAMBA_PDURATION] = 20 },
            jp = { [xi.jp.SAMBA_DURATION] = 3 },
        })
        assert(skill.onAbilityCheck(player, player, ability) == 0)
        assert(skill.onUseAbility(player, player, ability) == xi.effect.DRAIN_SAMBA)
        assert(player.delTPAmount == 100)
        -- duration = (120+10+6) * 1.2 = 163.2
        local added = player.added[1]
        assert(added.effect == xi.effect.DRAIN_SAMBA)
        assert(added.params.power == 1)
        assert(math.abs(added.params.duration - 163.2) < 1e-9)
        assert(contains(player.dels, xi.effect.HASTE_SAMBA))
        assert(contains(player.dels, xi.effect.ASPIR_SAMBA))
    end)

    it('drain samba skips TP cost under Trance', function()
        local skill = require('scripts/actions/abilities/drain_samba')
        local player = stubPlayer({ tp = 200, effects = { [xi.effect.TRANCE] = true } })
        -- Drain does not bypass TP check, so give enough TP
        assert(skill.onAbilityCheck(player, player, stubAbility()) == 0)
        skill.onUseAbility(player, player, stubAbility())
        assert(player.delTPAmount == 0)
    end)

    it('aspir samba II bypasses TP check under Trance and uses power 2', function()
        local skill = require('scripts/actions/abilities/aspir_samba_ii')
        local player = stubPlayer({ tp = 0, effects = { [xi.effect.TRANCE] = true } })
        assert(skill.onAbilityCheck(player, player, stubAbility()) == 0)
        assert(skill.onUseAbility(player, player, stubAbility()) == xi.effect.ASPIR_SAMBA)
        assert(player.added[1].params.power == 2)
        assert(player.delTPAmount == 0)
    end)

    it('haste samba power is 500 + merit', function()
        local skill = require('scripts/actions/abilities/haste_samba')
        local player = stubPlayer({
            tp = 350,
            merits = { [xi.merit.HASTE_SAMBA_EFFECT] = 15 },
        })
        assert(skill.onUseAbility(player, player, stubAbility()) == xi.effect.HASTE_SAMBA)
        assert(player.added[1].params.power == 515)
        assert(player.delTPAmount == 350)
    end)

    it('chocobo jig applies QUICKENING power 10 with jig duration product', function()
        local skill = require('scripts/actions/abilities/chocobo_jig')
        local player = stubPlayer({
            effects = { [xi.effect.WEIGHT] = true },
            jp = { [xi.jp.JIG_DURATION] = 5 },
            mods = { [xi.mod.JIG_DURATION] = 25 },
        })
        assert(skill.onUseAbility(player, player, stubAbility()) == xi.effect.QUICKENING)
        assert(contains(player.dels, xi.effect.WEIGHT))
        -- floor((120+5)*1.25) = 156
        assert(player.added[1].params.power == 10)
        assert(player.added[1].params.duration == 156)
    end)

    it('spectral jig applies sneak+invisible or NO_EFFECT when sneak active', function()
        local skill = require('scripts/actions/abilities/spectral_jig')
        local ability = stubAbility()
        local player = stubPlayer()
        assert(skill.onUseAbility(player, player, ability) == 1)
        assert(ability.getMsg() == xi.msg.basic.SPECTRAL_JIG)
        assert(#player.added == 2)

        local sneaked = stubPlayer({ effects = { [xi.effect.SNEAK] = true } })
        ability = stubAbility()
        assert(skill.onUseAbility(sneaked, sneaked, ability) == 1)
        assert(ability.getMsg() == xi.msg.basic.NO_EFFECT)
        assert(#sneaked.added == 0)
    end)

    it('fan dance and saber dance use fixed params', function()
        local fan = require('scripts/actions/abilities/fan_dance')
        local p = stubPlayer()
        fan.onUseAbility(p, p, stubAbility())
        assert(p.added[1].effect == xi.effect.FAN_DANCE)
        assert(p.added[1].params.power == 9000 and p.added[1].params.duration == 300)

        local saber = require('scripts/actions/abilities/saber_dance')
        p = stubPlayer()
        saber.onUseAbility(p, p, stubAbility())
        assert(p.added[1].effect == xi.effect.SABER_DANCE)
        assert(p.added[1].params.power == 50 and p.added[1].params.duration == 300)
        assert(p.added[1].params.tick == 3)
    end)

    it('trance reduces one-hour recast and grants JP TP', function()
        local skill = require('scripts/actions/abilities/trance')
        local player = stubPlayer({
            mods = { [xi.mod.ONE_HOUR_RECAST] = 2 },
            jp = { [xi.jp.TRANCE_EFFECT] = 3 },
        })
        local ability = stubAbility()
        assert(skill.onAbilityCheck(player, player, ability) == 0)
        assert(ability._getRecast() == 3480)
        skill.onUseAbility(player, player, ability)
        assert(player.added[1].effect == xi.effect.TRANCE)
        assert(player.added[1].params.duration == 60)
        assert(player.tp == 300)
    end)

    it('grand pas reduces one-hour recast and applies power 19 duration 30', function()
        local skill = require('scripts/actions/abilities/grand_pas')
        local player = stubPlayer({ mods = { [xi.mod.ONE_HOUR_RECAST] = 1 } })
        local ability = stubAbility()
        skill.onAbilityCheck(player, player, ability)
        assert(ability._getRecast() == 3540)
        skill.onUseAbility(player, player, ability)
        assert(player.added[1].effect == xi.effect.GRAND_PAS)
        assert(player.added[1].params.power == 19)
        assert(player.added[1].params.duration == 30)
        assert(player.added[1].params.tick == 1)
    end)

    it('climactic flourish requires finishing moves and clears FM1-5', function()
        local skill = require('scripts/actions/abilities/climactic_flourish')
        local none = stubPlayer()
        assert(skill.onAbilityCheck(none, none, stubAbility()) == xi.msg.basic.NO_FINISHINGMOVES)
        local player = stubPlayer({ effects = { [xi.effect.FINISHING_MOVE_2] = true } })
        assert(skill.onAbilityCheck(player, player, stubAbility()) == 0)
        skill.onUseAbility(player, player, stubAbility())
        -- last add is climactic; FM2 should be among dels
        assert(contains(player.dels, xi.effect.FINISHING_MOVE_2))
        local last = player.added[#player.added]
        assert(last.effect == xi.effect.CLIMACTIC_FLOURISH)
        assert(last.params.power == 3 and last.params.duration == 60)
    end)

    it('striking flourish FM5 leaves FM3 remainder and power 3', function()
        local skill = require('scripts/actions/abilities/striking_flourish')
        local player = stubPlayer({ effects = { [xi.effect.FINISHING_MOVE_5] = true } })
        assert(skill.onAbilityCheck(player, player, stubAbility()) == 0)
        skill.onUseAbility(player, player, stubAbility())
        assert(contains(player.dels, xi.effect.FINISHING_MOVE_5))
        local rem, flour
        for _, a in ipairs(player.added) do
            if a.effect == xi.effect.FINISHING_MOVE_3 then
                rem = a
            end
            if a.effect == xi.effect.STRIKING_FLOURISH then
                flour = a
            end
        end
        assert(rem and rem.params.duration == 7200)
        assert(flour and flour.params.power == 3 and flour.params.duration == 60)
    end)

    it('ternary flourish FM3 consumes all three without remainder', function()
        local skill = require('scripts/actions/abilities/ternary_flourish')
        local player = stubPlayer({ effects = { [xi.effect.FINISHING_MOVE_3] = true } })
        assert(skill.onAbilityCheck(player, player, stubAbility()) == 0)
        skill.onUseAbility(player, player, stubAbility())
        assert(contains(player.dels, xi.effect.FINISHING_MOVE_3))
        assert(#player.added == 1)
        assert(player.added[1].effect == xi.effect.TERNARY_FLOURISH)
        assert(player.added[1].params.power == 3)
    end)
end)
