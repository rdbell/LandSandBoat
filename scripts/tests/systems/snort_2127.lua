require('scripts/actions/mobskills/snort_2127')
describe('Snort 2127 mob skill', function()
    it('uses wind magical HIT_DMG plan and lowers enmity', function()
        local skill = require('scripts/actions/mobskills/snort_2127')
        local params, enmity = nil, nil
        local origM, origD = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getMainLvl = function() return 50 end, lowerEnmity = function(_, t, p) enmity = p end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.element == xi.element.WIND and params.primaryMessage == xi.msg.basic.HIT_DMG and enmity == 25)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = origM, origD
    end)
end)
