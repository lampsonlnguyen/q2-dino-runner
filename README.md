# Dino Runner for Q2 Pod

Play the Chrome-style dinosaur runner on your Shanling Q2. Jump over cacti, duck under pterodactyls, and see how far you can run. The game gets faster as you go, switches to night mode at distance milestones, and saves your best score to the SD card.

## Install

1. Extract `q2-dino-runner.zip` to the root of your microSD card. The archive installs a hidden `.dino-runner` folder.
2. Safely eject the card and open **Library → Games → Dino Runner** in Q2 Pod.

## Controls

| Control | Action |
| --- | --- |
| Centre | Start, jump, restart after a crash, or resume |
| Tap the playfield | Jump |
| Hold the lower part of the touchscreen | Duck, or drop faster while jumping |
| Return | Pause. Press Return again while paused to quit |

Your best score is stored in `.dino-runner/best`. The card app uses the same screen and input interface as Q2 Pod’s other card games.

## Build from source

On Ubuntu or WSL:

```sh
sudo apt install clang lld make gcc python3 python3-pil zip
make
make check
```

Use `dist/q2-dino-runner.zip` to install the build. The MIPS binary draws sprites directly into the Q2 framebuffer and uses the firmware’s input socket. Pillow packs the sprite sheet at build time.

## Credits

The game engine is from [AKJ7/dinorunner](https://github.com/AKJ7/dinorunner/tree/c6be2cb66d8de1480bd797e4f3ea2ab2769248f0). The original Dino artwork comes from Chromium, via the upstream demo’s sprite sheet. The Q2 platform code follows [q2-2048](https://github.com/lampsonlnguyen/q2-2048) and [Buddy](https://github.com/DiamondBond/q2-fidget).

Code is distributed under GNU GPL version 3; see [LICENSE](LICENSE). Imported engine files retain their original copyright years and identify [AKJ7/dinorunner](https://github.com/AKJ7/dinorunner) as their upstream. Q2 integration changes are maintained in this repository. The Chromium artwork has its own [notice](assets/LICENSE-Chromium.txt). Both license texts are included in the install package.

## Automatic builds and releases

GitHub Actions builds and tests each push to `main` and every pull request. Download the **q2-dino-runner-card-app** artifact from the successful Actions run for a test build. You can also start a build from **Actions → Build and release Dino Runner → Run workflow**.

To publish a release, push a new version tag after pulling the latest source:

```sh
git pull --ff-only
git tag v1.0.1
git push origin v1.0.1
```

Use an unused version number for each release. Once tests, the MIPS build, and ZIP verification pass, Actions creates the GitHub Release and attaches `dist/q2-dino-runner.zip` as **q2-dino-runner.zip**. Users can download this install ZIP directly from Releases. Re-running a tag build replaces that tag's ZIP asset. Ordinary branch pushes and manual branch builds only produce Actions artifacts.
