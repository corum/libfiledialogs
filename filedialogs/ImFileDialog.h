#pragma once
#include <ctime>
#include <stack>
#include <string>
#include <thread>
#include <vector>
#include <functional>
#include <unordered_map>
#include <algorithm> // std::min, std::max

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "filesystem.h"
#include "filesystem.hpp"

#define IFD_DIALOG_FILE      0
#define IFD_DIALOG_DIRECTORY    1
#define IFD_DIALOG_SAVE      2

#define IFD_DIALOG_WIDTH    (int)((!ngs::fs::environment_get_variable("IMGUI_DIALOG_WIDTH" ).empty()) ? strtoul(ngs::fs::environment_get_variable("IMGUI_DIALOG_WIDTH" ).c_str(), nullptr, 10) : 600)
#define IFD_DIALOG_HEIGHT    (int)((!ngs::fs::environment_get_variable("IMGUI_DIALOG_HEIGHT").empty()) ? strtoul(ngs::fs::environment_get_variable("IMGUI_DIALOG_HEIGHT").c_str(), nullptr, 10) : 400)

namespace ifd {
  class FileDialog {
  public:
    static inline FileDialog& Instance() {
      static FileDialog ret;
      return ret;
    }

    FileDialog();
    ~FileDialog();

    bool Save(const std::string& key, const std::string& title, const std::string& filter, const std::string& startingFile = "", const std::string& startingDir = "");

    bool Open(const std::string& key, const std::string& title, const std::string& filter, bool isMultiselect = false, const std::string& startingFile = "", const std::string& startingDir = "");

    bool IsDone(const std::string& key);

    inline bool HasResult() { return m_result.size(); }
    inline const ghc::filesystem::path& GetResult() { return m_result[0]; }
    inline const std::vector<ghc::filesystem::path>& GetResults() { return m_result; }

    void Close();

    void RemoveFavorite(const std::string& path);
    void AddFavorite(const std::string& path);
    inline const std::vector<std::string>& GetFavorites() { return m_favorites; }

    inline void SetZoom(float z) { 
      m_zoom = std::min<float>(25.0f, std::max<float>(1.0f, z)); 
      m_refreshIconPreview();
    }
    inline float GetZoom() { return m_zoom; }

    std::function<void*(uint8_t*, int, int, char)> CreateTexture; // char -> fmt -> { 0 = BGRA, 1 = RGBA }
    std::function<void(void*)> DeleteTexture;

    class FileTreeNode {
    public:
      #ifdef _WIN32
      FileTreeNode(const std::wstring& path) {
        Path = ghc::filesystem::path(path);
        Read = false;
      }
      #endif

      FileTreeNode(const std::string& path) {
        Path = ghc::filesystem::path(path);
        Read = false;
      }

      ghc::filesystem::path Path;
      bool Read;
      std::vector<FileTreeNode*> Children;
    };
    class FileData {
    public:
      FileData(const ghc::filesystem::path& path);

      ghc::filesystem::path Path;
      bool IsDirectory;
      size_t Size;
      time_t DateModified;

      bool HasIconPreview;
      void* IconPreview;
      uint8_t* IconPreviewData;
      int IconPreviewWidth, IconPreviewHeight;
    };

  private:
    std::string m_currentKey;
    std::string m_currentTitle;
    ghc::filesystem::path m_currentDirectory;
    bool m_isMultiselect;
    bool m_isOpen;
    uint8_t m_type;
    char m_inputTextbox[1024];
    char m_pathBuffer[1024];
    char m_newEntryBuffer[1024];
    char m_searchBuffer[128];
    std::vector<std::string> m_favorites;
    bool m_calledOpenPopup;
    std::stack<ghc::filesystem::path> m_backHistory, m_forwardHistory;
    float m_zoom;

    std::vector<ghc::filesystem::path> m_selections;
    int m_selectedFileItem;
    void m_select(const ghc::filesystem::path& path, bool isCtrlDown = false);

    std::vector<ghc::filesystem::path> m_result;
    bool m_finalize(const std::string& filename = "");

    std::string m_filter;
    std::vector<std::vector<std::string>> m_filterExtensions;
    size_t m_filterSelection;
    void m_parseFilter(const std::string& filter);

    std::vector<int> m_iconIndices;
    std::vector<std::string> m_iconFilepaths; // m_iconIndices[x] <-> m_iconFilepaths[x]
    std::unordered_map<std::string, void*> m_icons;
    void* m_getIcon(const ghc::filesystem::path& path);
    void m_clearIcons();
    void m_refreshIconPreview();
    void m_clearIconPreview();

    std::thread* m_previewLoader;
    bool m_previewLoaderRunning;
    void m_stopPreviewLoader();
    void m_loadPreview();

    std::vector<FileTreeNode*> m_treeCache;
    void m_clearTree(FileTreeNode* node);
    void m_renderTree(FileTreeNode* node);

    unsigned int m_sortColumn;
    unsigned int m_sortDirection;
    std::vector<FileData> m_content;
    void m_setDirectory(const ghc::filesystem::path& p, bool addHistory = true, bool clearFileName = true);
    void m_sortContent(unsigned int column, unsigned int sortDirection);
    void m_renderContent();

    void m_renderPopups();
    void m_renderFileDialog();
  };

  const unsigned int file_icon[] = {
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x4c000000, 0xf5000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xdd000000, 0x2d000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0xd1000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6a000000, 0xa1000000, 0xff000000, 0xff000000, 0x2e000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x54000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x46000000, 0xf5000000, 0xe0000000, 0xff000000, 0x30000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6e000000, 0xf8000000, 0x01000000, 0xc3000000, 0xff000000, 0x30000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00000000, 0x00000000, 0xd2000000, 0xff000000, 0x30000000, 0x00000000, 0x00000000, 0x00000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x13000000, 0x00000000, 0x00000000, 0xd2000000, 0xff000000, 0x30000000, 0x00000000, 0x00000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x73000000, 0xff000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xbe000000, 0xff000000, 0x30000000, 0x00000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x65000000, 0xff000000, 0x34000000, 0x10000000, 0x10000000, 0x03000000, 0x0a000000, 0xdb000000, 0xff000000, 0x2f000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0f000000, 0xd9000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xed000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x06000000, 0x5e000000, 0x6c000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x60000000, 0x9e000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x52000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6b000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6b000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6a000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0x54000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x54000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0xff000000, 0xd2000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0x6b000000, 0xd2000000, 0xff000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x4c000000, 0xf5000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xf5000000, 0x4b000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
  };

  const unsigned int folder_icon[] = {
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00000000, 0x00000000, 0x45000000, 0x8a000000, 0x99000000, 0x97000000, 0x97000000, 0x97000000, 0x97000000, 0x97000000, 0x98000000, 0x81000000, 0x35000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x00000000, 0x9e000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0x80000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0x76000000, 0xff000000, 0xff000000, 0xf6000000, 0xe2000000, 0xe2000000, 0xe2000000, 0xe2000000, 0xe2000000, 0xe2000000, 0xe2000000, 0xff000000, 0xff000000, 0xff000000, 0x80000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0xe7000000, 0xff000000, 0xbe000000, 0x11000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x1e000000, 0xd1000000, 0xff000000, 0xff000000, 0x75000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0xfa000000, 0xff000000, 0x5a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x06000000, 0xe0000000, 0xff000000, 0xff000000, 0x68000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000,
    0xf4000000, 0xff000000, 0x67000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x11000000, 0xe4000000, 0xff000000, 0xff000000, 0xad000000, 0x94000000, 0x94000000, 0x94000000, 0x94000000, 0x94000000, 0x94000000, 0x94000000, 0x94000000, 0x94000000, 0x96000000, 0x8b000000, 0x4f000000, 0x00000000, 0x00000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x17000000, 0xe8000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xaf000000, 0x00000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0e000000, 0x88000000, 0xc3000000, 0xcd000000, 0xcc000000, 0xcc000000, 0xcc000000, 0xcc000000, 0xcc000000, 0xcc000000, 0xcc000000, 0xcb000000, 0xcc000000, 0xe2000000, 0xff000000, 0xff000000, 0x81000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0xb6000000, 0xff000000, 0xec000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x5b000000, 0xff000000, 0xf9000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x68000000, 0xff000000, 0xf4000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6a000000, 0xff000000, 0xf3000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6a000000, 0xff000000, 0xf3000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6a000000, 0xff000000, 0xf3000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6a000000, 0xff000000, 0xf3000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6a000000, 0xff000000, 0xf3000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6a000000, 0xff000000, 0xf3000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6a000000, 0xff000000, 0xf3000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6a000000, 0xff000000, 0xf3000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6a000000, 0xff000000, 0xf3000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6a000000, 0xff000000, 0xf3000000,
    0xf3000000, 0xff000000, 0x6a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x6a000000, 0xff000000, 0xf3000000,
    0xf4000000, 0xff000000, 0x68000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x68000000, 0xff000000, 0xf4000000,
    0xfa000000, 0xff000000, 0x5a000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x5a000000, 0xff000000, 0xf9000000,
    0xea000000, 0xff000000, 0xb5000000, 0x05000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x05000000, 0xb5000000, 0xff000000, 0xea000000,
    0x7e000000, 0xff000000, 0xff000000, 0xeb000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xd6000000, 0xeb000000, 0xff000000, 0xff000000, 0x7f000000,
    0x00000000, 0xac000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xff000000, 0xac000000, 0x00000000,
    0x00000000, 0x00000000, 0x53000000, 0x8f000000, 0x9a000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x99000000, 0x9a000000, 0x8f000000, 0x53000000, 0x00000000, 0x00000000,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
    0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff,
  };
}
