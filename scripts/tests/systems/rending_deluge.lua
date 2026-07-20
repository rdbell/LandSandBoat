require('scripts/actions/mobskills/rending_deluge')
describe('Rending Deluge mob skill', function()
    it('uses water magical plan and dispels one beneficial after processing', function()
        local deluge = require('scripts/actions/mobskills/rending_deluge')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, dispelled = nil, nil, false
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            dispelStatusEffect = function() dispelled = true end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WATER }
        end
        xi.mobskills.processDamage = function() return false end
        assert(deluge.onMobSkillCheck(target, mob, {}) == 0 and deluge.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 1 and damage == nil and not dispelled)
        xi.mobskills.processDamage = function() return true end
        deluge.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and dispelled)
    end)
end)
