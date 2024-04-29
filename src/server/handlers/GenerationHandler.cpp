#include <server/handlers/GenerationHandler.h>

namespace Server::Handlers {
GenerationHandler::GenerationHandler(const Server::ClientSocket& _client, Files::FileManager& _fm,
                                     Multithreading::ThreadPool& _tp)
    : client(_client), tp(_tp), fm(_fm) {
    input_data.resize(32);
}

Response GenerationHandler::handle() {
    client.read_bytes(input_data.data(), input_data.size());
    if (!fm.exists(input_data)) {
        return not_found_response();
    }
    std::future<std::string> image_file = tp.add_task([this]() {
        auto in = fm.open(input_data);
        int num_vertex, num_edges;
        in >> num_vertex >> num_edges;
        if (num_vertex <= 0 || num_edges < 0) {
            throw Exceptions::InvalidInputException(
                "Неправильно указано количество вершин или количество ребер.");
        }
        if (num_vertex > 200) {
            throw Exceptions::InvalidInputException(
                "Слишком много вершин в графе. Максимальное количество вершин 200.");
        }
        if (num_edges > 200 * 199 / 2) {
            throw Exceptions::InvalidInputException("Так много ребер быть не может:)");
        }
        std::set<std::pair<int, int>> used_edges;
        for (int i = 0; i < num_edges; ++i) {
            int u, v;
            in >> u >> v;
            if (in.fail()) {
                throw Exceptions::InvalidInputException("Указаны не все ребра");
            }
            if (used_edges.find(std::make_pair(std::min(u, v), std::max(u, v))) !=
                used_edges.end()) {
                throw Exceptions::InvalidInputException("В графе не может быть кратных ребер");
            }
            if (u == v) {
                throw Exceptions::InvalidInputException("В графе не может быть петель");
            }
            if (u < 0 || v < 0 || u >= num_vertex || v >= num_vertex) {
                throw Exceptions::InvalidInputException(
                    "Неправильно указано ребро: " + std::to_string(u) + " <---> " +
                    std::to_string(v));
            }
            used_edges.insert(std::make_pair(std::min(u, v), std::max(u, v)));
        }
        in.close();
        std::filesystem::path result = fm.create_unique();
        generate_graph(std::filesystem::absolute(fm.get_dir() / input_data),
                       std::filesystem::absolute(result));
        return result.filename().string();
    });

    try {
        std::string result = image_file.get();
        return Response{result};
    } catch (const Exceptions::InvalidInputException& e) {
        return Response{e.what()};
    }
}
}    // namespace Server::Handlers