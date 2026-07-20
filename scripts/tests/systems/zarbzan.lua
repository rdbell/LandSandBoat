require('scripts/actions/mobskills/zarbzan')
describe('Zarbzan mob skill', function()
    it('admits RNG only and uses Fire magical plan with dStat 1', function()
        local skill = require('scripts/actions/mobskills/zarbzan')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = {
            getMainJob = function() return xi.job.WAR end,
            getMainLvl = function() return 50 end,
        }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.getMainJob = function() return xi.job.RNG end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.baseDamage == 52 and params.fTP[1] == 2.0 and params.dStatMultiplier == 1)
        assert(params.element == xi.element.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 80)
    end)
end)
