describe('Imperator mob skill', function()
    it('uses its TP-varying Slashing physical plan and damages only after processing', function()
        local imperator = require('scripts/actions/mobskills/imperator')
        local move = xi.mobskills.mobPhysicalMove
        local process = xi.mobskills.processDamage
        local params, damage
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end

        assert(imperator.onMobSkillCheck(target, mob, {}) == 0)
        assert(imperator.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3.75 and params.fTP[2] == 7.5 and params.fTP[3] == 11.75)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        imperator.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove = move
        xi.mobskills.processDamage = process

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
    end)
end)
