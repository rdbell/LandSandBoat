require('scripts/actions/mobskills/sonic_thrust')
describe('Sonic Thrust mob skill', function()
    it('uses piercing physical plan with TP-scaled fTP and damages only after processing', function()
        local thrust = require('scripts/actions/mobskills/sonic_thrust')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 120, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(thrust.onMobSkillCheck(target, mob, {}) == 0 and thrust.onMobWeaponSkill(mob, target, {}, {}) == 120)
        assert(params.fTP[1] == 3.0 and params.fTP[2] == 3.25 and params.fTP[3] == 3.5 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        thrust.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 120)
    end)
end)
