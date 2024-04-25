#ifndef ASYNC_SERVER_EXAMPLE_SORTHANDLER_H
#define ASYNC_SERVER_EXAMPLE_SORTHANDLER_H

#include <Server.h>
#include <thread-pool/Pool.h>
#include "../algo/AsyncMergeSort.h"
#include <fstream>

class SortHandler: public Server::AbstractHandler {
public:
    SortHandler(const Server::Socket& _client, const AsyncMergeSort& sorter): client(_client), sorter(sorter) {
        reader = client.read_bytes_nonblock(32, buff.data(), buff.size(), [this](size_t read) {
            filename += std::string(buff.data(), read);
        });
    }

    bool operator()() final {
        if (state == STATE::FINISHED) {
            return false;
        }
        if (state == STATE::TOKEN) {
                bool need_continue = reader();
                if (need_continue) return true;
                state = STATE::SORT;
                std::fstream file(std::filesystem::path(output) / filename, std::ios::in);
                if (!file.is_open()) {
                    state = STATE::FINISHED;
                    response = "ERROR|File not found";
                    return false;
                }
                std::vector<int> data;
                int v;
                while (file >> v) {
                    data.push_back(v);
                }
                sorter(data.begin(), data.end());
                state = STATE::FINISHED;
                for (auto &i: data) {
                    response += std::to_string(i) + ' ';
                }
                if (!response.empty())
                    response.pop_back();
                return false;
            }
        return true;
    }

    std::string get_response() final {
        return response;
    }

private:
    static constexpr const char* output = "../uploads";
    const Server::Socket& client;
    AsyncMergeSort sorter;
    std::string response;
    std::array<char, 32> buff{};
    Server::Socket::predicate_t reader = nullptr;
    std::string filename;

    enum class STATE {
        TOKEN,
        SORT,
        FINISHED
    } state = STATE::TOKEN;
};


#endif //ASYNC_SERVER_EXAMPLE_SORTHANDLER_H
