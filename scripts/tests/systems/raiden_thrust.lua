require('scripts/actions/mobskills/raiden_thrust')
describe('Raiden Thrust mob skill', function()
    it('uses its thunder magical plan with dStat and damages only after processing', function()
        local thrust = require('scripts/actions/mobskills/raiden_thrust')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.THUNDER }
        end
        xi.mobskills.processDamage = function() return false end
        assert(thrust.onMobSkillCheck(target, mob, {}) == 0 and thrust.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 1 and params.fTP[2] == 2 and params.fTP[3] == 3)
        assert(params.element == xi.element.THUNDER and params.dStatMultiplier == 1)
        assert(params.dStatAttackerMod == xi.mod.INT and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        thrust.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
