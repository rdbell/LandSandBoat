require('scripts/actions/mobskills/sickle_moon')
describe('Sickle Moon mob skill', function()
    it('uses twofold slashing plan and damages only after processing', function()
        local moon = require('scripts/actions/mobskills/sickle_moon')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 120, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(moon.onMobSkillCheck(target, mob, {}) == 0 and moon.onMobWeaponSkill(mob, target, {}, {}) == 120)
        assert(params.numHits == 2 and params.fTP[1] == 1.5 and params.fTP[2] == 2.0 and params.fTP[3] == 2.75 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        moon.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 120)
    end)
end)
