require('scripts/actions/mobskills/megaflare')

describe('Megaflare mob skill', function()
    it('uses its Fire plan and reduces only positive non-primary damage by 300 before processing', function()
        local flare = require('scripts/actions/mobskills/megaflare')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damages = nil, {}
        local raw = 500
        local mob = { getMainLvl = function() return 75 end }
        local skill = { getPrimaryTargetID = function() return 1 end }
        local target = {
            getID = function() return 2 end,
            takeDamage = function(_, ...) damages[#damages + 1] = { ... } end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = raw, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end

        assert(flare.onMobSkillCheck(target, mob, skill) == 0 and flare.onMobWeaponSkill(mob, target, skill, {}) == 200)
        assert(params.baseDamage == 75 and params.fTP[1] == 10 and params.fTP[2] == 10 and params.fTP[3] == 10)
        assert(params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 1.5 and #damages == 0)
        target.getID = function() return 1 end
        assert(flare.onMobWeaponSkill(mob, target, skill, {}) == 500)
        target.getID = function() return 2 end
        raw = 0
        assert(flare.onMobWeaponSkill(mob, target, skill, {}) == 0)
        raw = 200
        xi.mobskills.processDamage = function() return true end
        flare.onMobWeaponSkill(mob, target, skill, {})

        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(#damages == 1 and damages[1][1] == 0 and damages[1][2] == mob and damages[1][3] == xi.attackType.MAGICAL and damages[1][4] == xi.damageType.FIRE)
    end)
end)
