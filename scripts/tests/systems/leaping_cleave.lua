require('scripts/actions/mobskills/leaping_cleave')

describe('Leaping Cleave mob skill', function()
    it('requires animation sub 0', function()
        local cleave = require('scripts/actions/mobskills/leaping_cleave')
        assert(cleave.onMobSkillCheck({}, { getAnimationSub = function() return 0 end }, {}) == 0)
        assert(cleave.onMobSkillCheck({}, { getAnimationSub = function() return 1 end }, {}) == 1)
    end)

    it('uses a physical Slashing plan and applies TP-scaled Stun only after processing', function()
        local cleave = require('scripts/actions/mobskills/leaping_cleave')
        local move, process, status, duration = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, xi.mobskills.calculateDuration
        local params, damage, stun = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end, getTP = function() return 1500 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) stun = { ... } end
        xi.mobskills.calculateDuration = function(tp, minimum, maximum)
            assert(tp == 1500 and minimum == 15 and maximum == 30)
            return 22.5
        end

        assert(cleave.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 2.25 and params.fTP[2] == 2.25 and params.fTP[3] == 2.25)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and stun == nil)

        xi.mobskills.processDamage = function() return true end
        assert(cleave.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, xi.mobskills.calculateDuration = move, process, status, duration
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(stun[1] == xi.effect.STUN and stun[2] == 1 and stun[3] == 0 and stun[4] == 22.5)
    end)
end)
