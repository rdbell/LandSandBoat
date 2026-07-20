require('scripts/actions/mobskills/omega_javelin')

describe('Omega Javelin mob skill', function()
    it('uses its Slashing plan, petrifies and resets enmity only after processing', function()
        local javelin = require('scripts/actions/mobskills/omega_javelin')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, petrify, reset = nil, nil, nil, false
        local mob = {
            getWeaponDmg = function() return 77 end,
            resetEnmity = function(_, t) reset = t ~= nil end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) petrify = { ... } end
        assert(javelin.onMobSkillCheck(target, mob, {}) == 0 and javelin.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil and petrify == nil and not reset)
        xi.mobskills.processDamage = function() return true end
        javelin.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123 and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(petrify[3] == xi.effect.PETRIFICATION and petrify[4] == 1 and petrify[5] == 0 and petrify[6] == 45)
        assert(reset)
    end)
end)
