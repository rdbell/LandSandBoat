describe('Hypnotic Sway mob skill',function()
 it('uses TP-scaled Amnesia duration, forwards the gaze message, and returns Amnesia',function()
  local sway=require('scripts/actions/mobskills/hypnotic_sway');local duration,gaze=xi.mobskills.calculateDuration,xi.mobskills.mobGazeMove;local call,message=nil,nil;local mob={};local target={};local skill={getTP=function()return 1500 end,setMsg=function(_,value)message=value end};xi.mobskills.calculateDuration=function(tp,min,max)assert(tp==1500 and min==30 and max==60);return 45 end;xi.mobskills.mobGazeMove=function(...)call={...};return 456 end
  assert(sway.onMobSkillCheck(target,mob,skill)==0 and sway.onMobWeaponSkill(mob,target,skill,{})==xi.effect.AMNESIA);xi.mobskills.calculateDuration,xi.mobskills.mobGazeMove=duration,gaze;assert(call[1]==mob and call[2]==target and call[3]==xi.effect.AMNESIA and call[4]==1 and call[5]==0 and call[6]==45 and message==456)
 end)
end)
