describe('Frenetic Rip mob skill', function()
    it('rejects only Horned Imps at animation sub 4 and uses its three-hit blunt plan', function()
        local rip = require('scripts/actions/mobskills/frenetic_rip')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getAnimationSub = function() return 1 end, getSpecies = function() return xi.mobSpecies.HORNED_IMP end, getWeaponDmg = function() return 77 end }
        local rejected = { getAnimationSub = function() return 4 end, getSpecies = function() return xi.mobSpecies.HORNED_IMP end }
        local other = { getAnimationSub = function() return 4 end, getSpecies = function() return 1 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT } end
        xi.mobskills.processDamage = function() return false end
        assert(rip.onMobSkillCheck(target, rejected, {}) == 1 and rip.onMobSkillCheck(target, other, {}) == 0 and rip.onMobSkillCheck(target, mob, {}) == 0 and rip.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 3 and params.fTP[1] == 1.334 and params.fTP[2] == 1.334 and params.fTP[3] == 1.334 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        rip.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
    end)
end)
