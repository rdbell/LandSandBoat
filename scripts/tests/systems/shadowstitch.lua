require('scripts/actions/mobskills/shadowstitch')
describe('Shadowstitch mob skill', function()
    it('uses piercing physical plan and applies TP-scaled Bind after processing', function()
        local stitch = require('scripts/actions/mobskills/shadowstitch')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 25 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1000 end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 40, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(stitch.onMobSkillCheck(target, mob, skill) == 0 and stitch.onMobWeaponSkill(mob, target, skill, {}) == 40)
        assert(params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        stitch.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 40 and statusParams[3] == xi.effect.BIND and statusParams[6] == 10)
    end)
end)
