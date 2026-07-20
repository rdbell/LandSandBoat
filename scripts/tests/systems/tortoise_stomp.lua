require('scripts/actions/mobskills/tortoise_stomp')
describe('Tortoise Stomp mob skill', function()
    it('uses blunt plan and random defense down after processing', function()
        local skill = require('scripts/actions/mobskills/tortoise_stomp')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local random = math.random
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 80 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.damageType == xi.damageType.BLUNT and statusParams == nil)
        math.random = function(a, b) assert(a == 120 and b == 180); return 150 end
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        math.random = random
        assert(damage[1] == 100 and statusParams[3] == xi.effect.DEFENSE_DOWN and statusParams[4] == 25 and statusParams[6] == 150)
    end)
end)
