require('scripts/actions/mobskills/trembling')
describe('Trembling mob skill', function()
    it('uses blunt fTP 4 plan and dispels after processing', function()
        local skill = require('scripts/actions/mobskills/trembling')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, dispelled = nil, nil, false
        local mob = { getWeaponDmg = function() return 80 end }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            dispelStatusEffect = function() dispelled = true end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 4.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and not dispelled)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 100 and dispelled)
    end)
end)
