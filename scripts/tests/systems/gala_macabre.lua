describe('Gala Macabre mob skill',function()
 it('misses non-PCs and charms PCs only when Charm I lands',function()
  local gala=require('scripts/actions/mobskills/gala_macabre');local status=xi.mobskills.mobStatusEffectMove;local calls,charmed,message=0,false,nil;local mob={charm=function(_,target)charmed=target end};local skill={setMsg=function(_,v)message=v end};local npc={isPC=function()return false end};local pc={isPC=function()return true end}
  xi.mobskills.mobStatusEffectMove=function()calls=calls+1;return xi.msg.basic.SKILL_ENFEEB_IS end
  assert(gala.onMobSkillCheck(npc,mob,skill)==0 and gala.onMobWeaponSkill(mob,npc,skill,{})==xi.effect.CHARM_I and message==xi.msg.basic.SKILL_MISS and calls==0 and not charmed)
  assert(gala.onMobWeaponSkill(mob,pc,skill,{})==xi.effect.CHARM_I and calls==1 and charmed==pc and message==xi.msg.basic.SKILL_ENFEEB_IS)
  charmed=false;xi.mobskills.mobStatusEffectMove=function(_,_,effect,power,tick,duration)calls=calls+1;assert(effect==xi.effect.CHARM_I and power==0 and tick==3 and duration==60);return xi.msg.basic.SKILL_MISS end
  assert(gala.onMobWeaponSkill(mob,pc,skill,{})==xi.effect.CHARM_I and calls==2 and not charmed and message==xi.msg.basic.SKILL_MISS)
  xi.mobskills.mobStatusEffectMove=status
 end)
end)
