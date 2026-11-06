# to-resolve.md (tileship)

## Onto kazilib, web, Linux, deployed (2026-11-06; continue.md)

Manual checks:

- **Firefox and Safari**: open https://kazin.dev/projects/tileship/, make an account, log
  in. Why: only Edge (Chromium, software WebGL) was tested; ImGui's text fields read the
  browser's key events, which differ most in Safari.
- **A phone**: same page. Why: ImGui wants a mouse and a keyboard; a touch arrives as
  SFML's first-finger mouse events, and the on-screen keyboard is untested.
- **From outside the home network** (mobile data): the page and a log in. Why: the tests
  ran on the LAN (dnsmasq resolves kazin.dev to the box).
- **A native client by hand** (`deploy\client\client.exe`, two windows): type in the
  fields, Login / Create Account with the mouse. Why: the native runs used the
  `user=` / `pass=` flags; the clicks were tested only in the browser.

Decisions taken (say if one should change):

- **The client opens on Log in** (it opened on the test map, `SceneMgr::Set(ID::Game)`,
  and ignored the server's answers): a log in now leads to the map.
- **Passwords**: stored as `SHA2(salt + password, 256)` with a random salt per account
  (they were plain text); the SQL values escaped (`Sql::Quote`: the name went straight
  into the query); `Auth::IsValid` (was always true) wants a name of 1-32 letters, digits
  or `_` and a non-empty password.
- **Ports**: ENet 7021 (the old 321 needs root on Linux), the gateway 7020.
- **The wire**: the framing kept (`<NetOp><MainOp><fields>`, `Auth` two fixed 33-byte
  fields), now written by kazilib's `OPkt` / `IPkt`; the game's own `packet.*` /
  `Socket.*` / log / magic_enum copies are gone.

Open questions:

- **Native clients on the live server?** Open UDP 7021 on the box's firewall (ufw) and the
  router, or keep the live server browser-only.
- **Passwords over native ENet** travel unencrypted (the browser's go over wss). Fine for a
  demo; a real release wants an encrypted channel or a challenge.
