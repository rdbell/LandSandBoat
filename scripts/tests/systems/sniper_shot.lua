require('scripts/actions/mobskills/sniper_shot')
describe('Sniper Shot mob skill', function()
    it('uses ranged piercing plan with crit and applies INT Down after processing', function()
        local shot = require('scripts/actions/mobskills/sniper_shot')
        local move, process, status = xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 35 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 40, attackType = xi.attackType.RANGED, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(shot.onMobSkillCheck(target, mob, {}) == 0 and shot.onMobWeaponSkill(mob, target, {}, {}) == 40)
        assert(params.canCrit and params.skipParry and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        shot.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 40 and statusParams[3] == xi.effect.INT_DOWN and statusParams[4] == 10 and statusParams[6] == 140)
    end)
end)
