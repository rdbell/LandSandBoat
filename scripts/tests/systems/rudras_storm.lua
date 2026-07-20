require('scripts/actions/mobskills/rudras_storm')
describe("Rudra's Storm mob skill", function()
    it('uses piercing physical plan and applies Weight after processing', function()
        local storm = require('scripts/actions/mobskills/rudras_storm')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(storm.onMobSkillCheck(target, mob, {}) == 0 and storm.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 3.25 and params.fTP[2] == 4.25 and params.fTP[3] == 5.25 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        storm.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and statusParams[3] == xi.effect.WEIGHT and statusParams[4] == 25 and statusParams[6] == 60)
    end)
end)
