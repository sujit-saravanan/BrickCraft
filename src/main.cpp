#include <iostream>
#include "context_manager.h"

int main(int argc, char *argv[]) {
        DapperCraft::EngineContext engine_context("test", {800, 800});
        engine_context.run();
	return 0;
}