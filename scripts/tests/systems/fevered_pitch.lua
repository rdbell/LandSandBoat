describe('Fevered Pitch mob skill', function()
    it('rejects standing Gnoles and applies Defense Down then Stun only after processing', function()
        local feveredPitch = require('scripts/actions/mobskills/fevered_pitch')
        local physicalMove, processDamage, statusMove = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local mob = {
            getWeaponDmg = function() return 77 end,
            getAnimationSub = function() return 0 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) table.insert(effects, { ... }) end
        assert(feveredPitch.onMobSkillCheck(target, { getAnimationSub = function() return 1 end }, {}) == 1)
        assert(feveredPitch.onMobSkillCheck(target, mob, {}) == 0)
        assert(feveredPitch.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil and #effects == 0)
        xi.mobskills.processDamage = function() return true end
        assert(feveredPitch.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = physicalMove, processDamage, statusMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(#effects == 2)
        assert(effects[1][1] == xi.effect.DEFENSE_DOWN and effects[1][2] == 20 and effects[1][3] == 0 and effects[1][4] == 120)
        assert(effects[2][1] == xi.effect.STUN and effects[2][2] == 1 and effects[2][3] == 0 and effects[2][4] == 4)
    end)
end)
