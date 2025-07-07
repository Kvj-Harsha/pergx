# 🔍 pergx — A Customized Parallel Regex Searcher

**pergx** is a modified version of the original `perg` tool. It's a high-performance, multithreaded regex (or literal) search utility written in modern C++ with OpenMP. Designed for fast recursive file scanning, smart exclusion, and highlighted output.

---

## ✨ What's New in `pergx`

- Custom logic and formatting
- Enhanced multithreaded file scanning
- Optional output to file
- Flexible exclusion patterns
- Literal or regex search toggle
- Cleanly colored match output

---

## 🧱 Build Instructions

### 📦 Prerequisites

- C++17 compiler (e.g., `g++`, `clang++`)
- OpenMP support

### 🔨 Build

```sh
make
```

To clean:
```sh
make clean
```

---

## 🚀 Usage

```sh
./pergx -e <term> -f <path> [options]
```

### Required
- `-e <term>`: Search term (regex or literal).
- `-f <path>`: Path to file or directory.

### Optional
- `-o <file>`: Output results to a file.
- `-t <n>`: Number of threads (default: 4).
- `-i`: Case-insensitive search.
- `--literal`: Search as plain string, not regex.
- `--exclude <pattern>`: Exclude files by pattern.

### Example

```sh
./pergx -e "TODO" -f ./src -i --exclude ".git" -o result.txt
```

---

## 🗂 Project Structure

```
.
├── main.cpp       # Source code
├── Makefile       # For build automation
└── README.md      # You're here
```

---

## 🛠 Developer Tips

You can also run with arguments using:

```sh
make run ARGS="-e main -f ./ -t 8 -i"
```

---

## 📃 License

This is a modified version of `perg`. License terms: MIT or based on the original license if inherited.

---

## 🧠 Future Ideas

- Add interactive UI (e.g., TUI with `ncurses`)
- Export results as JSON
- Integrate filetype filters (e.g., only `.cpp` or `.md`)

---

Made with ❤️ and OpenMP.
