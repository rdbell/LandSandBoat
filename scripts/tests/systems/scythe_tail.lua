require('scripts/actions/mobskills/scythe_tail')
describe('Scythe Tail mob skill', function()
    it('uses slashing physical plan and applies Stun after processing', function()
        local tail = require('scripts/actions/mobskills/scythe_tail')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 55 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 66, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(tail.onMobSkillCheck(target, mob, {}) == 0 and tail.onMobWeaponSkill(mob, target, {}, {}) == 66)
        assert(params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        tail.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 66 and statusParams[3] == xi.effect.STUN and statusParams[6] == 4)
    end)
end)
