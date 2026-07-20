require('scripts/actions/mobskills/roller_chain')
describe('Roller Chain mob skill', function()
    it('requires closed door animation sub and applies Bind after processing', function()
        local chain = require('scripts/actions/mobskills/roller_chain')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local animSub, params, damage, statusParams = 0, nil, nil, nil
        local mob = {
            getAnimationSub = function() return animSub end,
            getWeaponDmg = function() return 77 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        animSub = 1; assert(chain.onMobSkillCheck(target, mob, {}) == 1)
        animSub = 0; assert(chain.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(chain.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 2 and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        chain.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and statusParams[3] == xi.effect.BIND and statusParams[6] == 30)
    end)
end)
