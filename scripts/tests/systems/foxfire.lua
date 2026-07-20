describe('Foxfire mob skill', function()
    it('allows only the listed Fomor jobs and applies its blunt plan and Stun after processing', function()
        local foxfire = require('scripts/actions/mobskills/foxfire')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, stun, job = nil, nil, nil, xi.job.RDM
        local mob = { getMainJob = function() return job end, getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) stun = { ... } end
        for _, allowed in ipairs({ xi.job.RDM, xi.job.THF, xi.job.PLD, xi.job.BST, xi.job.BRD, xi.job.RNG, xi.job.NIN, xi.job.COR }) do
            job = allowed
            assert(foxfire.onMobSkillCheck(target, mob, {}) == 0)
        end
        job = xi.job.WAR
        assert(foxfire.onMobSkillCheck(target, mob, {}) == 1)
        job = xi.job.RDM
        assert(foxfire.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2 and params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil and stun == nil)
        xi.mobskills.processDamage = function() return true end
        foxfire.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT and stun[1] == mob and stun[2] == target and stun[3] == xi.effect.STUN and stun[4] == 1 and stun[5] == 0 and stun[6] == 15)
    end)
end)
