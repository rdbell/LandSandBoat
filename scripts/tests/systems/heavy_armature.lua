require('scripts/actions/mobskills/heavy_armature')
describe('Heavy Armature mob skill', function()
    it('admits Armed Gears and applies Haste Protect Blink', function()
        local skill = require('scripts/actions/mobskills/heavy_armature')
        local buffMove = xi.mobskills.mobBuffMove
        local buffs, msg = {}, nil
        local origRandom = math.random
        math.random = function(a, b)
            if a == 10 and b == 25 then return 15 end
            return origRandom(a, b)
        end
        local mob = { getPool = function() return 0 end }
        local sk = { setMsg = function(_, m) msg = m end }
        assert(skill.onMobSkillCheck({}, mob, sk) == 1)
        mob.getPool = function() return xi.mobPool.ARMED_GEARS end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        xi.mobskills.mobBuffMove = function(_, effect, power, tick, duration)
            buffs[#buffs+1] = { effect, power, tick, duration }
            return 100
        end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.BLINK)
        assert(#buffs == 3)
        assert(buffs[1][1] == xi.effect.HASTE and buffs[1][2] == 4375)
        assert(buffs[2][1] == xi.effect.PROTECT and buffs[2][2] == 100)
        assert(buffs[3][1] == xi.effect.BLINK and buffs[3][2] == 15 and buffs[3][4] == 120)
        assert(msg == 100)
        math.random = origRandom
        xi.mobskills.mobBuffMove = buffMove
    end)
end)
