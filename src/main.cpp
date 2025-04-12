#include "log.hpp"
#include "utils.hpp"

int main(int argc, char *argv[]) {
	VT_UNUSED(argc);
	VT_UNUSED(argv);

	vt::log::debug("HI! {}", 555);
}
