require('scripts/actions/mobskills/tourbillion')
describe('Tourbillion mob skill', function()
    it('requires wings up and applies TP-scaled defense down after processing', function()
        local skill = require('scripts/actions/mobskills/tourbillion')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = {
            getAnimationSub = function() return 1 end,
            getWeaponDmg = function() return 80 end,
            getTP = function() return 1000 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.getAnimationSub = function() return 0 end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.numHits == 3 and params.fTP[1] == 1.5 and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.DEFENSE_DOWN and statusParams[4] == 20 and statusParams[6] == 20)
    end)
end)
