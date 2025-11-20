# 🚗 Carpooling Management System 🌍

> **Connect. Share. Commute Smart.**

A dynamic C-based carpool management platform that intelligently matches riders and drivers, optimizing shared journeys with powerful core logic and an intuitive user interface. Whether you're looking to reduce travel costs or help the environment, this system makes it seamless.

---

## ✨ Features

🔹 **Core Matching Logic** — Sophisticated algorithm in `corelogic.c` that intelligently pairs drivers with passengers.

🔹 **User Interface & Operations** — Clean, interactive driver in `carpool_gui.c` for managing rides and matches.

🔹 **Sample Data Files** — Pre-loaded test data in `testing/` to explore and validate the system.

---

## 📁 Project Structure

```
Carpooling-Management-System/
├── 📂 code/
│   ├── 🔧 carpool_gui.c        ← User interface & operations
│   └── ⚙️  corelogic.c          ← Matching algorithm & core logic
│
├── 📂 testing/
│   ├── 🔐 admin.txt            ← Admin configuration
│   ├── 👥 drivers.txt          ← Driver profiles
│   ├── 👨‍👩‍👧‍👦 passengers.txt       ← Passenger records
│   ├── 🚕 flex_rides.txt        ← Flexible ride requests
│   └── ✅ matches.txt          ← Sample matched rides
│
└── 📖 README.md                ← You are here
```

---

## 🛠️ Requirements

- **C Compiler** — GCC, Clang, MinGW, or any standard C compiler
- **Windows, macOS, or Linux** — Cross-platform compatible
- **~10 MB disk space** — Minimal footprint

---

## 🚀 Quick Start

### **Build** (Windows, cmd.exe)

Navigate to the repo root and compile:

```cmd
gcc code\*.c -o carpool
```

✅ This generates `carpool.exe` in your current directory.

### **Run**

```cmd
carpool
```

The program reads from the `testing/` directory by default. Ensure the test data files are present or provide custom paths as arguments.

---

## 📊 Test Data & Examples

All sample data files are in the `testing/` folder:

| File | Purpose | Format |
|------|---------|--------|
| `admin.txt` | 🔐 Admin credentials & configuration | Text-based config |
| `drivers.txt` | 👥 Driver profiles & availability | One driver per line |
| `passengers.txt` | 👨‍👩‍👧‍👦 Passenger requests & preferences | One passenger per line |
| `flex_rides.txt` | 🚕 Flexible ride/carpool requests | Dynamic ride data |
| `matches.txt` | ✅ Output: successful ride matches | Match results |

**Tip:** Copy these files to your working directory, or modify paths in the code to point to them.

---

## 🎯 Next Steps & Enhancements

This README is your starting point! Here's what you can explore or improve:

- 🔍 **Dive Deeper** — Inspect `carpool_gui.c` and `corelogic.c` for exact API documentation and usage examples.
- 🏗️ **Automate Builds** — Create a `build.bat` (Windows) or `Makefile` for one-command compilation.
- 📝 **Document Algorithms** — Add detailed comments or a DESIGN.md explaining the matching logic.
- 🧪 **Expand Tests** — Build more comprehensive test data sets.
- 🌐 **Add Web UI** — Wrap the core logic in a REST API or web interface.

---

## 💡 Tips & Troubleshooting

**❌ Compilation fails?**
- Ensure GCC is installed: `gcc --version`
- Check file paths in `code\*.c` reference the correct data files.

**❌ Program crashes on startup?**
- Verify test data files exist in `testing/` or update file paths in the code.
- Check that `admin.txt`, `drivers.txt`, etc., are not empty or malformed.

**✅ Everything works?**
- Congrats! 🎉 Explore the matching logic and extend it with new features.

---

## 📄 License & Credits

- **Project:** Carpooling Management System
- **Repository:** [sarimraza1/Carpooling-Management-System](https://github.com/sarimraza1/Carpooling-Management-System)
- **Use & Enjoy:** 🚀
