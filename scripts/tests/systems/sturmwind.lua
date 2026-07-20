require('scripts/actions/mobskills/sturmwind')
describe('Sturmwind mob skill', function()
    it('uses twofold slashing plan with TP-scaled attack multiplier', function()
        local wind = require('scripts/actions/mobskills/sturmwind')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(wind.onMobSkillCheck(target, mob, {}) == 0 and wind.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.numHits == 2 and params.attackMultiplier[1] == 1.0 and params.attackMultiplier[2] == 2.0 and params.attackMultiplier[3] == 3.5 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        wind.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 100)
    end)
end)
