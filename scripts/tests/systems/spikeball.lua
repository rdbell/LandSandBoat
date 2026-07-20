require('scripts/actions/mobskills/spikeball')
describe('Spikeball mob skill', function()
    it('uses piercing physical plan and applies Poison after processing', function()
        local ball = require('scripts/actions/mobskills/spikeball')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 20 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 30, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(ball.onMobSkillCheck(target, mob, {}) == 0 and ball.onMobWeaponSkill(mob, target, {}, {}) == 30)
        assert(params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        ball.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 30 and statusParams[3] == xi.effect.POISON and statusParams[4] == 16 and statusParams[5] == 3 and statusParams[6] == 60)
    end)
end)
