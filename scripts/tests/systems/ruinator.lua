require('scripts/actions/mobskills/ruinator')
describe('Ruinator mob skill', function()
    it('uses fourfold slashing plan with accuracy and attack multipliers', function()
        local ruinator = require('scripts/actions/mobskills/ruinator')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(ruinator.onMobSkillCheck(target, mob, {}) == 0 and ruinator.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 4 and params.fTP[1] == 1.08 and params.accuracyModifier[2] == 30 and params.attackMultiplier[1] == 1.1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        ruinator.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
