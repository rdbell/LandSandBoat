require('scripts/actions/mobskills/blood_saber')
describe('Blood Saber mob skill', function()
    it('uses NONE magical drain plan with dStat 1', function()
        local skill = require('scripts/actions/mobskills/blood_saber')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local drainMove = xi.mobskills.mobDrainMove
        local params, message = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local sk = { setMsg = function(_, v) message = v end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 70, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobDrainMove = function() return 99 end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 70)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.5 and params.dStatMultiplier == 1)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and message == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == 70)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobDrainMove = drainMove
        assert(message == 99)
    end)
end)
