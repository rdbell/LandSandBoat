require('scripts/actions/mobskills/randgrith')
describe('Randgrith mob skill', function()
    it('uses its blunt physical plan and applies Evasion Down after processing', function()
        local skill = require('scripts/actions/mobskills/randgrith')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0 and skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 2.75 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and statusParams[3] == xi.effect.EVASION_DOWN and statusParams[4] == 32 and statusParams[6] == 120)
    end)
end)
