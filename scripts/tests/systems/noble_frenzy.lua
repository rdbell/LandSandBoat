require('scripts/actions/mobskills/noble_frenzy')

describe('Noble Frenzy mob skill', function()
    it('uses its five-hit Slashing plan and damages only after processing', function()
        local frenzy = require('scripts/actions/mobskills/noble_frenzy')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = {}

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end

        assert(frenzy.onMobSkillCheck(target, mob, skill) == 0 and frenzy.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 5)
        assert(params.fTP[1] == 5.46875 and params.fTP[2] == 6.56250 and params.fTP[3] == 8.75000)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_5 and damage == nil)

        xi.mobskills.processDamage = function() return true end
        frenzy.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
    end)
end)
