require('scripts/actions/mobskills/leg_sweep')

describe('Leg Sweep mob skill', function()
    it('uses a physical Piercing plan and applies Stun only after processing succeeds', function()
        local legSweep = require('scripts/actions/mobskills/leg_sweep')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, stun = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) stun = { ... } end

        assert(legSweep.onMobSkillCheck(target, mob, {}) == 0)
        assert(legSweep.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and stun == nil)

        xi.mobskills.processDamage = function() return true end
        assert(legSweep.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
        assert(stun[1] == xi.effect.STUN and stun[2] == 1 and stun[3] == 0 and stun[4] == 4)
    end)
end)
