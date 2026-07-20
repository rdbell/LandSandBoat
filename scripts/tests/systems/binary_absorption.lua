require('scripts/actions/mobskills/binary_absorption')
describe('Binary Absorption mob skill', function()
    it('uses NONE magical plan with skipMagicBonusDiff and HP drain', function()
        local skill = require('scripts/actions/mobskills/binary_absorption')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local drainMove = xi.mobskills.mobDrainMove
        local params, message, drain = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
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
        assert(params.baseDamage == 50 and params.fTP[1] == 3.5 and params.skipMagicBonusDiff == true)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and message == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 90)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobDrainMove = drainMove
        assert(drain[1] == xi.mobskills.drainType.HP and drain[2] == 90 and message == 123)
    end)
end)
