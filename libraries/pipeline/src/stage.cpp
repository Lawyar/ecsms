#include "stage.h"

ecsms::stage::~stage() {
	_thread.join();
}
