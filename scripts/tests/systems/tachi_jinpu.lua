require('scripts/actions/mobskills/tachi_jinpu')
describe('Tachi Jinpu mob skill', function()
    it('uses hybrid physical+wind plan and sums damage', function()
        local skill = require('scripts/actions/mobskills/tachi_jinpu')
        local params, total = nil, 0
        local origP, origD = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return {
                damage = 50, hybridDamage = 30,
                attackType = p.attackType, damageType = p.damageType,
                hybridAttackType = p.hybridAttackType, hybridDamageType = p.hybridDamageType,
            }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getWeaponDmg = function() return 40 end }
        local target = {
            takeDamage = function(_, d) total = total + d end,
            getHP = function() return 100 end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.hybridSkill and params.hybridSkillElement == xi.element.WIND and total == 80)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
