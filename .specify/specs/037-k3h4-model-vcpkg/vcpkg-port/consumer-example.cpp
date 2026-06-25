// consumer-example.cpp
// Example C++ code using banana-k3h4-model (model-only port)

#include <iostream>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

int main() {
    // After CMake's find_package(banana-k3h4-model REQUIRED),
    // the target banana::k3h4::model is available with INTERFACE_PAYLOAD_LOCATION set.

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
    json metadata = json::parse(metadata_stream);
    
    std::cout << "K3H4 Model Metadata:" << std::endl;
    std::cout << "  Version: " << metadata["version"] << std::endl;
    std::cout << "  Schema: " << metadata.value("schema_version", "unknown") << std::endl;
    
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

    // If using native ABI (optional dependency):
    // 1. Check if banana-native-abi is available
    // 2. Load libbanana_native.so
    // 3. Initialize clustering query interface
    // 4. Use metadata's native_abi_version to validate compatibility

    std::cout << "\nModel is ready for use." << std::endl;
    return 0;
}
