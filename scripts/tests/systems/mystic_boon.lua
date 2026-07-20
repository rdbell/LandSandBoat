require('scripts/actions/mobskills/mystic_boon')

describe('Mystic Boon mob skill', function()
    it('uses its Blunt physical plan and converts damage to MP only after processing', function()
        local boon = require('scripts/actions/mobskills/mystic_boon')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, mp = nil, nil, nil
        local mob = {
            getWeaponDmg = function() return 77 end,
            addMP = function(_, value) mp = value end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = {}

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end

        assert(boon.onMobSkillCheck(target, mob, skill) == 0 and boon.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 1.0 and params.fTP[2] == 1.5 and params.fTP[3] == 2.0)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and mp == nil)

        xi.mobskills.processDamage = function() return true end
        boon.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
        assert(mp == 123)
    end)
end)
