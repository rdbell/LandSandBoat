require('scripts/actions/mobskills/pit_ambush')

describe('Pit Ambush mob skill', function()
    it('uses its piercing physical plan with wipe-shadows and damages only after processing', function()
        local ambush = require('scripts/actions/mobskills/pit_ambush')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(ambush.onMobSkillCheck(target, mob, {}) == 0 and ambush.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3.3)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)
        xi.mobskills.processDamage = function() return true end
        ambush.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
    end)
end)
