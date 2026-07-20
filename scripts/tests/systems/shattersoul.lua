require('scripts/actions/mobskills/shattersoul')
describe('Shattersoul mob skill', function()
    it('uses threefold blunt plan and applies Magic Def Down after processing', function()
        local soul = require('scripts/actions/mobskills/shattersoul')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(soul.onMobSkillCheck(target, mob, {}) == 0 and soul.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.numHits == 3 and params.fTP[1] == 1.375 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        soul.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 80 and statusParams[3] == xi.effect.MAGIC_DEF_DOWN and statusParams[4] == 10 and statusParams[6] == 120)
    end)
end)
