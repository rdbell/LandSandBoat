describe('Formation Attack mob skill', function()
    it('uses its animation-dependent blunt physical plan and damages only after processing', function()
        local attack = require('scripts/actions/mobskills/formation_attack')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, animation = nil, nil, 0
        local mob = { getWeaponDmg = function() return 77 end, getAnimationSub = function() return animation end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(attack.onMobSkillCheck(target, mob, {}) == 0 and attack.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil)
        animation = 5
        attack.onMobWeaponSkill(mob, target, {}, {})
        assert(params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        animation = 13
        xi.mobskills.processDamage = function() return true end
        attack.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2 and damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
    end)
end)
