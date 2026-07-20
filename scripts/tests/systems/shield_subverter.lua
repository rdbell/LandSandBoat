require('scripts/actions/mobskills/shield_subverter')
describe('Shield Subverter mob skill', function()
    it('uses blunt physical plan and applies Silence after processing', function()
        local sub = require('scripts/actions/mobskills/shield_subverter')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 30 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 45, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(sub.onMobSkillCheck(target, mob, {}) == 0 and sub.onMobWeaponSkill(mob, target, {}, {}) == 45)
        assert(params.fTP[1] == 1.0 and params.fTP[2] == 2.0 and params.fTP[3] == 3.0 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        sub.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 45 and statusParams[3] == xi.effect.SILENCE and statusParams[6] == 15)
    end)
end)
