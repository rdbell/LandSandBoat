local chigoe = require('scripts/mixins/families/chigoe')

local lethalAbilities = {
    xi.jobAbility.SHIELD_BASH,
    xi.jobAbility.JUMP,
    xi.jobAbility.HIGH_JUMP,
    xi.jobAbility.WEAPON_BASH,
    xi.jobAbility.CHI_BLAST,
    xi.jobAbility.TOMAHAWK,
    xi.jobAbility.ANGON,
    xi.jobAbility.QUICKSTEP,
    xi.jobAbility.BOX_STEP,
    xi.jobAbility.STUTTER_STEP,
    xi.jobAbility.FEATHER_STEP,
}

local function newMob()
    local mob = { hidden = nil, hp = 100, listeners = {}, mobMods = {}, untargetable = nil }

    function mob:addListener(event, _, callback)
        self.listeners[event] = callback
    end

    function mob:hideName(value)
        self.hidden = value
    end

    function mob:setUntargetable(value)
        self.untargetable = value
    end

    function mob:setMobMod(mod, value)
        self.mobMods[mod] = value
    end

    function mob:setHP(value)
        self.hp = value
    end

    return mob
end

local function assertKilled(mob)
    assert(mob.mobMods[xi.mobMod.EXP_BONUS] == -100)
    assert(mob.mobMods[xi.mobMod.NO_DROPS] == 1)
    assert(mob.hp == 0)
end

describe('Chigoe mixin', function()
    it('changes visibility and dies to its configured attacks', function()
        local mob = newMob()
        chigoe(mob)

        mob.listeners.SPAWN(mob)
        assert(mob.hidden == true and mob.untargetable == true)
        mob.listeners.ENGAGE(mob)
        assert(mob.hidden == false and mob.untargetable == false)
        mob.listeners.DISENGAGE(mob)
        assert(mob.hidden == true and mob.untargetable == true)

        for _, ability in ipairs(lethalAbilities) do
            assert(xi.mix.chigoe.isLethalAbility(ability) == true)
        end
        assert(xi.mix.chigoe.isLethalAbility(0) == false)

        mob.listeners.CRITICAL_TAKE(mob)
        assertKilled(mob)

        mob = newMob()
        chigoe(mob)
        mob.listeners.WEAPONSKILL_TAKE(nil, mob)
        assertKilled(mob)

        mob = newMob()
        chigoe(mob)
        local lethalSkill = { getID = function() return xi.jobAbility.JUMP end }
        mob.listeners.ABILITY_TAKE(nil, mob, lethalSkill)
        assertKilled(mob)

        mob = newMob()
        chigoe(mob)
        local harmlessSkill = { getID = function() return 0 end }
        mob.listeners.ABILITY_TAKE(nil, mob, harmlessSkill)
        assert(mob.hp == 100 and next(mob.mobMods) == nil)
    end)
end)
