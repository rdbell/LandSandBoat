require('scripts/actions/mobskills/wild_horn')
describe('Wild Horn mob skill', function()
    it('denies behind targets and uses its three-hit physical plan', function()
        local skill = require('scripts/actions/mobskills/wild_horn')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 80 end }
        local target = {
            isBehind = function(_, _, cone) assert(cone == 48); return false end,
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
        target.isBehind = function() return true end
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        target.isBehind = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 80 and params.numHits == 3 and params.fTP[1] == 1.5)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 100 and damage[3] == xi.attackType.PHYSICAL)
    end)
end)
