-----------------------------------
-- Pure system tests for Scholar job abilities
-- (scripts/actions/abilities/* arts/stratagems/sublimation/tabula/modus).
-- Stubs player/ability hosts so plans can be asserted without a full entity.
-----------------------------------

local function stubPlayer(opts)
    opts = opts or {}
    local effects = opts.effects or {}
    local mods = opts.mods or {}
    local merits = opts.merits or {}
    local jp = opts.jp or {}

    return {
        mainJob = opts.mainJob or xi.job.SCH,
        mainLvl = opts.mainLvl or 99,
        maxMP = opts.maxMP or 1000,
        mp = opts.mp or 500,
        effects = effects,
        mods = mods,
        merits = merits,
        jp = jp,
        silentDels = {},
        dels = {},
        added = nil,
        resetRecasts = {},
        mpAdded = 0,

        hasStatusEffect = function(self, effect)
            return self.effects[effect] ~= nil
        end,
        getStatusEffect = function(self, effect)
            return self.effects[effect]
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
        getMainJob = function(self)
            return self.mainJob
        end,
        getMainLvl = function(self)
            return self.mainLvl
        end,
        getMaxMP = function(self)
            return self.maxMP
        end,
        getMP = function(self)
            return self.mp
        end,
        addMP = function(self, amount)
            self.mpAdded = amount
            self.mp = self.mp + amount
        end,
        delStatusEffectSilent = function(self, effect)
            table.insert(self.silentDels, effect)
            self.effects[effect] = nil
        end,
        delStatusEffect = function(self, effect)
            table.insert(self.dels, effect)
            self.effects[effect] = nil
        end,
        addStatusEffect = function(self, effect, params)
            self.added = { effect = effect, params = params }
            self.effects[effect] = {
                getPower = function()
                    return params.power or 0
                end,
                getTier = function()
                    return params.tier or 0
                end,
            }
        end,
        resetRecast = function(self, recastType, id)
            table.insert(self.resetRecasts, { recastType, id })
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

describe('Scholar job ability pure plans', function()
    ---------------------------------------------------------------------------
    -- Fixed power-1 / duration-60 stratagems with already-active gate
    ---------------------------------------------------------------------------
    local fixedStratagems = {
        { 'accession', xi.effect.ACCESSION },
        { 'alacrity', xi.effect.ALACRITY },
        { 'celerity', xi.effect.CELERITY },
        { 'ebullience', xi.effect.EBULLIENCE },
        { 'immanence', xi.effect.IMMANENCE },
        { 'manifestation', xi.effect.MANIFESTATION },
        { 'parsimony', xi.effect.PARSIMONY },
        { 'penury', xi.effect.PENURY },
        { 'perpetuance', xi.effect.PERPETUANCE },
        { 'rapture', xi.effect.RAPTURE },
    }

    for _, entry in ipairs(fixedStratagems) do
        local name, effect = entry[1], entry[2]
        it(name .. ' refuses when active and applies power 1 duration 60', function()
            local skill = require('scripts/actions/abilities/' .. name)
            local player = stubPlayer({ effects = { [effect] = {} } })
            local ability = stubAbility()
            local msg, param = skill.onAbilityCheck(player, player, ability)
            assert(msg == xi.msg.basic.EFFECT_ALREADY_ACTIVE and param == 0)

            player = stubPlayer()
            msg, param = skill.onAbilityCheck(player, player, ability)
            assert(msg == 0 and param == 0)
            assert(skill.onUseAbility(player, player, ability) == effect)
            assert(player.added.effect == effect)
            assert(player.added.params.power == 1)
            assert(player.added.params.duration == 60)
        end)
    end

    ---------------------------------------------------------------------------
    -- Merit-powered stratagems
    ---------------------------------------------------------------------------
    local meritStratagems = {
        { 'altruism', xi.effect.ALTRUISM, xi.merit.ALTRUISM },
        { 'equanimity', xi.effect.EQUANIMITY, xi.merit.EQUANIMITY },
        { 'focalization', xi.effect.FOCALIZATION, xi.merit.FOCALIZATION },
        { 'tranquility', xi.effect.TRANQUILITY, xi.merit.TRANQUILITY },
    }

    for _, entry in ipairs(meritStratagems) do
        local name, effect, merit = entry[1], entry[2], entry[3]
        it(name .. ' uses merit as power with duration 60', function()
            local skill = require('scripts/actions/abilities/' .. name)
            local player = stubPlayer({ merits = { [merit] = 12 } })
            local ability = stubAbility()
            assert(skill.onAbilityCheck(player, player, ability) == 0)
            assert(skill.onUseAbility(player, player, ability) == effect)
            assert(player.added.params.power == 12)
            assert(player.added.params.duration == 60)
        end)
    end

    it('enlightenment uses merit-5 as power', function()
        local skill = require('scripts/actions/abilities/enlightenment')
        local player = stubPlayer({ merits = { [xi.merit.ENLIGHTENMENT] = 15 } })
        local ability = stubAbility()
        assert(skill.onUseAbility(player, player, ability) == xi.effect.ENLIGHTENMENT)
        assert(player.added.params.power == 10)
        assert(player.added.params.duration == 60)
    end)

    ---------------------------------------------------------------------------
    -- Light / Dark Arts + Addenda
    ---------------------------------------------------------------------------
    it('light arts already-active on LIGHT_ARTS or ADDENDUM_WHITE', function()
        local skill = require('scripts/actions/abilities/light_arts')
        local ability = stubAbility()
        local p1 = stubPlayer({ effects = { [xi.effect.LIGHT_ARTS] = {} } })
        assert(skill.onAbilityCheck(p1, p1, ability) == xi.msg.basic.EFFECT_ALREADY_ACTIVE)
        local p2 = stubPlayer({ effects = { [xi.effect.ADDENDUM_WHITE] = {} } })
        assert(skill.onAbilityCheck(p2, p2, ability) == xi.msg.basic.EFFECT_ALREADY_ACTIVE)
        local p3 = stubPlayer()
        assert(skill.onAbilityCheck(p3, p3, ability) == 0)
    end)

    it('light arts clears dark-side effects and applies regen subPower', function()
        local skill = require('scripts/actions/abilities/light_arts')
        local player = stubPlayer({
            mainJob = xi.job.SCH,
            mainLvl = 99,
            mods = { [xi.mod.LIGHT_ARTS_EFFECT] = 7 },
        })
        local ability = stubAbility()
        assert(skill.onUseAbility(player, player, ability) == xi.effect.LIGHT_ARTS)
        assert(contains(player.silentDels, xi.effect.DARK_ARTS))
        for _, e in ipairs({
            xi.effect.ADDENDUM_BLACK,
            xi.effect.PARSIMONY,
            xi.effect.ALACRITY,
            xi.effect.MANIFESTATION,
            xi.effect.EBULLIENCE,
            xi.effect.FOCALIZATION,
            xi.effect.EQUANIMITY,
            xi.effect.IMMANENCE,
        }) do
            assert(contains(player.dels, e), 'missing del ' .. tostring(e))
        end
        -- regenbonus = 3 * floor((99-10)/10) = 3*8 = 24
        assert(player.added.params.power == 7)
        assert(player.added.params.duration == 7200)
        assert(player.added.params.subPower == 24)
    end)

    it('light arts regenbonus is 0 below level 20 or non-SCH main', function()
        local skill = require('scripts/actions/abilities/light_arts')
        local pLow = stubPlayer({ mainJob = xi.job.SCH, mainLvl = 19 })
        skill.onUseAbility(pLow, pLow, stubAbility())
        assert(pLow.added.params.subPower == 0)
        local pSub = stubPlayer({ mainJob = xi.job.WHM, mainLvl = 99 })
        skill.onUseAbility(pSub, pSub, stubAbility())
        assert(pSub.added.params.subPower == 0)
    end)

    it('dark arts already-active on DARK_ARTS or ADDENDUM_BLACK', function()
        local skill = require('scripts/actions/abilities/dark_arts')
        local ability = stubAbility()
        local p1 = stubPlayer({ effects = { [xi.effect.DARK_ARTS] = {} } })
        assert(skill.onAbilityCheck(p1, p1, ability) == xi.msg.basic.EFFECT_ALREADY_ACTIVE)
        local p2 = stubPlayer({ effects = { [xi.effect.ADDENDUM_BLACK] = {} } })
        assert(skill.onAbilityCheck(p2, p2, ability) == xi.msg.basic.EFFECT_ALREADY_ACTIVE)
    end)

    it('dark arts clears light-side effects and applies helix subPower', function()
        local skill = require('scripts/actions/abilities/dark_arts')
        local player = stubPlayer({ mainJob = xi.job.SCH, mainLvl = 99 })
        assert(skill.onUseAbility(player, player, stubAbility()) == xi.effect.DARK_ARTS)
        assert(contains(player.silentDels, xi.effect.LIGHT_ARTS))
        for _, e in ipairs({
            xi.effect.ADDENDUM_WHITE,
            xi.effect.PENURY,
            xi.effect.CELERITY,
            xi.effect.ACCESSION,
            xi.effect.RAPTURE,
            xi.effect.ALTRUISM,
            xi.effect.TRANQUILITY,
            xi.effect.PERPETUANCE,
        }) do
            assert(contains(player.dels, e), 'missing del ' .. tostring(e))
        end
        -- helixbonus = floor(99/4) = 24
        assert(player.added.params.power == 1)
        assert(player.added.params.duration == 7200)
        assert(player.added.params.subPower == 24)
    end)

    it('addendum white applies light arts effect bonus and silent clears', function()
        local skill = require('scripts/actions/abilities/addendum_white')
        local player = stubPlayer({
            mainJob = xi.job.SCH,
            mainLvl = 50,
            mods = { [xi.mod.LIGHT_ARTS_EFFECT] = 3 },
        })
        assert(skill.onUseAbility(player, player, stubAbility()) == xi.effect.ADDENDUM_WHITE)
        assert(contains(player.silentDels, xi.effect.DARK_ARTS))
        assert(contains(player.silentDels, xi.effect.ADDENDUM_BLACK))
        assert(contains(player.silentDels, xi.effect.LIGHT_ARTS))
        -- regen = 3 * floor((50-10)/10) = 3*4 = 12
        assert(player.added.params.power == 3)
        assert(player.added.params.duration == 7200)
        assert(player.added.params.subPower == 12)
        assert(player.added.params.silent == true)
    end)

    it('addendum black applies dark arts effect bonus and helix subPower', function()
        local skill = require('scripts/actions/abilities/addendum_black')
        local player = stubPlayer({
            mainJob = xi.job.SCH,
            mainLvl = 80,
            mods = { [xi.mod.DARK_ARTS_EFFECT] = 5 },
        })
        assert(skill.onUseAbility(player, player, stubAbility()) == xi.effect.ADDENDUM_BLACK)
        assert(contains(player.silentDels, xi.effect.LIGHT_ARTS))
        assert(contains(player.silentDels, xi.effect.ADDENDUM_WHITE))
        assert(contains(player.silentDels, xi.effect.DARK_ARTS))
        -- helix = floor(80/4) = 20
        assert(player.added.params.power == 5)
        assert(player.added.params.subPower == 20)
        assert(player.added.params.silent == true)
    end)

    ---------------------------------------------------------------------------
    -- Tabula Rasa
    ---------------------------------------------------------------------------
    it('tabula rasa reduces one-hour recast and applies 1.5x bonuses', function()
        local skill = require('scripts/actions/abilities/tabula_rasa')
        local player = stubPlayer({
            mainJob = xi.job.SCH,
            mainLvl = 99,
            maxMP = 1000,
            jp = { [xi.jp.TABULA_RASA_EFFECT] = 5 },
            mods = { [xi.mod.ONE_HOUR_RECAST] = 2 },
        })
        local ability = stubAbility()
        -- recast starts 3600; -2*60 = 3480
        assert(skill.onAbilityCheck(player, player, ability) == 0)
        assert(ability._getRecast() == 3480)

        assert(skill.onUseAbility(player, player, ability) == xi.effect.TABULA_RASA)
        -- helix=24, regen=24 → power floor(24*1.5)=36, sub floor(24*1.5)=36
        assert(player.added.params.power == 36)
        assert(player.added.params.subPower == 36)
        assert(player.added.params.duration == 180)
        -- JP MP: 1000 * 0.02 * 5 = 100
        assert(player.mpAdded == 100)
        local ids = {}
        for _, r in ipairs(player.resetRecasts) do
            ids[#ids + 1] = r[2]
        end
        assert(contains(ids, 228) and contains(ids, 231) and contains(ids, 232))
    end)

    ---------------------------------------------------------------------------
    -- Sublimation pure branches
    ---------------------------------------------------------------------------
    it('sublimation recovers MP from complete and activated states', function()
        local skill = require('scripts/actions/abilities/sublimation')
        local ability = stubAbility()

        local complete = stubPlayer({
            mp = 900,
            maxMP = 1000,
            effects = {
                [xi.effect.SUBLIMATION_COMPLETE] = {
                    getPower = function()
                        return 200
                    end,
                },
            },
        })
        local ret = skill.onUseAbility(complete, complete, ability)
        assert(ret == 100) -- clamped to missing MP
        assert(ability.getMsg() == xi.msg.basic.JA_RECOVERS_MP)
        assert(complete.effects[xi.effect.SUBLIMATION_COMPLETE] == nil)

        local activated = stubPlayer({
            mp = 100,
            maxMP = 1000,
            effects = {
                [xi.effect.SUBLIMATION_ACTIVATED] = {
                    getPower = function()
                        return 50
                    end,
                },
            },
        })
        ability = stubAbility()
        ret = skill.onUseAbility(activated, activated, ability)
        assert(ret == 50)
        assert(ability.getMsg() == xi.msg.basic.JA_RECOVERS_MP)
    end)

    it('sublimation starts charge unless refresh tier >= 3', function()
        local skill = require('scripts/actions/abilities/sublimation')
        local ability = stubAbility()
        local player = stubPlayer({
            effects = {
                [xi.effect.REFRESH] = {
                    getTier = function()
                        return 1
                    end,
                },
            },
        })
        assert(skill.onUseAbility(player, player, ability) == 0)
        assert(player.added.effect == xi.effect.SUBLIMATION_ACTIVATED)
        assert(player.added.params.duration == 7200)
        assert(player.added.params.tick == 3)
        assert(contains(player.dels, xi.effect.REFRESH))

        local blocked = stubPlayer({
            effects = {
                [xi.effect.REFRESH] = {
                    getTier = function()
                        return 3
                    end,
                },
            },
        })
        ability = stubAbility()
        assert(skill.onUseAbility(blocked, blocked, ability) == 0)
        assert(ability.getMsg() == xi.msg.basic.JA_NO_EFFECT_2)
        assert(blocked.added == nil)
    end)

    ---------------------------------------------------------------------------
    -- Modus Veritas pure products
    ---------------------------------------------------------------------------
    it('modus veritas no-effect without helix; miss on stack/NM/low resist', function()
        local skill = require('scripts/actions/abilities/modus_veritas')
        local ability = stubAbility()
        local noHelix = stubPlayer()
        skill.onUseAbility(stubPlayer(), noHelix, ability)
        assert(ability.getMsg() == xi.msg.basic.JA_NO_EFFECT_2)

        -- Patch resist + NM path: inject via target methods
        local orig = xi.combat.magicHitRate.calculateResistRate
        xi.combat.magicHitRate.calculateResistRate = function()
            return 1.0
        end

        local stacked = {
            isNM = function()
                return false
            end,
            getStatusEffect = function()
                return {
                    getSubPower = function()
                        return 1
                    end, -- already applied
                    getPower = function()
                        return 10
                    end,
                    getDuration = function()
                        return 60
                    end,
                    getTimeRemaining = function()
                        return 30000
                    end,
                    setSubPower = function() end,
                    setPower = function() end,
                    setDuration = function() end,
                }
            end,
        }
        ability = stubAbility()
        assert(skill.onUseAbility(stubPlayer(), stacked, ability) == 0)
        assert(ability.getMsg() == xi.msg.basic.JA_MISS)

        xi.combat.magicHitRate.calculateResistRate = orig
    end)

    it('modus veritas doubles helix power and scales remaining duration', function()
        local skill = require('scripts/actions/abilities/modus_veritas')
        local ability = stubAbility()
        local orig = xi.combat.magicHitRate.calculateResistRate
        xi.combat.magicHitRate.calculateResistRate = function()
            return 1.0
        end

        local setSub, setPow, setDur
        local target = {
            isNM = function()
                return false
            end,
            getStatusEffect = function(_, effect)
                if effect == xi.effect.HELIX then
                    return {
                        getSubPower = function()
                            return 0
                        end,
                        getPower = function()
                            return 20
                        end,
                        getDuration = function()
                            return 100
                        end, -- seconds in effect storage before ms convert
                        getTimeRemaining = function()
                            return 40000
                        end, -- 40s remaining in ms
                        setSubPower = function(_, v)
                            setSub = v
                        end,
                        setPower = function(_, v)
                            setPow = v
                        end,
                        setDuration = function(_, v)
                            setDur = v
                        end,
                    }
                end
                return nil
            end,
        }
        local player = stubPlayer({
            merits = { [xi.merit.MODUS_VERITAS_DURATION] = 2 },
            jp = { [xi.jp.MODUS_VERITAS_EFFECT] = 3 },
        })
        skill.onUseAbility(player, target, ability)
        -- mvPower = 0+1 = 1
        assert(setSub == 1)
        -- helixPower = 20*2 + 3*3 = 49
        assert(setPow == 49)
        -- durationMultiplier = 0.5 + 0.05*2 = 0.6
        -- remaining = floor(40000/1000) = 40
        -- duration = (100-40) + floor(40*0.6) = 60 + 24 = 84
        -- setDuration ms = 84000
        assert(setDur == 84000)
        assert(ability.getMsg() == nil)

        xi.combat.magicHitRate.calculateResistRate = orig
    end)
end)
