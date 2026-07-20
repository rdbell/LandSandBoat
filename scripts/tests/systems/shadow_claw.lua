require('scripts/actions/mobskills/shadow_claw')
describe('Shadow Claw mob skill', function()
    it('uses blunt physical plan and applies Blindness after processing', function()
        local claw = require('scripts/actions/mobskills/shadow_claw')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 33 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 44, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(claw.onMobSkillCheck(target, mob, {}) == 0 and claw.onMobWeaponSkill(mob, target, {}, {}) == 44)
        assert(params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        claw.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 44 and statusParams[3] == xi.effect.BLINDNESS and statusParams[4] == 30 and statusParams[6] == 30)
    end)
end)
