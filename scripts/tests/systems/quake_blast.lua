require('scripts/actions/mobskills/quake_blast')
describe('Quake Blast mob skill', function()
    it('uses earth magical plan and unequips MAIN through BACK after processing', function()
        local blast = require('scripts/actions/mobskills/quake_blast')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, slots = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            unequipItem = function(_, slot) slots[#slots + 1] = slot end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.processDamage = function() return false end
        assert(blast.onMobSkillCheck(target, mob, {}) == 0 and blast.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.element == xi.element.EARTH and params.dStatMultiplier == 1)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and #slots == 0)
        xi.mobskills.processDamage = function() return true end
        blast.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and slots[1] == xi.slot.MAIN and slots[#slots] == xi.slot.BACK and #slots == 16)
    end)
end)
