require('scripts/actions/mobskills/smash_axe')
describe('Smash Axe mob skill', function()
    it('uses slashing physical plan and applies Stun after processing', function()
        local axe = require('scripts/actions/mobskills/smash_axe')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 50, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(axe.onMobSkillCheck(target, mob, {}) == 0 and axe.onMobWeaponSkill(mob, target, {}, {}) == 50)
        assert(params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        axe.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 50 and statusParams[3] == xi.effect.STUN and statusParams[6] == 4)
    end)
end)
