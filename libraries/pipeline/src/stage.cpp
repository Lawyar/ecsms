#include "stage.h"

ecsms::stage::~stage() {
	shutdown();
	thread_.join();
}

void ecsms::stage::shutdown() {
	shutdown_signaled_ = true;
}
