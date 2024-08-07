# shogun

shogun is a [TUI](https://en.wikipedia.org/wiki/Text-based_user_interface) app for learning Japanese kanji. It is similar to [Anki](https://apps.ankiweb.net/), but requires the user to type in the answer.

![Screenshot of the app](/assets/screenshot.jpeg)


## Motivation

The only way for me to learn something is through brute force repetition. I've tried [Anki](https://apps.ankiweb.net/), but I disliked the "Hard" and "Easy" buttons method, as I was never really sure if I remembered the answer.

Typing in the answers aligns more closely with my learning style. In fact, I've previously developed a similar app for learning German using JavaScript, and it proved to be highly effective.

**Note:** You need to already know both katakana and hiragana before using this app.


## Features

- Written in modern C++ (C++17).
- Instant startups (<0.1s).
- Comprehensive documentation with doxygen-style comments.
- Automatic third-party dependency management using CMake's [FetchContent](https://www.foonathan.net/2022/06/cmake-fetchcontent/).
- No missing STL headers thanks to [header-warden](https://github.com/ryouze/header-warden).


## Project Structure

The `src` directory is organized as follows:

- `core`: Contains the most basic, primitive, standalone functions that do not depend on any other part of the application.
- `utils`: Contains utility functions that, while not as low-level as `core`, are used across different parts of the application.
- `io`: Manages input/output operations.

The `assets` directory contains the required `vocabulary.json` file used by the application. On configuration, CMake will automatically copy this file to the `build` directory. Installing the application will also copy the file to the appropriate directory (e.g., `/usr/local/bin`).


## Tested Systems

This project has been tested on the following systems:

- MacOS 14.5 (Sonoma)
- Manjaro 24.0 (Wynsdey)


## Requirements

To build and run this project, you'll need:

- C++17 or higher
- CMake


## Build

Follow these steps to build the project:

1. **Clone the repository**:

    ```bash
    git clone https://github.com/ryouze/shogun.git
    ```

2. **Generate the build system**:

    ```bash
    cd shogun
    mkdir build && cd build
    cmake ..
    ```

3. **Compile the project**:

    To compile the project, use the following command:

    ```bash
    make
    ```

    To use all available cores with `make`, pass the `-j` flag with the number of cores available on your system:

    ```bash
    # MacOS
    make -j$(sysctl -n hw.ncpu)
    ```

    ```bash
    # GNU/Linux
    make -j$(nproc)
    ```

    ```bash
    # 8-core M1 Pro on MacOS
    make -j8
    ```

After successful compilation, you can run the program using `./shogun`.

The mode is set to `Release` by default. To build in `Debug` mode, use `cmake -DCMAKE_BUILD_TYPE=Debug ..`.


## Install

If not already built, follow the steps in the [Build](#build) section and ensure that you are in the `build` directory.

To install the program, use the following command:

```bash
sudo cmake --install .
```


## Usage

To run the program, use the following command:

```bash
shogun
```

While running, the `Tab` key can be used to display a hint. The first time it is pressed, the program will display the kana transcription of the kanji. The second time, it will display the correct answer. If a flag (see below) is provided, the hint will respect the flag.

## Flags

```bash
[~] $ shogun --help
Usage: shogun [--help] [--kana] [--answer]

Learn Japanese kanji in the terminal.

Optional arguments:
  -h, --help  shows help message and exits
  --kana      display the kana transcription of the kanji
  --answer    display the correct answer
```

If the kana (`--kana`) flag is provided, the program displays the kana transcription of the kanji. It is disabled by default, because the program is designed for kanji learning and the kana transcription is displayed in the example sentence anyway.

```bash
shogun --kana
```

If the answer (`--answer`) flag is provided, the program displays the correct answer. This is useful for exploring the vocabulary or debugging.

```bash
shogun --answer
```


## Contributing

All contributions are welcome.


## Credits

- [argparse](https://github.com/p-ranav/argparse)
- [FTXUI](https://github.com/ArthurSonzogni/ftxui)
- [json](https://github.com/nlohmann/json)

## License

This project is licensed under the MIT License.
