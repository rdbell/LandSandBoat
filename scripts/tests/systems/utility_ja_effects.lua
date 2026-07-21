-----------------------------------
-- Pure system tests for utility JA status-effect scripts (Crooked Cards,
-- Store TP, Flee, Quickening, Retaliation, Warriors/Assassins Charge,
-- Dragon Breaker, Bust, Restraint, Addendum).
-----------------------------------

local function stubTarget()
    return {
        mods = {},
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
        delMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) - delta
        end,
        getJobPointLevel = function()
            return 0
        end,
        hasStatusEffect = function()
            return false
        end,
        recalculateAbilitiesTable = function() end,
        recalculateSkillsTable = function() end,
    }
end

local function stubEffect(opts)
    opts = opts or {}
    return {
        power = opts.power or 0,
        subPower = opts.subPower or 0,
        subType = opts.subType or 0,
        mods = {},
        getPower = function(self)
            return self.power
        end,
        getSubPower = function(self)
            return self.subPower
        end,
        getSubType = function(self)
            return self.subType
        end,
        addMod = function(self, mod, delta)
            self.mods[mod] = (self.mods[mod] or 0) + delta
        end,
    }
end

describe('Utility JA status-effect pure plans', function()
    it('crooked cards applies PHANTOM_ROLL 100', function()
        local script = require('scripts/effects/crooked_cards')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.PHANTOM_ROLL] == 100)
        script.onEffectLose(target, stubEffect())
        assert(target.mods[xi.mod.PHANTOM_ROLL] == 0)
    end)

    it('store tp and quickening apply power mods', function()
        local store = require('scripts/effects/store_tp')
        local target = stubTarget()
        store.onEffectGain(target, stubEffect({ power = 15 }))
        assert(target.mods[xi.mod.STORETP] == 15)
        store.onEffectLose(target, stubEffect({ power = 15 }))
        assert(target.mods[xi.mod.STORETP] == 0)

        local quick = require('scripts/effects/quickening')
        target = stubTarget()
        quick.onEffectGain(target, stubEffect({ power = 12 }))
        assert(target.mods[xi.mod.MOVE_SPEED_QUICKENING] == 12)
    end)

    it('flee and retaliation apply movement mods', function()
        local flee = require('scripts/effects/flee')
        local effect = stubEffect({ power = 50 })
        flee.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.MOVE_SPEED_FLEE] == 50)

        local ret = require('scripts/effects/retaliation')
        local target = stubTarget()
        ret.onEffectGain(target, stubEffect())
        assert(target.mods[xi.mod.MOVE_SPEED_WEIGHT_PENALTY] == 8)
        ret.onEffectLose(target, stubEffect())
        assert(target.mods[xi.mod.MOVE_SPEED_WEIGHT_PENALTY] == 0)
    end)

    it('warriors and assassins charge apply multi-attack mods', function()
        local war = require('scripts/effects/warriors_charge')
        local effect = stubEffect({ power = 20 })
        war.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.TRIPLE_ATTACK] == 20)
        assert(effect.mods[xi.mod.DOUBLE_ATTACK] == 100)

        local ass = require('scripts/effects/assassins_charge')
        effect = stubEffect({ power = 5, subPower = 2 })
        ass.onEffectGain(stubTarget(), effect)
        assert(effect.mods[xi.mod.QUAD_ATTACK] == 5)
        assert(effect.mods[xi.mod.TRIPLE_ATTACK] == 100)
        assert(effect.mods[xi.mod.CRITHITRATE] == 2)
    end)

    it('dragon breaker reduces ACC EVA MACC MEVA STORETP', function()
        local script = require('scripts/effects/dragon_breaker')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 15 }))
        assert(target.mods[xi.mod.ACC] == -15)
        assert(target.mods[xi.mod.EVA] == -15)
        assert(target.mods[xi.mod.MACC] == -15)
        assert(target.mods[xi.mod.MEVA] == -15)
        assert(target.mods[xi.mod.STORETP] == -15)
        script.onEffectLose(target, stubEffect({ power = 15 }))
        assert(target.mods[xi.mod.ACC] == 0)
        assert(target.mods[xi.mod.STORETP] == 0)
    end)

    it('bust applies DMG power or paired RACC/RATTP penalties', function()
        local script = require('scripts/effects/bust')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 500, subType = xi.mod.DMG }))
        assert(target.mods[xi.mod.DMG] == 500)

        target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 15, subType = xi.mod.ACC }))
        assert(target.mods[xi.mod.RACC] == -15)
        assert(target.mods[xi.mod.ACC] == -15)

        target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 20, subType = xi.mod.ATTP }))
        assert(target.mods[xi.mod.RATTP] == -20)
        assert(target.mods[xi.mod.ATTP] == -20)
    end)

    it('restraint lose removes ALL_WSDMG_FIRST_HIT power', function()
        local script = require('scripts/effects/restraint')
        local target = stubTarget()
        -- seed the mod as if combat applied it during the effect
        target.mods[xi.mod.ALL_WSDMG_FIRST_HIT] = 30
        script.onEffectLose(target, stubEffect({ power = 30 }))
        assert(target.mods[xi.mod.ALL_WSDMG_FIRST_HIT] == 0)
    end)

    it('addendum white mirrors light arts mods without tabula', function()
        local script = require('scripts/effects/addendum_white')
        local target = stubTarget()
        script.onEffectGain(target, stubEffect({ power = 10, subPower = 5 }))
        assert(target.mods[xi.mod.WHITE_MAGIC_COST] == -20) -- -10 bonus + -10 favored
        assert(target.mods[xi.mod.BLACK_MAGIC_COST] == 20)
        assert(target.mods[xi.mod.LIGHT_ARTS_REGEN] == 5)
        assert(target.mods[xi.mod.REGEN_DURATION] == 10)
    end)
end)
