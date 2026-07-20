require('scripts/actions/mobskills/stasis')
describe('Stasis mob skill', function()
    it('uses slashing physical plan, applies Paralysis, resets enmity after processing', function()
        local stasis = require('scripts/actions/mobskills/stasis')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, reset = nil, nil, nil, nil
        local mob = { getWeaponDmg = function() return 40 end }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            resetEnmity = function(_, m) reset = m end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 60, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(stasis.onMobSkillCheck(target, mob, {}) == 0 and stasis.onMobWeaponSkill(mob, target, {}, {}) == 60)
        assert(damage == nil and statusParams == nil and reset == nil)
        xi.mobskills.processDamage = function() return true end
        stasis.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 60 and statusParams[3] == xi.effect.PARALYSIS and statusParams[4] == 20 and statusParams[6] == 20)
        assert(reset == mob)
    end)
end)
