require('scripts/actions/mobskills/thunderbolt_behemoth')
describe('Thunderbolt Behemoth mob skill', function()
    it('uses fTP 0.6 thunder plan and random stun after processing', function()
        local skill = require('scripts/actions/mobskills/thunderbolt_behemoth')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local random = math.random
        local params, damage, statusParams = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.THUNDER }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 0.6 and statusParams == nil)
        math.random = function(a, b) assert(a == 8 and b == 14); return 10 end
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        math.random = random
        assert(damage[1] == 100 and statusParams[3] == xi.effect.STUN and statusParams[6] == 10)
    end)
end)
