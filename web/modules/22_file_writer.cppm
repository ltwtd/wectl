export module wectl.web.writer;
import <string>;
import <string_view>;
import <fstream>;
import <iostream>;

export namespace wectl::web {
    bool write_output(std::string_view filename, const std::string& content) {
        std::ofstream out(filename.data(), std::ios::binary);
        if (!out) {
            std::cerr << "[ERROR] Failed to open output file: " << filename << "\n";
            return false;
        }
        out.write(content.data(), static_cast<std::streamsize>(content.size()));
        std::cout << "[OK] Built " << filename << " (" << content.size() << " bytes)\n";
        return true;
    }
}
