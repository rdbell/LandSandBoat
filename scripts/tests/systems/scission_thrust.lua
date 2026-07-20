require('scripts/actions/mobskills/scission_thrust')
describe('Scission Thrust mob skill', function()
    it('requires sword form and uses blunt physical plan with crit', function()
        local thrust = require('scripts/actions/mobskills/scission_thrust')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, anim = nil, nil, 0
        local mob = {
            getAnimationSub = function() return anim end,
            getWeaponDmg = function() return 70 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        anim = 0; assert(thrust.onMobSkillCheck(target, mob, {}) == 1)
        anim = 1; assert(thrust.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(thrust.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.canCrit and params.criticalChance[1] == 0.10 and params.criticalChance[3] == 0.25 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        thrust.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 90)
    end)
end)
