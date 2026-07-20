require('scripts/actions/mobskills/rampant_gnaw')
describe('Rampant Gnaw mob skill', function()
    it('uses its blunt physical plan with attack multiplier and applies Paralysis after processing', function()
        local gnaw = require('scripts/actions/mobskills/rampant_gnaw')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(gnaw.onMobSkillCheck(target, mob, {}) == 0 and gnaw.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 1.7 and params.attackMultiplier[1] == 1.5 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        gnaw.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and statusParams[3] == xi.effect.PARALYSIS and statusParams[4] == 20 and statusParams[6] == 120)
    end)
end)
