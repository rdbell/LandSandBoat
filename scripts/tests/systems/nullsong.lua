require('scripts/actions/mobskills/nullsong')
describe('Nullsong mob skill', function()
    it('gates on silence and dispelable count then damages per dispel', function()
        local skill = require('scripts/actions/mobskills/nullsong')
        local params = nil
        local origM, origD = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 200, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { hasStatusEffect = function(_, e) return e == xi.effect.SILENCE and false end }
        local target = {
            takeDamage = function() end,
            countEffectWithFlag = function() return 3 end,
            dispelAllStatusEffect = function() return 4 end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        mob.hasStatusEffect = function() return true end
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.hasStatusEffect = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.baseDamage == 117 and params.fTP[1] == 4 and params.skipDamageAdjustment)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = origM, origD
    end)
end)
