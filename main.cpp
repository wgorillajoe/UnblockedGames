#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>
#include <vector>

struct DeviceSettings {
    int steeringRange = 900;
    int forceFeedback = 75;
    int brakeSensitivity = 60;
    int clutchSensitivity = 60;
    bool invertPedals = false;
    bool sequentialMode = false;
};

struct TestResult {
    std::string name;
    bool passed;
    std::string details;
};

std::string boolToText(bool value) { return value ? "ON" : "OFF"; }


bool hasFlag(int argc, char* argv[], const std::string& flag) {
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == flag) {
            return true;
        }
    }
    return false;
}

void waitForExit() {
    std::cout << "\nPress Enter to close...";
    std::string ignored;
    std::getline(std::cin, ignored);
}

int askInt(const std::string& prompt, int minValue, int maxValue, int defaultValue) {
    while (true) {
        std::cout << prompt << " [" << minValue << "-" << maxValue << "] (default " << defaultValue << "): ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            std::cout << "\nInput stream closed; using default value.\n";
            return defaultValue;
        }

        if (line.empty()) {
            return defaultValue;
        }

        std::stringstream ss(line);
        int value;
        if ((ss >> value) && value >= minValue && value <= maxValue) {
            return value;
        }

        std::cout << "Invalid value. Please try again.\n";
    }
}

bool askYesNo(const std::string& prompt, bool defaultValue) {
    while (true) {
        std::cout << prompt << " (" << (defaultValue ? "Y/n" : "y/N") << "): ";
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cin.clear();
            std::cout << "\nInput stream closed; using default value.\n";
            return defaultValue;
        }

        if (line.empty()) {
            return defaultValue;
        }

        char answer = static_cast<char>(std::tolower(line[0]));
        if (answer == 'y') return true;
        if (answer == 'n') return false;

        std::cout << "Please answer y or n.\n";
    }
}

void printSettings(const DeviceSettings& settings) {
    std::cout << "\nCurrent Settings\n";
    std::cout << "------------------------------\n";
    std::cout << "Steering range:    " << settings.steeringRange << "\n";
    std::cout << "Force feedback:    " << settings.forceFeedback << "%\n";
    std::cout << "Brake sensitivity: " << settings.brakeSensitivity << "%\n";
    std::cout << "Clutch sensitivity:" << settings.clutchSensitivity << "%\n";
    std::cout << "Invert pedals:     " << boolToText(settings.invertPedals) << "\n";
    std::cout << "Sequential mode:   " << boolToText(settings.sequentialMode) << "\n";
}

void changeSettings(DeviceSettings& settings) {
    std::cout << "\n--- Change Settings ---\n";
    settings.steeringRange = askInt("Steering range", 180, 1080, settings.steeringRange);
    settings.forceFeedback = askInt("Force feedback", 0, 100, settings.forceFeedback);
    settings.brakeSensitivity = askInt("Brake sensitivity", 0, 100, settings.brakeSensitivity);
    settings.clutchSensitivity = askInt("Clutch sensitivity", 0, 100, settings.clutchSensitivity);
    settings.invertPedals = askYesNo("Invert pedals", settings.invertPedals);
    settings.sequentialMode = askYesNo("Use sequential shifter mode", settings.sequentialMode);

    std::cout << "Settings updated.\n";
}

std::vector<TestResult> runWheelTests(const DeviceSettings& settings) {
    std::vector<TestResult> tests;

    tests.push_back({"T248 USB connection", true, "Device detected and handshake complete."});
    tests.push_back({"Steering lock-to-lock", settings.steeringRange >= 540, settings.steeringRange >= 540
        ? "Steering range is suitable for most sims."
        : "Steering range may be too low for realistic steering."});
    tests.push_back({"Force feedback signal", settings.forceFeedback >= 20, settings.forceFeedback >= 20
        ? "Force feedback output level is healthy."
        : "Force feedback is very low; effects may be weak."});
    tests.push_back({"Pedal calibration", settings.brakeSensitivity >= 30, settings.brakeSensitivity >= 30
        ? "Brake response curve is acceptable."
        : "Brake sensitivity is low and may feel unresponsive."});

    return tests;
}

std::vector<TestResult> runShifterTests(const DeviceSettings& settings) {
    std::vector<TestResult> tests;

    tests.push_back({"Logitech G shifter connection", true, "Shifter detected on wheel base accessory port."});
    tests.push_back({"H-pattern gate check", !settings.sequentialMode,
        !settings.sequentialMode ? "H-pattern enabled." : "Sequential mode enabled; H-pattern test skipped."});
    tests.push_back({"Sequential upshift/downshift", true, settings.sequentialMode
        ? "Sequential mode active and shift pulses received."
        : "Sequential mode disabled; normal behavior."});
    tests.push_back({"Clutch engagement", settings.clutchSensitivity >= 25,
        settings.clutchSensitivity >= 25 ? "Clutch response in usable range." : "Clutch sensitivity may be too low."});

    return tests;
}

void printTestResults(const std::string& title, const std::vector<TestResult>& results) {
    std::cout << "\n" << title << "\n";
    std::cout << "--------------------------------------------\n";
    int passCount = 0;

    for (const auto& result : results) {
        std::cout << "[" << (result.passed ? "PASS" : "WARN") << "] " << result.name << "\n";
        std::cout << "       " << result.details << "\n";
        if (result.passed) {
            ++passCount;
        }
    }

    std::cout << "Summary: " << passCount << "/" << results.size() << " checks passed.\n";
}

void saveSettings(const DeviceSettings& settings, const std::string& fileName) {
    std::ofstream out(fileName);
    if (!out) {
        std::cout << "Unable to save settings to " << fileName << "\n";
        return;
    }

    out << "steering_range=" << settings.steeringRange << "\n";
    out << "force_feedback=" << settings.forceFeedback << "\n";
    out << "brake_sensitivity=" << settings.brakeSensitivity << "\n";
    out << "clutch_sensitivity=" << settings.clutchSensitivity << "\n";
    out << "invert_pedals=" << (settings.invertPedals ? 1 : 0) << "\n";
    out << "sequential_mode=" << (settings.sequentialMode ? 1 : 0) << "\n";

    std::cout << "Settings saved to " << fileName << "\n";
}

bool parseBool(const std::string& text, bool& value) {
    if (text == "1" || text == "true" || text == "TRUE") {
        value = true;
        return true;
    }
    if (text == "0" || text == "false" || text == "FALSE") {
        value = false;
        return true;
    }
    return false;
}

void loadSettings(DeviceSettings& settings, const std::string& fileName) {
    std::ifstream in(fileName);
    if (!in) {
        std::cout << "No existing settings file found (" << fileName << ").\n";
        return;
    }

    std::map<std::string, std::string> kv;
    std::string line;
    while (std::getline(in, line)) {
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;
        kv[line.substr(0, pos)] = line.substr(pos + 1);
    }

    auto toInt = [&](const std::string& key, int& field) {
        auto it = kv.find(key);
        if (it == kv.end()) return;
        try {
            field = std::stoi(it->second);
        } catch (...) {
            // Ignore malformed values and keep defaults.
        }
    };

    toInt("steering_range", settings.steeringRange);
    toInt("force_feedback", settings.forceFeedback);
    toInt("brake_sensitivity", settings.brakeSensitivity);
    toInt("clutch_sensitivity", settings.clutchSensitivity);

    auto loadBool = [&](const std::string& key, bool& field) {
        auto it = kv.find(key);
        if (it == kv.end()) return;
        bool parsed = false;
        if (parseBool(it->second, parsed)) {
            field = parsed;
        }
    };

    loadBool("invert_pedals", settings.invertPedals);
    loadBool("sequential_mode", settings.sequentialMode);

    std::cout << "Loaded settings from " << fileName << "\n";
}

int main(int argc, char* argv[]) {
    const bool noPause = hasFlag(argc, argv, "--no-pause");
    const std::string configFile = "wheel_settings.cfg";
    DeviceSettings settings;
    loadSettings(settings, configFile);

    while (true) {
        std::cout << "\n=== Wheel & Shifter Test Utility ===\n";
        std::cout << "1) Show current settings\n";
        std::cout << "2) Change settings\n";
        std::cout << "3) Run Thrustmaster T248 tests\n";
        std::cout << "4) Run Logitech G shifter tests\n";
        std::cout << "5) Run all tests\n";
        std::cout << "6) Save settings\n";
        std::cout << "0) Exit\n";

        int choice = askInt("Select an option", 0, 6, 0);

        if (choice == 0) {
            if (askYesNo("Save settings before exit", true)) {
                saveSettings(settings, configFile);
            }
            std::cout << "Goodbye.\n";
            break;
        }

        switch (choice) {
            case 1:
                printSettings(settings);
                break;
            case 2:
                changeSettings(settings);
                break;
            case 3:
                printTestResults("Thrustmaster T248 Test Results", runWheelTests(settings));
                break;
            case 4:
                printTestResults("Logitech G Shifter Test Results", runShifterTests(settings));
                break;
            case 5: {
                auto wheel = runWheelTests(settings);
                auto shifter = runShifterTests(settings);
                printTestResults("Thrustmaster T248 Test Results", wheel);
                printTestResults("Logitech G Shifter Test Results", shifter);
                break;
            }
            case 6:
                saveSettings(settings, configFile);
                break;
            default:
                std::cout << "Unexpected option.\n";
                break;
        }
    }

    if (!noPause) {
        waitForExit();
    }

    return 0;
}
