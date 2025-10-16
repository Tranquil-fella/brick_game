#ifndef SIMPLE_FILE_STORAGE_H
#define SIMPLE_FILE_STORAGE_H

#include <filesystem>
#include <fstream>
#include <string>

namespace s21 {
struct SimpleFileStorage {
 private:
  std::filesystem::path GetSaveFilePath() {
    std::filesystem::path saveDir = GetSaveDirectory();
    // Create directory if it doesn't exist
    std::filesystem::create_directories(saveDir);
    return saveDir / "snake.score";
  }

  std::filesystem::path GetSaveDirectory() {
    const char* home = std::getenv("HOME");
    if (home) {
      // Default XDG path: ~/.local/share/
      return std::filesystem::path(home) / ".local" / "share" / "BrickGame" /
             "saves";
    }

// Fallback for Windows or other systems
#ifdef _WIN32
    const char* appdata = std::getenv("APPDATA");
    if (appdata) {
      return std::filesystem::path(appdata) / "BrickGame" / "saves";
    }
#endif

    // Ultimate fallback - current directory
    return std::filesystem::path("./../saves");
  }

 public:
  int ReadHighscore() {
    std::filesystem::path filePath = GetSaveFilePath();

    if (!std::filesystem::exists(filePath)) {
      return 0;
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
      return 0;
    }

    int highscore = 0;
    file >> highscore;
    file.close();

    return highscore;
  }

  void WriteHighscore(int highscore) {
    std::filesystem::path filePath = GetSaveFilePath();

    std::ofstream file(filePath);
    if (!file.is_open()) {
      return;
    }
    file << highscore;
    file.close();
  }
};
}  // namespace s21
#endif