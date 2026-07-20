require('scripts/actions/mobskills/zesho_meppo')
describe('Zesho Meppo mob skill', function()
    it('uses four-hit TP-scaled fTP plan and processed damage', function()
        local skill = require('scripts/actions/mobskills/zesho_meppo')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.numHits == 4 and params.fTP[1] == 4.0 and params.fTP[2] == 11.3575 and params.fTP[3] == 18.715)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_4 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 200)
    end)
end)
