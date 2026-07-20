describe('Hypnosis mob skill',function()
 it('uses TP-scaled Sleep I duration, forwards the gaze message, and returns Sleep I',function()
  local hypnosis=require('scripts/actions/mobskills/hypnosis');local duration,gaze=xi.mobskills.calculateDuration,xi.mobskills.mobGazeMove;local call,message=nil,nil;local mob={getTP=function()return 1500 end};local target={};local skill={setMsg=function(_,value)message=value end};xi.mobskills.calculateDuration=function(tp,min,max)assert(tp==1500 and min==45 and max==90);return 75 end;xi.mobskills.mobGazeMove=function(...)call={...};return 456 end
  assert(hypnosis.onMobSkillCheck(target,mob,skill)==0 and hypnosis.onMobWeaponSkill(mob,target,skill,{})==xi.effect.SLEEP_I);xi.mobskills.calculateDuration,xi.mobskills.mobGazeMove=duration,gaze;assert(call[1]==mob and call[2]==target and call[3]==xi.effect.SLEEP_I and call[4]==1 and call[5]==0 and call[6]==75 and message==456)
 end)
end)
