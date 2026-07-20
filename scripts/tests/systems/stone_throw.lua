require('scripts/actions/mobskills/stone_throw')
describe('Stone Throw mob skill', function()
    it('uses ranged blunt plan and applies Paralysis after processing', function()
        local thr = require('scripts/actions/mobskills/stone_throw')
        local move, process, status = xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 20 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobRangedMove = function(_, _, _, _, value)
            params = value
            return { damage = 30, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(thr.onMobSkillCheck(target, mob, {}) == 0 and thr.onMobWeaponSkill(mob, target, {}, {}) == 30)
        assert(params.fTP[1] == 1.5 and params.skipParry and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        thr.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobRangedMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 30 and statusParams[3] == xi.effect.PARALYSIS and statusParams[4] == 50 and statusParams[6] == 60)
    end)
end)
