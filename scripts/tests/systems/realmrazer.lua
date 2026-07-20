require('scripts/actions/mobskills/realmrazer')
describe('Realmrazer mob skill', function()
    it('uses its sevenfold blunt plan with accuracy modifiers and damages only after processing', function()
        local razer = require('scripts/actions/mobskills/realmrazer')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(razer.onMobSkillCheck(target, mob, {}) == 0 and razer.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 7 and params.fTP[1] == 0.88 and params.accuracyModifier[2] == 30 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_7 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        razer.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
