require('scripts/actions/mobskills/thunder_meeble_warble')
describe('Thunder Meeble Warble mob skill', function()
    it('uses fTP 18 thunder plan and applies shock then stun after processing', function()
        local skill = require('scripts/actions/mobskills/thunder_meeble_warble')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusCalls = nil, nil, {}
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.THUNDER }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusCalls[#statusCalls + 1] = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 18.00 and params.element == xi.element.THUNDER and #statusCalls == 0)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100)
        assert(statusCalls[1][3] == xi.effect.SHOCK and statusCalls[1][4] == 50 and statusCalls[1][5] == 3 and statusCalls[1][6] == 60)
        assert(statusCalls[2][3] == xi.effect.STUN and statusCalls[2][4] == 30 and statusCalls[2][6] == 15)
    end)
end)
