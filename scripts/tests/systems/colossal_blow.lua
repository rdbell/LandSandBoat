require('scripts/actions/mobskills/colossal_blow')
describe('Colossal Blow mob skill', function()
    it('uses target-HP base physical plan and resets enmity when processed', function()
        local skill = require('scripts/actions/mobskills/colossal_blow')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage, reset = nil, nil, false
        local mob = { resetEnmity = function() reset = true end }
        local target = {
            getHP = function() return 1000 end,
            takeDamage = function(_, v) damage = v end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 950, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 950)
        assert(params.baseDamage == 1000 and params.fTP[1] == 0.95 and params.skipPDIF and params.skipFSTR)
        assert(not reset)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 950)
        assert(damage == 950 and reset)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
