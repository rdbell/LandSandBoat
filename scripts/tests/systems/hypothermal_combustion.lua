require('scripts/actions/mobskills/hypothermal_combustion')
describe('Hypothermal Combustion mob skill', function()
    it('rejects NM and self-destructs with Ice breath damage', function()
        local skill = require('scripts/actions/mobskills/hypothermal_combustion')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, hp = nil, nil, 500
        local origRandom = math.random
        math.random = function(a, b)
            if a == 0.7 and b == 1.1 then return 1.0 end
            return origRandom(a, b)
        end
        local mob = {
            isMobType = function(_, t) return t == xi.mobType.NOTORIOUS end,
            getHP = function() return 500 end,
            setHP = function(_, v) hp = v end,
        }
        assert(skill.onMobSkillCheck({}, mob, {}) == 1)
        mob.isMobType = function() return false end
        assert(skill.onMobSkillCheck({}, mob, {}) == 0)
        local target = {
            getMaxHP = function() return 1000 end,
            takeDamage = function(_, v) damage = v end,
        }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=400, attackType=xi.attackType.BREATH, damageType=xi.damageType.ICE } end
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 400)
        assert(params.baseDamage == 500 and params.element == xi.element.ICE and params.skipMagicBonusDiff)
        assert(damage == 400)
        skill.onMobSkillFinalize(mob, {})
        assert(hp == 0)
        math.random = origRandom
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
    end)
end)
