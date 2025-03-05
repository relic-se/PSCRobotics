
#include "Grabber.hpp"
 Grabber::Grabber(){
   sparki.gripperOpen();
  sparki.servo(SERVO_CENTER);
  reset();
 
  }
  void Grabber::reset(){
    setState(GrabberState:: SEARCH);


  }

bool Grabber::setState(GrabberState state){
 
  if (isState(state)) return false;
  switch(state) {
    case GrabberState::CLOSING:
    sparki.gripperClose();
    break;
  
    case GrabberState::OPENING:
    sparki.gripperOpen();
    break;

    default:
    sparki.gripperStop();
    break;
  }
 
  _state = state;
  
  return true;
}
GrabberState Grabber::getState(){
  return _state;
}
 
  bool Grabber::isState( GrabberState state){
    return state == _state;
}
void Grabber::update(){

  
  distance = sparki.ping();
 if (distance < 2)
 
  {
    setState(GrabberState::CLOSING);
    
    
  }
}
void Grabber :: debug(){
  sparki.print(distance);
  sparki.println(" cm");
  }
