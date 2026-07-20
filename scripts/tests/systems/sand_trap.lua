require('scripts/actions/mobskills/sand_trap')
describe('Sand Trap mob skill', function()
    it('uses slashing physical plan and applies Petrification after processing', function()
        local trap = require('scripts/actions/mobskills/sand_trap')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, enmity = nil, nil, nil, nil
        local mob = {
            getWeaponDmg = function() return 50 end,
            setEnmityActive = function(_, t, active) enmity = { t, active } end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 99, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(trap.onMobSkillCheck(target, mob, {}) == 0 and trap.onMobWeaponSkill(mob, target, {}, {}) == 99)
        assert(params.fTP[1] == 0.5 and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil and statusParams == nil and enmity == nil)
        xi.mobskills.processDamage = function() return true end
        trap.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 99 and statusParams[3] == xi.effect.PETRIFICATION and statusParams[6] == 15)
        assert(enmity[1] == target and enmity[2] == false)
    end)
end)
