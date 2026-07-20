require('scripts/actions/mobskills/hypothermal_combustion_tzar')
describe('Hypothermal Combustion Tzar mob skill', function()
    it('uses Ice breath plan without self-kill finalize', function()
        local skill = require('scripts/actions/mobskills/hypothermal_combustion_tzar')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local origRandom = math.random
        math.random = function(a, b)
            if a == 0.7 and b == 1.1 then return 0.8 end
            return origRandom(a, b)
        end
        local mob = { getHP = function() return 800 end }
        local target = {
            getMaxHP = function() return 1000 end,
            takeDamage = function(_, v) damage = v end,
        }
        xi.mobskills.mobMagicalMove = function(_,_,_,_,v) params=v; return { damage=700, attackType=xi.attackType.BREATH, damageType=xi.damageType.ICE } end
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 700)
        assert(params.baseDamage == 800 and params.element == xi.element.ICE)
        assert(damage == 700)
        assert(skill.onMobSkillFinalize == nil)
        math.random = origRandom
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
    end)
end)
