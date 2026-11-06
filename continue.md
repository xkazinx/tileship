# continue.md (tileship, master)

## Repos

| Repo | Commit | State |
|---|---|---|
| tileship | master: the kazilib / web / Linux / deploy commit (this file's) on `7136885` | pushed |
| kazilib (`src/dep/kazilib`, new submodule) | master `a28bf1b` (ImGui in a browser, pushed by the tiled port) | pushed, nothing of this work in it |

## Last work (2026-11-06)

- **Onto kazilib, SFML 3.1, FASTBuild**: Release x86, v145, 64-bit-hosted tools
  (`build.ps1`, `setup.ps1`, `src\fastbuild.json`, `src\Directory.Build.props`). The game's
  own packet / socket / log / magic_enum / imgui copies replaced by kazilib's (`OPkt` /
  `IPkt`, `Socket`, `BroadcasterBase`, `scene/manager.h`, `initflags.h`, `log.h`, ImGui).
  The wire's framing kept. `C:\LIBPATH`, `symlinks.bat` gone.
- **Game**: opens on Log in, a log in leads to the test map with the player's name;
  reconnects every 3 s; passwords salted + SHA2 in MySQL, SQL values escaped; the
  server makes its table; player slots freed on disconnect; `Random` seeded properly.
- **Linux server**: `build_linux.ps1` (Docker `tileship-linux-build:26.04`, libmysqlclient).
- **Web client**: `build_web.ps1` (kazilib `web\`, `kazilib_web_imgui`).
- **Deployed**: https://kazin.dev/projects/tileship/ (service `tileship`, `/opt/tileship`,
  UDP 7021, gateway 127.0.0.1:7020, Caddy `kazin.dev.d/tileship.caddy`, MySQL database
  `tileship` / `tileship@localhost`, its password only in `/opt/tileship/bin/init.json`).
- **Showcase**: `docs\showcase\1_login.png` .. `6_game_second.png` (live page),
  `docs\screenshot.png`, kazin.dev photos `docs\showcase\kazin\tileship.jpg`,
  `tileship-1.jpg` .. `tileship-4.jpg` (1024x768; not placed in the portfolio repo).

## Tested

| Test | Result |
|---|---|
| Native: two `client.exe` (auto log in, new accounts) on the Windows server, MySQL 8.4 in Docker | both in the game (screenshots); taken name and wrong password refused |
| Linux server in a clean Ubuntu 26.04 container (libmysqlclient24) against MySQL 8.4 (default auth, TLS), a Windows client over UDP | account made, logged in |
| `src\tests\web\run_web_test.ps1` (two Edge tabs, local Windows server) | passed |
| same, `-Url https://kazin.dev/projects/tileship/` (Caddy, wss, Linux server, the box's MySQL) | passed (its accounts deleted after) |
| a native client and two browser tabs on one server at once | all logged in |

Known gaps: only Chromium (Edge, SwiftShader); no Firefox / Safari / phone; native
clients can't reach the live server (UDP 7021 closed by ufw); the native GUI's clicks
only by hand (the native runs used `user=` / `pass=`). `deploy\server\init.json` here
points at a test container that is gone: refill it for a local server.

## Next steps

- `to-resolve.md`: the manual checks and the decisions to confirm.
- The game itself (turns, ships, the map from the server): nothing written yet.
- kazin.dev portfolio entry: the photos are ready in `docs\showcase\kazin\`.

## The user's prompts (this work)

1. "now I want you standarize the work and do the same with the githubs of yip, yips, tileship, tiled, nft, use various agents if well scoped"
2. "all the process we've done with tictactoe"
