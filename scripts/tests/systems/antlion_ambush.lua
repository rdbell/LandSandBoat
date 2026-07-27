local ambush = require('scripts/mixins/families/antlion_ambush')
local noRehide = require('scripts/mixins/families/antlion_ambush_no_rehide')

local function newMob()
    local mob = {
        abilities = {},
        animationSub = -1,
        animationUpdate = nil,
        autoAttack = nil,
        hidden = nil,
        listeners = {},
        localVars = {},
        mobMods = {},
        untargetable = nil,
    }

    function mob:addListener(event, _, callback)
        self.listeners[event] = callback
    end

    function mob:hideName(value)
        self.hidden = value
    end

    function mob:setUntargetable(value)
        self.untargetable = value
    end

    function mob:setAutoAttackEnabled(value)
        self.autoAttack = value
    end

    function mob:setAnimationSub(value, sendUpdate)
        self.animationSub = value
        if sendUpdate == nil then
            sendUpdate = true
        end
        self.animationUpdate = sendUpdate
    end

    function mob:setMobMod(mod, value)
        self.mobMods[mod] = value
    end

    function mob:useMobAbility(skill)
        table.insert(self.abilities, skill)
    end

    function mob:getLocalVar(name)
        return self.localVars[name] or 0
    end

    function mob:setLocalVar(name, value)
        self.localVars[name] = value
    end

    return mob
end

local function assertHidden(mob)
    assert(mob.hidden == true)
    assert(mob.untargetable == true)
    assert(mob.autoAttack == false)
    assert(mob.animationSub == 0)
    assert(mob.animationUpdate == true)
    assert(mob.mobMods[xi.mobMod.NO_MOVE] == 1)
end

local function assertRevealed(mob, animationUpdate)
    assert(mob.hidden == false)
    assert(mob.untargetable == false)
    assert(mob.autoAttack == true)
    assert(mob.animationSub == 1)
    assert(mob.animationUpdate == animationUpdate)
    assert(mob.mobMods[xi.mobMod.NO_MOVE] == 0)
end

describe('Antlion ambush mixins', function()
    it('rehides standard ambushers after disengage', function()
        local mob = newMob()
        ambush(mob)

        mob.listeners.PRESPAWN(mob)
        assertHidden(mob)

        mob.listeners.ENGAGE(mob)
        assert(mob.abilities[1] == xi.mobSkill.PIT_AMBUSH_1)

        mob.listeners.WEAPONSKILL_STATE_EXIT(mob, xi.mobSkill.PIT_AMBUSH_1 + 1, false)
        assertHidden(mob)

        mob.listeners.WEAPONSKILL_STATE_EXIT(mob, xi.mobSkill.PIT_AMBUSH_1, false)
        assertRevealed(mob, true)

        mob.listeners.DISENGAGE(mob)
        assertHidden(mob)
    end)

    it('reveals no-rehide ambushers once and does not queue a second ambush', function()
        local mob = newMob()
        noRehide(mob)

        mob.listeners.PRESPAWN(mob)
        assertHidden(mob)

        mob.listeners.ENGAGE(mob)
        assert(#mob.abilities == 1)
        assert(mob.abilities[1] == xi.mobSkill.PIT_AMBUSH_1)

        mob.listeners.WEAPONSKILL_STATE_EXIT(mob, xi.mobSkill.PIT_AMBUSH_1, false)
        assertRevealed(mob, false)
        assert(mob.localVars['[Ambush]Done'] == 1)

        mob.listeners.ENGAGE(mob)
        assert(#mob.abilities == 1)
        assert(mob.listeners.DISENGAGE == nil)
    end)
end)
