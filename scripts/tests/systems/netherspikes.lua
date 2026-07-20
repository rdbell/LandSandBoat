require('scripts/actions/mobskills/netherspikes')

describe('Netherspikes mob skill', function()
    it('uses its Slashing physical plan and binds only after processing', function()
        local spikes = require('scripts/actions/mobskills/netherspikes')
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
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) bind = { ... } end

        assert(spikes.onMobSkillCheck(target, mob, skill) == 0 and spikes.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil and bind == nil)

        xi.mobskills.processDamage = function() return true end
        spikes.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(takeOpts and takeOpts.breakBind == false)
        assert(bind[3] == xi.effect.BIND and bind[4] == 1 and bind[5] == 0 and bind[6] == 30)
    end)
end)
