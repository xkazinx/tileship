# CLAUDE.md (tileship)

- **Build:** `powershell -ExecutionPolicy Bypass -File build.ps1` (`setup.ps1`: kazilib submodule, SFML 3.1, MySQL Connector/C 6.1.11 x86, `deploy\server\init.json` from the example; then kazilib FASTBuild on `src\fastbuild.json`: kazilib, server, client). `build.ps1 server|client`, `-Regen`, `-Clean`, `-Local`; `-Configuration Debug` only if asked. `setup.ps1`'s `submodule update` resets kazilib to the recorded pointer: stage a moved pointer first (`git add src/dep/kazilib`).
- **Web:** `build_web.ps1` (Emscripten, kazilib `web\`: `kazilib_web_target`, `kazilib_web_imgui`) → `deploy\server\web\`; `src\client\CMakeLists.txt` is the web build only. Browser-only code `#ifdef __EMSCRIPTEN__` (the loop, the WebSocket URL), Windows-only `#ifdef _WIN32`.
- **Linux / deploy:** `build_linux.ps1` (Docker `tileship-linux-build:26.04`, `src\server\CMakeLists.txt`); `deploy_ubuntu.ps1` → box `kazin-music`: service `tileship`, `/opt/tileship`, ENet UDP 7021, gateway 127.0.0.1:7020, Caddy `kazin.dev/projects/tileship` (`deploy\ubuntu\`).
- **Database:** MySQL table `players` (made by `Sql::EnsureSchema`); the account in `init.json` beside the server (`db_*` flags), never in git (`src\server\init.json.example`); values always through `Sql::Quote`.
- **Kazilib first:** packets / sockets / broadcasters (`net/`), scenes, flags, log (`Out` / `Info` / `Warn` / `Err`), ImGui, the gateway; reusable code goes there.
- **Tests:** `src\tests\web\run_web_test.ps1` (local, and `-Url https://kazin.dev/projects/tileship/`), 0 failed, outside the command sandbox; native: two `client.exe user=<n> pass=<p> create shot=<png> quitaftershot` on one server.
- **Protocol:** `src\common\netops.h` `NetOp` / `MainOp` (u8) append-only; writer and reader change together (`Auth`: two fixed fields, 33 bytes each).
- **README:** the 2021 original stays at its end (`## Original README`).
- **Names:** members `_name`, functions / types `PascalCase`, `struct` (full rules: `D:\code\hb\hbmillennium\CLAUDE.md`).
