require('scripts/actions/mobskills/shadow_thrust')
describe('Shadow Thrust mob skill', function()
    it('uses piercing physical plan and damages only after processing', function()
        local thrust = require('scripts/actions/mobskills/shadow_thrust')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 60 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 150, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(thrust.onMobSkillCheck(target, mob, {}) == 0 and thrust.onMobWeaponSkill(mob, target, {}, {}) == 150)
        assert(params.fTP[1] == 3 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        thrust.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 150)
    end)
end)
