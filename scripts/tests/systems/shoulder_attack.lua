require('scripts/actions/mobskills/shoulder_attack')
describe('Shoulder Attack mob skill', function()
    it('uses slashing physical plan and applies Stun after processing', function()
        local atk = require('scripts/actions/mobskills/shoulder_attack')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 30 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 45, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(atk.onMobSkillCheck(target, mob, {}) == 0 and atk.onMobWeaponSkill(mob, target, {}, {}) == 45)
        assert(params.fTP[1] == 1.5 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        atk.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 45 and statusParams[3] == xi.effect.STUN and statusParams[6] == 4)
    end)
end)
