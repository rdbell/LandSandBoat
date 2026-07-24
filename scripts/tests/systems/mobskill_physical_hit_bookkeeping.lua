require('scripts/globals/mobskills')

describe('Mob skill default hit info', function()
    it('records the hit number and zeros every other field', function()
        local h = xi.mobskills.defaultHitInfo(3)
        assert(h.hitNumber == 3)
        assert(not h.hitLanded and not h.hitYaegasumi and not h.hitAnticipated)
        assert(not h.hitParried and not h.hitGuarded and not h.hitAbsorbed and not h.hitBlocked)
        assert(not h.isCritical)
        assert(h.pDif == 0 and h.hitDamage == 0 and h.shadowsConsumed == 0)
        assert(h.missType == nil)
    end)
end)

describe('Mob skill tally hit results', function()
    it('sums landed damage and crits only from landed hits', function()
        local dmg, landed, yae, ant, absHits, shadows, crit = xi.mobskills.tallyHitResults({
            { hitLanded = true,  hitDamage = 10, isCritical = true,  hitYaegasumi = false, hitAnticipated = false, hitAbsorbed = false, shadowsConsumed = 0 },
            { hitLanded = false, hitDamage = 99, isCritical = true,  hitYaegasumi = false, hitAnticipated = false, hitAbsorbed = false, shadowsConsumed = 0 },
            { hitLanded = true,  hitDamage = 5,  isCritical = false, hitYaegasumi = false, hitAnticipated = false, hitAbsorbed = false, shadowsConsumed = 0 },
        })
        assert(dmg == 15 and landed == 2 and crit)
    end)

    it('ORs yaegasumi and anticipate across hits', function()
        local _, _, yae, ant = xi.mobskills.tallyHitResults({
            { hitLanded = false, hitDamage = 0, isCritical = false, hitYaegasumi = true,  hitAnticipated = false, hitAbsorbed = false, shadowsConsumed = 0 },
            { hitLanded = false, hitDamage = 0, isCritical = false, hitYaegasumi = false, hitAnticipated = true,  hitAbsorbed = false, shadowsConsumed = 0 },
        })
        assert(yae and ant)
    end)

    it('counts absorbed hits and sums shadows consumed', function()
        local _, _, _, _, absHits, shadows = xi.mobskills.tallyHitResults({
            { hitLanded = false, hitDamage = 0, isCritical = false, hitYaegasumi = false, hitAnticipated = false, hitAbsorbed = true, shadowsConsumed = 2 },
            { hitLanded = false, hitDamage = 0, isCritical = false, hitYaegasumi = false, hitAnticipated = false, hitAbsorbed = true, shadowsConsumed = 1 },
            { hitLanded = false, hitDamage = 0, isCritical = false, hitYaegasumi = false, hitAnticipated = false, hitAbsorbed = false, shadowsConsumed = 9 },
        })
        assert(absHits == 2 and shadows == 3)
    end)

    it('returns zeros for an empty hit list', function()
        local dmg, landed, yae, ant, absHits, shadows, crit = xi.mobskills.tallyHitResults({})
        assert(dmg == 0 and landed == 0 and not yae and not ant and absHits == 0 and shadows == 0 and not crit)
    end)
end)

describe('Mob skill physical param normalize', function()
    it('fills every field from defaults when skillParams is empty', function()
        local p = xi.mobskills.normalizePhysicalSkillParams({})
        assert(p.numHits == 1)
        assert(p.fTP[1] == 1 and p.fTP[2] == 1 and p.fTP[3] == 1)
        assert(p.attackType == xi.attackType.PHYSICAL)
        assert(p.damageType == xi.damageType.SLASHING)
        assert(p.hybridAttackType == xi.attackType.MAGICAL)
        assert(p.hybridDamageType == xi.damageType.ELEMENTAL)
        assert(p.hybridSkillElement == xi.element.NONE)
        assert(p.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(p.primaryMessage == xi.msg.basic.DAMAGE)
        assert(p.baseDamage == nil)
        assert(not p.canCrit and not p.guaranteedFirstHit)
    end)

    it('keeps explicit overrides including false and zero', function()
        local p = xi.mobskills.normalizePhysicalSkillParams({
            numHits            = 3,
            canCrit            = true,
            guaranteedFirstHit = false, -- explicit false still wins over default
            fTPBonus           = 0,
            attackType         = xi.attackType.RANGED,
            baseDamage         = 42,
            fTP                = { 1.5, 2.0, 2.5 },
        })
        assert(p.numHits == 3 and p.canCrit)
        assert(not p.guaranteedFirstHit)
        assert(p.fTPBonus == 0)
        assert(p.attackType == xi.attackType.RANGED)
        assert(p.baseDamage == 42)
        assert(p.fTP[1] == 1.5 and p.fTP[2] == 2.0 and p.fTP[3] == 2.5)
        -- Unset fields still default.
        assert(p.damageType == xi.damageType.SLASHING)
    end)

    it('always takes baseDamage from skillParams even when nil', function()
        local p = xi.mobskills.normalizePhysicalSkillParams({ numHits = 2, baseDamage = nil })
        assert(p.baseDamage == nil)
        assert(p.numHits == 2)
    end)
end)
