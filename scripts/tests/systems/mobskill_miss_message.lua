require('scripts/globals/mobskills')

local function plan(hits, yae, ant, abs, shadows, primary, dmg)
    return xi.mobskills.resolveMissMessagePlan(hits, yae, ant, abs, shadows, primary, dmg)
end

describe('Mob skill miss message plan', function()
    it('leaves the message alone when a hit landed', function()
        local p = plan(1, true, true, 2, 3, xi.msg.basic.HIT_DMG, 100)
        assert(not p.setMsg)
        assert(p.totalDamage == 100)
    end)

    it('uses EVADES for Yaegasumi before Third Eye', function()
        local p = plan(0, true, true, 0, 0, xi.msg.basic.HIT_DMG, 50)
        assert(p.setMsg and p.msg == xi.msg.basic.EVADES)
        assert(p.totalDamage == 50)
    end)

    it('uses ANTICIPATE for Third Eye', function()
        local p = plan(0, false, true, 0, 0, xi.msg.basic.HIT_DMG, 50)
        assert(p.setMsg and p.msg == xi.msg.basic.ANTICIPATE)
        assert(p.totalDamage == 50)
    end)

    it('uses SHADOW_ABSORB and rewrites damage to shadows consumed', function()
        local p = plan(0, false, false, 2, 5, xi.msg.basic.HIT_DMG, 99)
        assert(p.setMsg and p.msg == xi.msg.basic.SHADOW_ABSORB)
        assert(p.totalDamage == 5)
    end)

    it('maps primary messages to their miss counterparts', function()
        assert(plan(0, false, false, 0, 0, xi.msg.basic.RANGED_ATTACK_HIT, 40).msg == xi.msg.basic.RANGED_ATTACK_MISS)
        assert(plan(0, false, false, 0, 0, xi.msg.basic.HIT_DMG, 40).msg == xi.msg.basic.HIT_MISS)
        assert(plan(0, false, false, 0, 0, xi.msg.basic.USES_JA_TAKE_DAMAGE, 40).msg == xi.msg.basic.JA_MISS_2)
        assert(plan(0, false, false, 0, 0, 0, 40).msg == xi.msg.basic.SKILL_MISS)
        assert(plan(0, false, false, 0, 0, 0, 40).totalDamage == 0)
    end)
end)
