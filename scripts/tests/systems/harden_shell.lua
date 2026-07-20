describe('Harden Shell mob skill',function()
 it('rejects an existing Defense Boost and uses NM power with random duration',function()
  local shell=require('scripts/actions/mobskills/harden_shell');local random,buff=math.random,xi.mobskills.mobBuffMove;local active,nm=false,false;local call,message=nil,nil;local mob={hasStatusEffect=function(_,effect)return active and effect==xi.effect.DEFENSE_BOOST end,isNM=function()return nm end};local target={};local skill={setMsg=function(_,value)message=value end};math.random=function(min,max)assert(min==60 and max==180);return 123 end;xi.mobskills.mobBuffMove=function(...)call={...};return 456 end
  assert(shell.onMobSkillCheck(target,mob,skill)==0 and shell.onMobWeaponSkill(mob,target,skill,{})==xi.effect.DEFENSE_BOOST);assert(message==456 and call[1]==target and call[2]==xi.effect.DEFENSE_BOOST and call[3]==33 and call[4]==0 and call[5]==123);nm=true;shell.onMobWeaponSkill(mob,target,skill,{});assert(call[3]==80);active=true;assert(shell.onMobSkillCheck(target,mob,skill)==1);math.random,xi.mobskills.mobBuffMove=random,buff
 end)
end)
