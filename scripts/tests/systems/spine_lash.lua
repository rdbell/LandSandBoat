require('scripts/actions/mobskills/spine_lash')
describe('Spine Lash mob skill', function()
    it('uses slashing physical plan with crit and applies Plague after processing', function()
        local lash = require('scripts/actions/mobskills/spine_lash')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 35 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 40, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(lash.onMobSkillCheck(target, mob, {}) == 0 and lash.onMobWeaponSkill(mob, target, {}, {}) == 40)
        assert(params.canCrit and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        lash.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 40 and statusParams[3] == xi.effect.PLAGUE and statusParams[4] == 5 and statusParams[6] == 120)
    end)
end)
