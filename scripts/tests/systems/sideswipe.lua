require('scripts/actions/mobskills/sideswipe')
describe('Sideswipe mob skill', function()
    it('uses slashing physical plan with crit and resets enmity after processing', function()
        local swipe = require('scripts/actions/mobskills/sideswipe')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, reset = nil, nil, nil
        local mob = {
            getWeaponDmg = function() return 35 end,
            resetEnmity = function(_, t) reset = t end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 40, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(swipe.onMobSkillCheck(target, mob, {}) == 0 and swipe.onMobWeaponSkill(mob, target, {}, {}) == 40)
        assert(params.canCrit and damage == nil and reset == nil)
        xi.mobskills.processDamage = function() return true end
        swipe.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 40 and reset == target)
    end)
end)
