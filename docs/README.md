# Persona 3 Dual

A Nintendo DS demake of **Persona 3**, developed in C++ using devkitPro. Based on the **Persona 3** series of games and inspired by the **Persona 3 Dual** online joke.
> Want to help? Join the [Discord!](https://discord.gg/CQnkc5gS6a) Any help, big or small, would be greatly appreciated!

![Stars](https://img.shields.io/github/stars/p3d-project/persona-3-dual?style=flat-square&color=gold)
![Forks](https://img.shields.io/github/forks/p3d-project/persona-3-dual?style=flat-square&color=blue)
![Last Commit](https://img.shields.io/github/last-commit/p3d-project/persona-3-dual?style=flat-square&color=green)
![License](https://img.shields.io/github/license/p3d-project/persona-3-dual?style=flat-square)

[![C++](https://img.shields.io/badge/C++-%2300599C.svg?logo=c%2B%2B&logoColor=white)](#)
[![Python](https://img.shields.io/badge/python-3670A0?logo=python&logoColor=white)](#)
![Platform](https://img.shields.io/badge/platform-Nintendo%20DS-red?style=flat-square)
![Architecture](https://img.shields.io/badge/architecture-ARM9/ARM7-blue)

[![Discord](https://img.shields.io/discord/1498850477545357482?label=Discord&logo=discord&style=flat-square&color=5865F2)](https://discord.gg/CQnkc5gS6a)





[![IMAGE ALT TEXT](http://img.youtube.com/vi/4RW8ppcPK6o/0.jpg)](http://www.youtube.com/watch?v=4RW8ppcPK6o "Persona 3 Dual (First Look)")

---

## Running the Game

The game requires FAT filesystem support to load assets at runtime. Currently, we only support:

- **[melonDS](https://melonds.kuribo64.net/downloads.php)** (multiplatform emulator)
- **Real DS / DSi / 3DS hardware** via [TWiLight Menu++](https://wiki.ds-homebrew.com/twilightmenu/)

> Other flashcard launchers may work, but are untested

### melonDS (Emulator)

1. Download `persona-3-dual.nds` and `sdcard.img.gz` from the latest release, & decompress `sdcard.img.gz` **Developers**: Build the project with `make` - this produces both `persona-3-dual.nds` and `sdcard.img`.
2. In melonDS, go to **Settings → DLDI** and enable DLDI.
3. Set the SD card image path to the generated `sdcard.img`.
> **Do NOT enable "Sync SD card to folder"**. This will wipe the contents of the folder!

Now, you can open melonDS and load the `persona-3-dual.nds` ROM!

<img width="316" height="300" alt="melonDS" src="https://github.com/user-attachments/assets/d34997e6-d13f-4428-a2b6-41b5272405d7" />

### Real Hardware (DS / DSi / 3DS)
Requires [TWiLight Menu++](https://wiki.ds-homebrew.com/twilightmenu/) with DLDI patching enabled.

1. Download `persona-3-dual.nds` and `data.zip` from the latest release, & decompress `data.zip`
**Developers**: Build the project with `make` - this produces the `persona-3-dual.nds` and the content in `/data`.
2. In TWiLight Menu++ settings, ensure **DLDI access** is set to **ARM9** & the **Game Loader** is set to **nds-bootstrap**
3. On your SD card, navigate to your `/roms/nds/` folder (or equivalent).
4. Copy `persona-3-dual.nds` and the entire `/data` folder into that directory:
   ```
   /roms/nds/
   ├── persona-3-dual.nds
   └── data/
       ├── music/
       ├── video/
       └── ...
   ```
5. Launch the game through TWiLight Menu++ as normal.
---

## Team Onboarding & Setup
See [ONBOARDING.md](https://p3dual.com/docs/onboarding/

---

## Roadmap
- See the [Project Board](https://github.com/orgs/p3d-project/projects/1) for current progress and open issues.
- See the [P3D Roadmap](https://docs.google.com/document/d/1VUxY2xVGtKzGWCES0VeZ_-vMpKSifB5vTW2ZkXJMPJM/edit?usp=sharing) document for progress on Milestones

---

![Alt](https://repobeats.axiom.co/api/embed/7e6123f89c4c8a46b04e80b52694693203c2cf9d.svg "Repobeats analytics image")

---
## Legal
> Disclaimer: This section is inspired by the [FEMC Reloaded project](https://github.com/MadMax1960/Femc-Reloaded-Project)

**TLDR**: This is a fan project and is not affiliated with or endorsed by Atlus or Sega. We (the team & the project) will not accept any monetary donations or funding for this project, nor will we make money from this project. We (the team & the project) do not enable, condone, or endorce piracy.

### Intellectual Property & Copyright
This game is a labor of love created to celebrate Persona 3 and share our collective creativity within the fan community, not to infringe on the rights of the original creators.

- **ⓒ Atlus ⓒ Sega. All Rights Reserved.** A large number of game assets, including but not limited to 3D models, 2D artwork, music, sound effects, character designs, logos, and the Persona trademark, are the exclusive intellectual property of Atlus and Sega (or their respective copyright holders).
- **Not for Sale**: This project does not, and will never, go up for sale. Nothing monetary is being sought from it.

### Codebase License
While the game assets belong to their respective copyright holders, the custom source code written for this game engine is licensed under the Creative Commons **Attribution-NonCommercial-ShareAlike 4.0 International License (CC BY-NC-SA 4.0).**

The open-source license applied to this codebase **does not** extend to any copyrighted assets or intellectual property owned by Atlus or Sega. For the engine code itself, the license means:

- **Attribution (BY)**: You must give appropriate credit, provide a link to the license, and indicate if changes were made. You may do so in any reasonable manner, but not in any way that suggests the licensor endorses you or your use.
- **NonCommercial (NC)**: You may not utilize this codebase for commercial purposes.
- **ShareAlike (SA)**: If you remix, transform, or build upon the engine code, you must distribute your contributions under the same license as the original.
