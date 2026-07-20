describe('Foot Kick mob skill', function()
    it('uses its level-dependent critical physical slashing plan and damages only after processing', function()
        local kick = require('scripts/actions/mobskills/foot_kick')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, level = nil, nil, 49
        local mob = { getWeaponDmg = function() return 77 end, getMainLvl = function() return level end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(kick.onMobSkillCheck(target, mob, {}) == 0 and kick.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and params.canCrit and params.criticalChance[1] == 1 and params.criticalChance[2] == 1 and params.criticalChance[3] == 1 and damage == nil)
        level = 50
        xi.mobskills.processDamage = function() return true end
        kick.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2 and damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
    end)
end)
