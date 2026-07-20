require('scripts/actions/mobskills/blood_drain')
describe('Blood Drain mob skill', function()
    it('uses NONE drain plan and Asanbosam ignores shadows', function()
        local skill = require('scripts/actions/mobskills/blood_drain')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local drainMove = xi.mobskills.mobDrainMove
        local params, message, drain = nil, nil, nil
        local mob = {
            getMainLvl = function() return 50 end,
            getPool = function() return 0 end,
        }
        local sk = { setMsg = function(_, v) message = v end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobDrainMove = function(_, _, dtype, amount, at, dt)
            drain = { dtype, amount, at, dt }
            return 123
        end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 90)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.0 and params.fTP[3] == 2.84)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and message == nil)
        mob.getPool = function() return xi.mobPool.ASANBOSAM end
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 90)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobDrainMove = drainMove
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(drain[1] == xi.mobskills.drainType.HP and message == 123)
    end)
end)
