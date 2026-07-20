require('scripts/actions/mobskills/torrential_torment')
describe('Torrential Torment mob skill', function()
    it('uses water magical plan and unequips MAIN..BACK', function()
        local skill = require('scripts/actions/mobskills/torrential_torment')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, unequipped = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            takeDamage = function(_, ...) damage = { ... } end,
            unequipItem = function(_, slot) unequipped[#unequipped + 1] = slot end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WATER }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 52 and params.fTP[1] == 2.00 and params.element == xi.element.WATER and damage == nil)
        assert(unequipped[1] == xi.slot.MAIN and unequipped[#unequipped] == xi.slot.BACK)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
    end)
end)
