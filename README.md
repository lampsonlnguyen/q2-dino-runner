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

See [LICENSE](LICENSE), [the Buddy notice](LICENSE-Buddy.txt), and [the Chromium notice](assets/LICENSE-Chromium.txt).
