#include "Engine.hpp"

int main(int argc, char *argv[]) {
	VT_UNUSED(argc);
	VT_UNUSED(argv);

	if (!vt::Engine::init()) {
		return EXIT_FAILURE;
	}

	auto& engine = vt::Engine::get();
	engine.run_loop();

	return EXIT_SUCCESS;
}
