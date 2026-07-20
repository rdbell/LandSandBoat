require('scripts/actions/mobskills/wing_whirl')
describe('Wing Whirl mob skill', function()
    it('delegates Apkallu check and uses four-hit Slashing plan', function()
        local skill = require('scripts/actions/mobskills/wing_whirl')
        local canUse = xi.apkallu.canUseAbility
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage, threshold = nil, nil, nil
        xi.apkallu.canUseAbility = function(_, t) threshold = t; return 0 end
        local mob = { getWeaponDmg = function() return 60 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(threshold == 40)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.numHits == 4 and params.fTP[1] == 1.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_4)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        xi.apkallu.canUseAbility = canUse
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 100)
    end)
end)
