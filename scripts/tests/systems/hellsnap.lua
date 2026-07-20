describe('Hellsnap mob skill',function()
 it('uses normal and NM Stun durations, forwards the message, and returns Stun',function()
  local snap=require('scripts/actions/mobskills/hellsnap');local effect=xi.mobskills.mobStatusEffectMove;local call,message=nil,nil;local nm=false;local mob={isNM=function()return nm end};local target={};local skill={setMsg=function(_,value)message=value end};xi.mobskills.mobStatusEffectMove=function(...)call={...};return 456 end
  assert(snap.onMobSkillCheck(target,mob,skill)==0 and snap.onMobWeaponSkill(mob,target,skill,{})==xi.effect.STUN);assert(call[1]==mob and call[2]==target and call[3]==xi.effect.STUN and call[4]==1 and call[5]==0 and call[6]==4 and message==456);nm=true;snap.onMobWeaponSkill(mob,target,skill,{});xi.mobskills.mobStatusEffectMove=effect;assert(call[6]==10)
 end)
end)
