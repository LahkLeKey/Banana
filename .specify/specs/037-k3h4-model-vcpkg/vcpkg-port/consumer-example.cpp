// consumer-example.cpp
// Example C++ code using banana-k3h4-model (model-only port)

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

int main() {
    // After CMake's find_package(banana-k3h4-model REQUIRED),
    // the target banana::k3h4::model is available with INTERFACE_MODEL_PAYLOAD_LOCATION set.

    // Option 1: Access payload location from CMake (hardcoded example)
    // In real code, this comes from a CMake-generated header or CMake cache variable
    const char* payload_dir = std::getenv("K3H4_MODEL_PAYLOAD_PATH");
    if (!payload_dir) {
        // Fallback: try to find it via manifest
        payload_dir = "/usr/local/share/banana-k3h4-model/2026.06.14";
    }

    fs::path model_root(payload_dir);
    
    // Load model metadata
    fs::path metadata_file = model_root / "MODEL_METADATA.json";
    if (!fs::exists(metadata_file)) {
        std::cerr << "Error: MODEL_METADATA.json not found at " << metadata_file << std::endl;
        return 1;
    }

    std::ifstream metadata_stream(metadata_file);
    std::string metadata_text((std::istreambuf_iterator<char>(metadata_stream)), std::istreambuf_iterator<char>());

    auto read_field = [&](const std::string& field_name) -> std::string {
        const std::string needle = "\"" + field_name + "\"";
        const std::size_t key_pos = metadata_text.find(needle);
        if (key_pos == std::string::npos) {
            return "unknown";
        }

        const std::size_t colon_pos = metadata_text.find(':', key_pos + needle.size());
        if (colon_pos == std::string::npos) {
            return "unknown";
        }

        const std::size_t quote_start = metadata_text.find('"', colon_pos + 1);
        if (quote_start == std::string::npos) {
            return "unknown";
        }

        const std::size_t quote_end = metadata_text.find('"', quote_start + 1);
        if (quote_end == std::string::npos) {
            return "unknown";
        }

        return metadata_text.substr(quote_start + 1, quote_end - quote_start - 1);
    };

    std::cout << "K3H4 Model Metadata:" << std::endl;
    std::cout << "  Bundle version: " << read_field("bundle_version") << std::endl;
    std::cout << "  Native ABI version: " << read_field("native_abi_version") << std::endl;
    
    // Load model data files
    fs::path data_dir = model_root / "data";
    if (!fs::exists(data_dir)) {
        std::cerr << "Error: data directory not found at " << data_dir << std::endl;
        return 1;
    }

    std::cout << "\nModel data files:" << std::endl;
    for (const auto& entry : fs::directory_iterator(data_dir)) {
        if (fs::is_regular_file(entry)) {
            std::cout << "  - " << entry.path().filename().string() 
                      << " (" << fs::file_size(entry) << " bytes)" << std::endl;
        }
    }

    // Optional: Load contracts directory if it exists
    fs::path contracts_dir = model_root / "contracts";
    if (fs::exists(contracts_dir)) {
        std::cout << "\nABI contract documentation available at: " << contracts_dir << std::endl;
    }

    // If using native ABI (optional dependency), pair this model with banana-native-abi.
    // The ABI package exposes banana::native::library and installs the shared library
    // plus public headers separately from the model payload.

    std::cout << "\nModel is ready for use." << std::endl;
    return 0;
}
