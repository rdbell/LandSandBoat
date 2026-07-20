require('scripts/actions/mobskills/mountain_buster')

describe('Mountain Buster mob skill', function()
    it('uses its Blunt physical plan and binds only after processing', function()
        local buster = require('scripts/actions/mobskills/mountain_buster')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, bind, takeOpts = nil, nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = {
            takeDamage = function(_, dmg, source, attackType, damageType, opts)
                damage = { dmg, source, attackType, damageType }
                takeOpts = opts
            end,
        }
        local skill = {}

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) bind = { ... } end

        assert(buster.onMobSkillCheck(target, mob, skill) == 0 and buster.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and bind == nil)

        xi.mobskills.processDamage = function() return true end
        buster.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
        assert(takeOpts and takeOpts.breakBind == false)
        assert(bind[3] == xi.effect.BIND and bind[4] == 1 and bind[5] == 0 and bind[6] == 60)
    end)
end)
