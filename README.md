<div align="center">

# ⚔️ Elysian Shadows

### The engine, toolkit, and tech behind *Elysian Shadows* — now open source.

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Kickstarter backers](https://img.shields.io/badge/Kickstarter%20backers-no%20strings%20attached-orange.svg)](LICENSE)
[![Status](https://img.shields.io/badge/status-needs%20love-red.svg)](#-heads-up)
[![PRs welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg)](#-contributing)

</div>

---

## 💜 To our Kickstarter backers

You believed in this project when it was just a dream and a Dreamcast. This release is for you.

We're opening up the full *Elysian Shadows* codebase — engine, toolkit, VMU emulator, and the cross-platform libraries underneath them — with its complete development history. It's MIT licensed for everyone, and **if you backed the Kickstarter, you can do whatever you want with it, no license terms attached.** See [LICENSE](LICENSE) for details.

## ⚠️ Heads up

Before you dive in, know what you're getting:

- 🎨 **No assets.** No art, music, maps, or game scripts are included — this is code only.
- 🔧 **It probably doesn't build anymore.** Toolchains, Qt versions, and SDKs have moved on since this code was last actively developed.
- 🩹 **It could use some love.** Expect rough edges, dead code, and the occasional colorful comment.

## 🧩 What's inside

| Component | What it is |
|---|---|
| **ESGamma** | The core *Elysian Shadows* game engine |
| **ESToolkit** | The ESTk level editor and content-creation toolkit (Qt) |
| **AssetIO** | Asset objects saved and loaded by the engine and toolkit |
| **game** | Game-specific engine code |
| **libGyro** | Cross-platform multimedia C API powering ESTk and ElysianVMU |
| **libGyro-Kinetic** | libGyro backend for the Sega Dreamcast |
| **ElysianVMU** | Sega Dreamcast VMU emulator (with **evmu-core**) |
| **QESTkUtils** | Shared Qt utilities and widgets |
| **UnitTests** | Tests for ESToolkit |

Third-party libraries (Lua 5.2, OOLua, the Spine runtime, prebuilt SDKs under `lib/`, and the Dreamcast tools under `tools/dc/`) remain under their own licenses.

## 🛠️ Building (good luck!)

Everything is wired together with qmake. Open the main project file in Qt Creator:

```
qtcreator/ESDev/ESDev.pro
```

Then pick what to build by adding a `CONFIG+=` argument under **Projects → Build Steps → Additional Arguments**:

| Target | Argument | Extras |
|---|---|---|
| ESToolkit | `CONFIG+=estk` | `CONFIG+=tests` (unit tests), `CONFIG+=docs` (Lua API docs) |
| ElysianVMU | `CONFIG+=vmu` | |

From the command line, run `qmake` on the same `.pro` file with the same arguments, then `make`.

## 🤝 Contributing

**Contributions are very welcome.** Getting things building on modern toolchains, fixing bugs, cleaning up, writing docs, porting — all of it helps. Open an issue or send a pull request.

## 📜 License

MIT, with an additional permission letting Elysian Shadows Kickstarter backers use the code without any license conditions. See [LICENSE](LICENSE) and [AUTHORS](AUTHORS).

<div align="center">

*Made with love (and a lot of late nights) by the Elysian Shadows Team.*

</div>
