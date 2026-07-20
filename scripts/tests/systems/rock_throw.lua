require('scripts/actions/mobskills/rock_throw')
describe('Rock Throw mob skill', function()
    it('uses ranged slashing plan and applies Slow after processing', function()
        local throw = require('scripts/actions/mobskills/rock_throw')
        local move, process, status = xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.RANGED, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(throw.onMobSkillCheck(target, mob, {}) == 0 and throw.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 3.5 and params.skipParry and params.attackType == xi.attackType.RANGED and damage == nil)
        xi.mobskills.processDamage = function() return true end
        throw.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and statusParams[3] == xi.effect.SLOW and statusParams[4] == 3000 and statusParams[5] == 3)
    end)
end)
