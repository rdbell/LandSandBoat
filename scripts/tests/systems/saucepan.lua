require('scripts/actions/mobskills/saucepan')
describe('Saucepan mob skill', function()
    it('uses blunt physical plan and force-feeds FOOD after processing', function()
        local pan = require('scripts/actions/mobskills/saucepan')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, hasFood, stripped, added = nil, nil, false, false, nil
        local mob = { getWeaponDmg = function() return 30 end }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            hasStatusEffect = function(_, e) return e == xi.effect.FOOD and hasFood end,
            delStatusEffectSilent = function(_, e) if e == xi.effect.FOOD then stripped = true end end,
            addStatusEffect = function(_, e, opts) added = { e, opts } end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 44, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(pan.onMobSkillCheck(target, mob, {}) == 0 and pan.onMobWeaponSkill(mob, target, {}, {}) == 44)
        assert(params.fTP[1] == 0.8 and damage == nil and added == nil)
        hasFood = true
        xi.mobskills.processDamage = function() return true end
        pan.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 44 and stripped and added[1] == xi.effect.FOOD)
        assert(added[2].power == 255 and added[2].duration == 1800 and added[2].sourceType == xi.effectSourceType.FOOD)
    end)
end)
