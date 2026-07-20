require('scripts/actions/mobskills/bomb_toss_suicide')
describe('Bomb Toss Suicide mob skill', function()
    it('rejects NM/Dynamis and uses Fire magical plan with HP/3 base', function()
        local skill = require('scripts/actions/mobskills/bomb_toss_suicide')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage, hp = nil, nil, 300
        local mob = {
            isMobType = function(_, t) return t == xi.mobType.NOTORIOUS end,
            isInDynamis = function() return false end,
            setHP = function(_, v) hp = v end,
        }
        assert(skill.onMobSkillCheck({}, mob, {}) == 1)
        mob.isMobType = function() return false end
        mob.isInDynamis = function() return true end
        assert(skill.onMobSkillCheck({}, mob, {}) == 1)
        mob.isInDynamis = function() return false end
        assert(skill.onMobSkillCheck({}, mob, {}) == 0)
        local skillObj = { getMobHP = function() return 300 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 50, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, skillObj, {}) == 50)
        assert(params.baseDamage == 100 and params.element == xi.element.FIRE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, skillObj, {}) == 50)
        assert(damage[1] == 50)
        skill.onMobSkillFinalize(mob, skillObj)
        assert(hp == 0)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
