#include "core/Engine.hpp"

using vt::core::Engine;

int main(int argc, char *argv[]) {
	VT_UNUSED(argc);
	VT_UNUSED(argv);

	if (!Engine::init()) {
		return EXIT_FAILURE;
	}

	auto& engine = Engine::get();
	engine.run_loop();

	return EXIT_SUCCESS;
}
