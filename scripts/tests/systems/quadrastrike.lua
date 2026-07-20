require('scripts/actions/mobskills/quadrastrike')
describe('Quadrastrike mob skill', function()
    it('allows NM or Dynamis use and applies fourfold slashing with attack multiplier', function()
        local strike = require('scripts/actions/mobskills/quadrastrike')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local isNM, inDynamis, params, damage = false, false, nil, nil
        local mob = {
            isMobType = function(_, t) return isNM and t == xi.mobType.NOTORIOUS end,
            isInDynamis = function() return inDynamis end,
            getWeaponDmg = function() return 77 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(strike.onMobSkillCheck(target, mob, {}) == 1)
        isNM = true; assert(strike.onMobSkillCheck(target, mob, {}) == 0)
        isNM, inDynamis = false, true; assert(strike.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(strike.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 4 and params.attackMultiplier[1] == 1.25 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        strike.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
