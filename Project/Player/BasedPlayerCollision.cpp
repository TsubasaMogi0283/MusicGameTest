#include "BasePlayerCollision.h"

void BasePlayerCollision::OnCollision(){
	isTouch_ = true;
}

void BasePlayerCollision::OffCollision(){
	isTouch_ = false;
}
