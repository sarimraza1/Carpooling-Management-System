# 🚗 University Carpooling Management System

A comprehensive carpooling management system for universities, built with C and GTK3 GUI framework.

## 📥 Download & Run (No Installation Required!)

### For Windows Users:

1. Go to [Releases](https://github.com/sarimraza1/Carpooling-Management-System/releases)
2. Download `CarPoolApp-Portable.zip` (~36 MB)
3. Extract the ZIP file
4. Double-click `run_carpool.bat`
5. That's it! No installation needed.

### Default Login Credentials:

- **Admin**:
  - Email: `admin@uni.edu`
  - Password: `admin123`

## ✨ Features

### For Drivers:

- Register and create profile
- Set up fixed routes or offer flexible rides
- Manage weekly timetable
- View matched passengers
- Track available seats

### For Passengers:

- Register and create profile
- Search for available rides
- View driver routes and schedules
- Request rides
- Manage bookings

### For Administrators:

- View all drivers and passengers
- Manage user accounts
- Monitor system activity
- Generate reports

## 🛠️ Development

### Project Structure:

```
├── code/
│   ├── carpool_gui.c          # Main GUI application
│   ├── corelogic.c            # Core business logic
│   ├── README_corelogic.md    # Core logic documentation
│   └── technicaldoc.md        # Technical documentation
├── testing/
│   ├── carpool_gui.exe        # Compiled executable
│   └── *.txt                  # Test data files
└── portable_package/          # Ready-to-distribute package
```

### Building from Source:

#### Prerequisites:

- MinGW-w64 or MSYS2
- GTK3 development libraries
- GCC compiler

#### Compile:

```bash
gcc carpool_gui.c -o carpool_gui.exe `pkg-config --cflags --libs gtk+-3.0`
```

For detailed build instructions, see [BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)

### Creating Portable Package:

```batch
create_portable_package.bat
```

This will automatically:

- Copy the executable
- Bundle all required GTK DLLs
- Include themes and icons
- Create launcher script
- Package everything for distribution

See [PORTABLE_PACKAGE_GUIDE.md](PORTABLE_PACKAGE_GUIDE.md) for details.

## 📚 Documentation

- **[BUILD_INSTRUCTIONS.md](BUILD_INSTRUCTIONS.md)** - How to compile and build
- **[PORTABLE_PACKAGE_GUIDE.md](PORTABLE_PACKAGE_GUIDE.md)** - Creating portable packages
- **[DISTRIBUTION_GUIDE.md](DISTRIBUTION_GUIDE.md)** - Distributing your application
- **[code/README_corelogic.md](code/README_corelogic.md)** - Core logic documentation
- **[code/technicaldoc.md](code/technicaldoc.md)** - Technical specifications

## 🎯 Use Cases

### University Scenario:

Students living in the same area can:

1. Drivers register and set their daily routes to campus
2. Passengers search for rides along their route
3. System matches drivers and passengers
4. Both parties save money and reduce carbon footprint

### Features:

- **Fixed Routes**: Regular daily commutes with set schedules
- **Flexible Rides**: One-time or occasional rides
- **Smart Matching**: Automatic matching based on routes and schedules
- **Timetable Integration**: Match rides with class schedules

## 🔧 Technical Details

- **Language**: C
- **GUI Framework**: GTK3
- **Platform**: Windows (portable package available)
- **Data Storage**: Text files (drivers.txt, passengers.txt, etc.)
- **Architecture**: Modular design with separate GUI and core logic

## 📝 License

This project is created as a university project for Programming Fundamentals course.

## 👥 Contributors

- Sarim Raza (@sarimraza1)

## 🐛 Known Issues & Limitations

- Currently Windows-only (portable package)
- Data stored in plain text files (not encrypted)
- Single-user mode (no concurrent access)

## 🚀 Future Enhancements

- Database integration (SQLite)
- Multi-platform support (Linux, macOS)
- Real-time notifications
- GPS integration
- Mobile app
- Payment integration

## 📞 Support

If you encounter any issues:

1. Check the [DISTRIBUTION_GUIDE.md](DISTRIBUTION_GUIDE.md) troubleshooting section
2. Make sure you're running `run_carpool.bat` (not the .exe directly)
3. Verify all files are extracted properly
4. Open an issue on GitHub

## 🎓 Academic Context

This project was developed as part of the Programming Fundamentals course to demonstrate:

- C programming skills
- GUI development
- Data structure implementation
- File I/O operations
- Software design principles

---

**Version**: 1.0  
**Last Updated**: November 2025  
**Status**: Active Development
