require('scripts/actions/mobskills/moonlit_charge')

describe('Moonlit Charge mob skill', function()
    it('uses its Slashing physical plan and blinds only after processing', function()
        local charge = require('scripts/actions/mobskills/moonlit_charge')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, blind = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = {}

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) blind = { ... } end

        assert(charge.onMobSkillCheck(target, mob, skill) == 0 and charge.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 1.5 and params.fTP[2] == 1.5 and params.fTP[3] == 1.5)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and blind == nil)

        xi.mobskills.processDamage = function() return true end
        charge.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(blind[3] == xi.effect.BLINDNESS and blind[4] == 25 and blind[5] == 0 and blind[6] == 120)
    end)
end)
