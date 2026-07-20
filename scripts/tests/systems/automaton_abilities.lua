-----------------------------------
-- Pure system tests for Automaton ability scripts under
-- scripts/actions/abilities/pets/automaton/.
-----------------------------------

local function stubSkill()
    local msg = nil
    return {
        setMsg = function(_, m)
            msg = m
        end,
        getMsg = function()
            return msg
        end,
        getID = function()
            return 1
        end,
    }
end

local function stubAutomaton(opts)
    opts = opts or {}
    return {
        skills = opts.skills or { melee = 0, ranged = 0, magic = 0 },
        int = opts.int or 0,
        maxMP = opts.maxMP or 1000,
        mp = opts.mp or 0,
        attachments = opts.attachments or {},
        recasts = {},
        getSkillLevel = function(self, skill)
            if skill == xi.skill.AUTOMATON_MELEE then
                return self.skills.melee
            end
            if skill == xi.skill.AUTOMATON_RANGED then
                return self.skills.ranged
            end
            if skill == xi.skill.AUTOMATON_MAGIC then
                return self.skills.magic
            end
            return 0
        end,
        getStat = function(self, mod)
            if mod == xi.mod.INT then
                return self.int
            end
            return 0
        end,
        getMaxMP = function(self)
            return self.maxMP
        end,
        addMP = function(self, amount)
            self.mp = self.mp + amount
            return amount
        end,
        addRecast = function(self, recastType, id, sec)
            table.insert(self.recasts, { recastType, id, sec })
        end,
        hasAttachmentSet = function(self, item)
            return self.attachments[item] == true
        end,
    }
end

local function stubTarget(opts)
    opts = opts or {}
    local effects = opts.effects or {}
    return {
        hp = opts.hp or 500,
        maxHP = opts.maxHP or 1000,
        mp = opts.mp or 0,
        tp = opts.tp or 0,
        effects = effects,
        added = {},
        dels = {},
        enmity = nil,
        getHP = function(self)
            return self.hp
        end,
        getMaxHP = function(self)
            return self.maxHP
        end,
        getTP = function(self)
            return self.tp
        end,
        setHP = function(self, v)
            self.hp = v
        end,
        addHP = function(self, amount)
            self.hp = self.hp + amount
        end,
        addTP = function(self, amount)
            self.tp = self.tp + amount
        end,
        wakeUp = function() end,
        eraseAllStatusEffect = function(self)
            self.erasedAll = true
        end,
        delStatusEffect = function(self, effect)
            table.insert(self.dels, effect)
            self.effects[effect] = nil
        end,
        delStatusEffectSilent = function(self, effect)
            table.insert(self.dels, effect)
            self.effects[effect] = nil
        end,
        hasStatusEffect = function(self, effect)
            return self.effects[effect] ~= nil
        end,
        addStatusEffect = function(self, effect, params)
            table.insert(self.added, { effect = effect, params = params })
            self.effects[effect] = true
            return true
        end,
        addEnmity = function(self, actor, ce, ve)
            self.enmity = { actor = actor, ce = ce, ve = ve }
        end,
        dispelStatusEffect = function(self)
            if opts.dispel then
                return opts.dispel
            end
            return xi.effect.NONE
        end,
    }
end

local function stubMaster(opts)
    opts = opts or {}
    local counts = opts.counts or {}
    return {
        countEffect = function(_, effect)
            return counts[effect] or 0
        end,
    }
end

local origBuff, origStatus
local function stubHosts()
    origBuff = xi.mobskills.mobBuffMove
    origStatus = xi.mobskills.mobStatusEffectMove
    xi.mobskills.mobBuffMove = function(target, effect, power, tick, duration)
        target.buff = { effect = effect, power = power, tick = tick, duration = duration }
        return 0
    end
    xi.mobskills.mobStatusEffectMove = function(automaton, target, effect, power, tick, duration)
        target.statusMove = { effect = effect, power = power, tick = tick, duration = duration }
        return xi.msg.basic.SKILL_ENFEEB_IS -- arbitrary return observed into setMsg
    end
end

local function restoreHosts()
    xi.mobskills.mobBuffMove = origBuff
    xi.mobskills.mobStatusEffectMove = origStatus
end

describe('Automaton ability pure plans', function()
    before_each(function()
        stubHosts()
    end)
    after_each(function()
        restoreHosts()
    end)

    local fixedBuffs = {
        { 'chainspell_automaton', xi.effect.CHAINSPELL, 1, 60 },
        { 'invincible_automaton', xi.effect.INVINCIBLE, 1, 30 },
        { 'manafont_automaton', xi.effect.MANAFONT, 1, 60 },
        { 'mighty_strikes_automaton', xi.effect.MIGHTY_STRIKES, 1, 45 },
    }

    for _, entry in ipairs(fixedBuffs) do
        local name, effect, power, duration = entry[1], entry[2], entry[3], entry[4]
        it(name .. ' applies fixed buff via mobBuffMove', function()
            local skill = require('scripts/actions/abilities/pets/automaton/' .. name)
            local target = stubTarget()
            local sk = stubSkill()
            assert(skill.onAutomatonAbility(target, stubAutomaton(), sk, stubMaster(), {}) == effect)
            assert(target.buff.effect == effect)
            assert(target.buff.power == power and target.buff.duration == duration)
            assert(sk.getMsg() == xi.msg.basic.USES)
        end)
    end

    it('flashbulb applies FLASH power 0 duration 12 with recast 45', function()
        local skill = require('scripts/actions/abilities/pets/automaton/flashbulb')
        local auto = stubAutomaton()
        local target = stubTarget()
        skill.onAutomatonAbility(target, auto, stubSkill(), stubMaster(), {})
        assert(target.statusMove.effect == xi.effect.FLASH)
        assert(target.statusMove.power == 0 and target.statusMove.duration == 12)
        assert(auto.recasts[1][3] == 45)
    end)

    it('reactive shield power is floor(skill/16)+floor(INT/8)', function()
        local skill = require('scripts/actions/abilities/pets/automaton/reactive_shield')
        local auto = stubAutomaton({ skills = { melee = 100, ranged = 320, magic = 200 }, int = 80 })
        local target = stubTarget()
        assert(skill.onAutomatonAbility(target, auto, stubSkill(), stubMaster(), {}) == xi.effect.BLAZE_SPIKES)
        assert(target.added[1].params.power == 30)
        assert(target.added[1].params.duration == 60)
    end)

    it('provoke adds enmity CE 1 VE 1800', function()
        local skill = require('scripts/actions/abilities/pets/automaton/provoke')
        local auto = stubAutomaton()
        local target = stubTarget()
        local sk = stubSkill()
        skill.onAutomatonAbility(target, auto, sk, stubMaster(), {})
        assert(target.enmity.ce == 1 and target.enmity.ve == 1800)
        assert(sk.getMsg() == xi.msg.basic.PROVOKE_SWITCH)
        assert(auto.recasts[1][3] == 30)
    end)

    it('disruptor messages depend on dispel result', function()
        local skill = require('scripts/actions/abilities/pets/automaton/disruptor')
        local sk = stubSkill()
        skill.onAutomatonAbility(stubTarget({ dispel = xi.effect.POISON }), stubAutomaton(), sk, stubMaster(), {})
        assert(sk.getMsg() == xi.msg.basic.SKILL_ERASE)
        sk = stubSkill()
        skill.onAutomatonAbility(stubTarget({ dispel = xi.effect.NONE }), stubAutomaton(), sk, stubMaster(), {})
        assert(sk.getMsg() == xi.msg.basic.SKILL_NO_EFFECT)
    end)

    it('economizer recovers floor(maxMP * 0.2 * darkManeuvers)', function()
        local skill = require('scripts/actions/abilities/pets/automaton/economizer')
        local auto = stubAutomaton({ maxMP = 1000 })
        local ret = skill.onAutomatonAbility(stubTarget(), auto, stubSkill(), stubMaster({
            counts = { [xi.effect.DARK_MANEUVER] = 2 },
        }), {})
        assert(ret == 400)
    end)

    it('benediction automaton heals full missing HP', function()
        local skill = require('scripts/actions/abilities/pets/automaton/benediction_automaton')
        local target = stubTarget({ hp = 250, maxHP = 1000 })
        local ret = skill.onAutomatonAbility(target, stubAutomaton(), stubSkill(), stubMaster(), {})
        assert(ret == 750)
        assert(target.erasedAll == true)
        assert(target.hp == 1000)
    end)

    it('mana converter spends half HP and applies refresh floor(cost/10)', function()
        local skill = require('scripts/actions/abilities/pets/automaton/mana_converter')
        local target = stubTarget({ hp = 500 })
        assert(skill.onAutomatonAbility(target, stubAutomaton(), stubSkill(), stubMaster(), {}) == xi.effect.REFRESH)
        assert(target.hp == 250)
        assert(target.added[1].params.power == 25)
        assert(target.added[1].params.duration == 30 and target.added[1].params.tick == 3)
    end)

    it('eraser removes one effect per light maneuver in catalog order', function()
        local skill = require('scripts/actions/abilities/pets/automaton/eraser')
        local target = stubTarget({
            effects = {
                [xi.effect.ELEGY] = true,
                [xi.effect.POISON] = true,
                [xi.effect.BLINDNESS] = true,
            },
        })
        local sk = stubSkill()
        local ret = skill.onAutomatonAbility(target, stubAutomaton(), sk, stubMaster({
            counts = { [xi.effect.LIGHT_MANEUVER] = 2 },
        }), {})
        assert(ret == 2)
        assert(sk.getMsg() == xi.msg.basic.DISAPPEAR_NUM)
        -- ELEGY then BLINDNESS before POISON in catalog
        assert(target.effects[xi.effect.ELEGY] == nil)
        assert(target.effects[xi.effect.BLINDNESS] == nil)
        assert(target.effects[xi.effect.POISON] == true)
    end)

    it('heat capacitor grants TP from fire maneuvers and attachments', function()
        local skill = require('scripts/actions/abilities/pets/automaton/heat_capacitor')
        local target = stubTarget()
        local auto = stubAutomaton({
            attachments = {
                [xi.item.HEAT_CAPACITOR_ATTACHMENT] = true,
                [xi.item.HEAT_CAPACITOR_II_ATTACHMENT] = true,
            },
        })
        local ret = skill.onAutomatonAbility(target, auto, stubSkill(), stubMaster({
            counts = { [xi.effect.FIRE_MANEUVER] = 1 },
        }), {})
        -- both attachments at 1 maneuver: 400+600=1000 TP; getTP returns sum if started 0
        assert(target.tp == 1000)
        assert(ret == 1000)
    end)
end)
