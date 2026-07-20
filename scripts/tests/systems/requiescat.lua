require('scripts/actions/mobskills/requiescat')
describe('Requiescat mob skill', function()
    it('uses its fivefold slashing plan with TP-scaled attack multiplier', function()
        local req = require('scripts/actions/mobskills/requiescat')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(req.onMobSkillCheck(target, mob, {}) == 0 and req.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 5 and params.attackMultiplier[1] == 0.8 and params.attackMultiplier[2] == 0.9 and params.attackMultiplier[3] == 1.0 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        req.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
