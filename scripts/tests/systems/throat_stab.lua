require('scripts/actions/mobskills/throat_stab')
describe('Throat Stab mob skill', function()
    it('uses target-HP base skip-defense plan and resets enmity after process', function()
        local skill = require('scripts/actions/mobskills/throat_stab')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, reset = nil, nil, false
        local mob = { resetEnmity = function() reset = true end }
        local target = {
            getHP = function() return 1000 end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 950, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 950)
        assert(params.baseDamage == 1000 and params.fTP[1] == 0.95 and params.skipFSTR and params.skipPDIF and params.skipParry and reset == false)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 950 and reset == true)
    end)
end)
