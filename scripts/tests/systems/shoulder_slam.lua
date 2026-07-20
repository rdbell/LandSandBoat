require('scripts/actions/mobskills/shoulder_slam')
describe('Shoulder Slam mob skill', function()
    it('uses slashing physical plan and damages only after processing', function()
        local slam = require('scripts/actions/mobskills/shoulder_slam')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 25 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 30, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(slam.onMobSkillCheck(target, mob, {}) == 0 and slam.onMobWeaponSkill(mob, target, {}, {}) == 30)
        assert(params.fTP[1] == 1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        slam.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 30)
    end)
end)
