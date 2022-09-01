#include "main.hpp"

std::shared_ptr<executor::module::executor_module_t> executor::module::global_module;

LONG WINAPI VEH(EXCEPTION_POINTERS* ep)
{
	std::cout << std::hex << ep->ExceptionRecord->ExceptionAddress << std::endl;

	return EXCEPTION_CONTINUE_SEARCH;
}

int executor::executor_main()
{
	module::global_module = std::make_shared<module::executor_module_t>();

	module::global_module->bypass_checks();

	module::global_module->initialize_scheduler();

	module::global_module->push_custom_functions();

    // UI Listener
    std::string WholeScript = "";
    HANDLE hPipe;
    char buffer[999999];
    DWORD dwRead;
    hPipe = CreateNamedPipe(TEXT("\\\\.\\pipe\\FGFDGDFG53434yhDHJASJFG"),
        PIPE_ACCESS_DUPLEX | PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
        PIPE_WAIT,
        1,
        999999,
        999999,
        NMPWAIT_USE_DEFAULT_WAIT,
        NULL);
    while (hPipe != INVALID_HANDLE_VALUE)
    {
        if (ConnectNamedPipe(hPipe, NULL) != FALSE)
        {
            while (ReadFile(hPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)
            {
                buffer[dwRead] = '\0';
                try {
                    try {
                        WholeScript = WholeScript + buffer;
                    }
                    catch (...) {
                    }
                }
                catch (std::exception e) {

                }
                catch (...) {

                }
            }

            if (!WholeScript.empty()) {
                module::global_module->execute(WholeScript);
            }

            WholeScript = "";
        }
        DisconnectNamedPipe(hPipe);
    }

    return 0;
}