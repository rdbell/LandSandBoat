require('scripts/actions/mobskills/quadratic_continuum')
describe('Quadratic Continuum mob skill', function()
    it('uses its fourfold slashing plan with attack multiplier and damages only after processing', function()
        local continuum = require('scripts/actions/mobskills/quadratic_continuum')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(continuum.onMobSkillCheck(target, mob, {}) == 0 and continuum.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 4 and params.attackMultiplier[1] == 0.80 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        continuum.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
