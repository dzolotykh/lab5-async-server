#include <server/handlers/AmogusHandler.h>

Server::Response Server::Handlers::Sus::AmogusHandler::handle() {
    client.send_bytes(SUSSY_BAKA);
    client.send_bytes("\n");
    std::ifstream file("amogus.txt");
    if (!file.is_open()) {
        return create_response("no amogus.txt for animation found:(");
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    client.send_bytes("3..\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    client.send_bytes("2..\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    client.send_bytes("1..\n");
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::vector<std::string> frames;
    std::string buff;
    for (std::string line; std::getline(file, line);) {
        frames.push_back(line);
    }

    while (true) {
        int cnt = 0;
        for (const auto& frame : frames) {
            client.send_bytes(frame);
            client.send_bytes("\n");
            cnt++;
            if (cnt == 26) {
                cnt = 0;
                for (int i = 0; i < 26; i++) {
                    client.send_bytes("\033[A");
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(60));
            }
        }
    }
}