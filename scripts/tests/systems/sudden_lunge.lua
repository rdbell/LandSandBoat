require('scripts/actions/mobskills/sudden_lunge')
describe('Sudden Lunge mob skill', function()
    it('uses blunt ignore-shadows plan, stun on process, and finalize HP cost', function()
        local skill = require('scripts/actions/mobskills/sudden_lunge')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local random = math.random
        local params, damage, statusParams, setHP = nil, nil, nil, nil
        local mob = {
            getWeaponDmg = function() return 80 end,
            getHP = function() return 1000 end,
            setHP = function(_, v) setHP = v end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 1.5 and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        assert(damage[1] == 100 and statusParams[3] == xi.effect.STUN and statusParams[6] == 4)
        math.random = function(a, b) assert(a == 5 and b == 15); return 10 end
        skill.onMobSkillFinalize(mob, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        math.random = random
        assert(setHP == 900)
    end)
end)
