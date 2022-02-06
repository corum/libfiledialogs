cd "${0%/*}"

if [ `uname` = "Darwin" ]; then
  clang++ "/opt/local/lib/libSDL2.a" "/opt/local/lib/libiconv.a" "lib/ImGuiFileDialog/ImGuiFileDialog.cpp" "imgui.cpp" "imgui_impl_sdl.cpp" "imgui_impl_opengl2.cpp" "imgui_draw.cpp" "imgui_tables.cpp" "imgui_widgets.cpp" "modifywindow.mm" "setpolicy.mm" "filedialogs.cpp" "main.cpp" -o "filedialog" -std=c++20 -Wno-deprecated-enum-enum-conversion -I. -DIMGUI_USE_WCHAR32 -DUSE_STD_FILESYSTEM -I/opt/local/include -I/opt/local/include/SDL2 -std=c++20 -Wno-deprecated-enum-enum-conversion -ObjC++ -framework OpenGL -Wl,-framework,CoreAudio -Wl,-framework,AudioToolbox -Wl,-weak_framework,CoreHaptics -Wl,-weak_framework,GameController -Wl,-framework,ForceFeedback -lobjc -Wl,-framework,CoreVideo -Wl,-framework,Cocoa -Wl,-framework,Carbon -Wl,-framework,IOKit -Wl,-weak_framework,QuartzCore -Wl,-weak_framework,Metal -fPIC -arch arm64 -arch x86_64 -fPIC
elif [ $(uname) = "Linux" ]; then
  g++ "lib/ImGuiFileDialog/ImGuiFileDialog.cpp" "imgui.cpp" "imgui_impl_sdl.cpp" "imgui_impl_opengl2.cpp" "imgui_draw.cpp" "imgui_tables.cpp" "imgui_widgets.cpp" "filedialogs.cpp" "main.cpp" -o "filedialog" -std=c++20 -Wno-deprecated-enum-enum-conversion -I. -DIMGUI_USE_WCHAR32 -DUSE_STD_FILESYSTEM -static-libgcc -static-libstdc++ `pkg-config --cflags --libs sdl2 --static` -lGL -no-pie -fPIC
elif [ $(uname) = "FreeBSD" ]; then
  clang++ "lib/ImGuiFileDialog/ImGuiFileDialog.cpp" "imgui.cpp" "imgui_impl_sdl.cpp" "imgui_impl_opengl2.cpp" "imgui_draw.cpp" "imgui_tables.cpp" "imgui_widgets.cpp" "filedialogs.cpp" "main.cpp" -o "filedialog" -std=c++20 -Wno-deprecated-enum-enum-conversion -I. -DIMGUI_USE_WCHAR32 -DUSE_STD_FILESYSTEM `pkg-config --cflags --libs sdl2 --static` -lGL -lc -fPIC
elif [ $(uname) = "DragonFly" ]; then
  g++ "lib/ImGuiFileDialog/ImGuiFileDialog.cpp" "imgui.cpp" "imgui_impl_sdl.cpp" "imgui_impl_opengl2.cpp" "imgui_draw.cpp" "imgui_tables.cpp" "imgui_widgets.cpp" "filedialogs.cpp" "main.cpp" -o "filedialog" -std=c++20 -Wno-deprecated-enum-enum-conversion -I. -DIMGUI_USE_WCHAR32 -DUSE_STD_FILESYSTEM -static-libgcc -static-libstdc++ `pkg-config --cflags --libs sdl2 --static` -lGL -lc -fPIC
elif [ $(uname) = "OpenBSD" ]; then
  clang++ "lib/ImGuiFileDialog/ImGuiFileDialog.cpp" "imgui.cpp" "imgui_impl_sdl.cpp" "imgui_impl_opengl2.cpp" "imgui_draw.cpp" "imgui_tables.cpp" "imgui_widgets.cpp" "filedialogs.cpp" "main.cpp" -o "filedialog" -std=c++20 -Wno-deprecated-enum-enum-conversion -I. -DIMGUI_USE_WCHAR32 -DUSE_STD_FILESYSTEM -I/usr/local/include `pkg-config --cflags --libs sdl2 --static` -lGL -lc -fPIC
else
  g++ "lib/ImGuiFileDialog/ImGuiFileDialog.cpp" "imgui.cpp" "imgui_impl_sdl.cpp" "imgui_impl_opengl2.cpp" "imgui_draw.cpp" "imgui_tables.cpp" "imgui_widgets.cpp" "filedialogs.cpp" "main.cpp" -o "filedialog.exe" -std=c++20 -Wno-deprecated-enum-enum-conversion -I. -DIMGUI_USE_WCHAR32 -DUSE_STD_FILESYSTEM -DWIN32 -static-libgcc -static-libstdc++ -static `pkg-config --cflags --libs sdl2 --static` -lopengl22 -fPIC
fi
