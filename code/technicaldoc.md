<style>
@media print {
    /* A4 Paper Optimization */
    @page {
        size: A4;
        margin: 2cm 1.5cm;
    }
    
    body {
        font-size: 10pt;
        line-height: 1.4;
        max-width: 100%;
    }
    
    /* Prevent page breaks inside important elements */
    h1, h2, h3, h4, h5, h6 {
        page-break-after: avoid;
        page-break-inside: avoid;
    }
    
    pre, code, table {
        page-break-inside: avoid;
    }
    
    /* Add page breaks before major sections */
    h2 {
        page-break-before: always;
        margin-top: 0;
    }
    
    /* First h2 shouldn't break */
    h2:first-of-type {
        page-break-before: avoid;
    }
    
    /* Optimize tables for A4 width */
    table {
        font-size: 9pt;
        width: 100%;
        table-layout: fixed;
    }
    
    /* Optimize code blocks */
    pre {
        font-size: 8pt;
        white-space: pre-wrap;
        word-wrap: break-word;
    }
    
    /* Better spacing */
    p {
        orphans: 3;
        widows: 3;
    }
}

@media screen {
    body {
        max-width: 21cm; /* A4 width */
        margin: 0 auto;
        padding: 2cm;
        background: #f5f5f5;
    }
    
    .markdown-body {
        background: white;
        padding: 2cm;
        box-shadow: 0 0 10px rgba(0,0,0,0.1);
    }
}
</style>

---

<div style="page-break-after: always;"></div>

<div align="center" style="margin-top: 8cm;">

# 🚗 University Carpooling System

## Complete Technical Documentation

### Detailed Code Analysis & Implementation Guide

---

**Project**: University Carpooling System  
**File**: corelogic.c  
**Language**: C  
**Lines of Code**: 978  
**Functions**: 36

---

**Documentation Version**: 2.0  
**Last Updated**: November 2025  
**Author**: Technical Documentation Team

---

### Document Overview

This comprehensive technical documentation provides an in-depth analysis of the
University Carpooling System implementation. It covers all aspects including data
structures, algorithms, memory management, file handling, and user interfaces.

**Total Pages**: ~95  
**Sections**: 14  
**Code Examples**: 100+  
**Diagrams**: 25+

</div>

<div style="page-break-after: always;"></div>

---

## 📋 Table of Contents

1. [System Overview](#-system-overview)
2. [Constants & Macros](#-constants--macros)
3. [Architecture & Design](#️-architecture--design)
4. [Data Structures Deep Dive](#-data-structures-deep-dive)
5. [Memory Management](#-memory-management)
6. [File Handling System](#-file-handling-system)
7. [Complete Function Reference](#-complete-function-reference)
8. [Core Functions Explained](#️-core-functions-explained)
9. [Dashboard Functions Deep Dive](#️-dashboard-functions-deep-dive)
10. [Program Flow & Algorithms](#-program-flow--algorithms)
11. [Code Examples & Snippets](#-code-examples--snippets)
12. [String Manipulation & Input Handling](#-string-manipulation--input-handling)
13. [Advanced Features](#-advanced-features)
14. [Troubleshooting & Best Practices](#-troubleshooting--best-practices)

---

## 🎯 System Overview

### What is This System?

The **University Carpooling System** is a comprehensive C-based application designed to facilitate ride-sharing among university students. It connects drivers who have available seats with passengers looking for rides, supporting both regular semester-long carpools and one-time flexible rides.

### Key Capabilities

✅ **Dual Ride Types**: Fixed (semester-long) and Flexible (one-time) rides  
✅ **Dynamic Memory Management**: Automatically grows with user base  
✅ **Persistent Data Storage**: All data saved to text files  
✅ **Smart Matching Algorithm**: Links passengers with compatible drivers  
✅ **Multi-User Support**: Admin, Driver, and Passenger roles  
✅ **Real-time Seat Management**: Prevents overbooking  
✅ **Co-passenger Visibility**: Encourages community building

### System Requirements

- **Platform**: Windows (uses `cls` command)
- **Compiler**: GCC or any C compiler supporting C99
- **Memory**: Minimal (starts with ~10KB, grows dynamically)
- **Storage**: Text files for data persistence

---

## 🔢 Constants & Macros

### System-Wide Constants

The system uses preprocessor macros to define important limits and constraints:

```c
#define MAX_STOPS 10
#define MAX_DAYS 5
```

### MAX_STOPS

**Value**: `10`  
**Purpose**: Maximum number of stops/locations in a route  
**Used in**: Driver routes, FlexibleRide stops

**Why 10?**

- Balances flexibility with memory efficiency
- Typical carpooling routes have 3-7 stops
- Allows room for complex routes without excessive memory

**Memory Impact**:

```c
// In Driver structure
char route[MAX_STOPS][100];  // 10 × 100 = 1000 bytes

// In FlexibleRide structure
char stops[MAX_STOPS][100];  // 10 × 100 = 1000 bytes
```

**Usage Example**:

```c
void add_route() {
    Driver *d = &app.drivers[app.current_driver_idx];

    while (d->route_count < MAX_STOPS) {  // Prevents overflow
        char stop[100];
        printf("Stop %d: ", d->route_count + 1);
        scanf(" %[^\n]", stop);

        if (strcmp(stop, "done") == 0)
            break;

        strcpy(d->route[d->route_count++], stop);
    }
}
```

**What happens if exceeded?**

- Loop terminates at MAX_STOPS
- User cannot add more stops
- No buffer overflow occurs

---

### MAX_DAYS

**Value**: `5`  
**Purpose**: Number of working days in a week (Monday-Friday)  
**Used in**: Driver timetables, Passenger timetables

**Why 5?**

- University operates Monday through Friday
- Weekend rides not supported in current version
- Simplifies scheduling logic

**Memory Impact**:

```c
// In Driver structure
DaySchedule timetable[MAX_DAYS];  // 5 × 24 bytes = 120 bytes

// In Passenger structure (currently unused)
DaySchedule timetable[MAX_DAYS];  // 5 × 24 bytes = 120 bytes
```

**Day Mapping**:

```c
const char *days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

// Index 0 = Monday
// Index 1 = Tuesday
// Index 2 = Wednesday
// Index 3 = Thursday
// Index 4 = Friday
```

**Usage Example**:

```c
void add_timetable() {
    Driver *d = &app.drivers[app.current_driver_idx];
    const char *days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

    for (int i = 0; i < MAX_DAYS; i++) {  // Loop exactly 5 times
        printf("--- %s ---\n", days[i]);
        printf("Start Time (HH:MM or 'off'): ");
        scanf("%s", d->timetable[i].start_time);

        if (strcmp(d->timetable[i].start_time, "off") == 0) {
            d->timetable[i].is_off_day = 1;
        } else {
            d->timetable[i].is_off_day = 0;
            printf("End Time (HH:MM): ");
            scanf("%s", d->timetable[i].end_time);
        }
    }
}
```

### Included Headers

```c
#include <stdio.h>   // Standard I/O: printf, scanf, FILE operations
#include <string.h>  // String operations: strcpy, strcmp, strstr, strlen
#include <stdlib.h>  // Memory: malloc, realloc, free, system()
```

**Why these headers?**

| Header     | Functions Used                                                                   | Purpose                            |
| ---------- | -------------------------------------------------------------------------------- | ---------------------------------- |
| `stdio.h`  | `printf`, `scanf`, `fopen`, `fclose`, `fprintf`, `fscanf`, `fgets`               | Console I/O and file operations    |
| `string.h` | `strcpy`, `strncpy`, `strcmp`, `strstr`, `strcat`, `strlen`, `strchr`, `strcspn` | String manipulation and searching  |
| `stdlib.h` | `malloc`, `realloc`, `free`, `system`, `exit`                                    | Dynamic memory and system commands |

---

## 🏗️ Architecture & Design

### System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     MAIN APPLICATION                         │
│                      (corelogic.c)                          │
└─────────────────────────────────────────────────────────────┘
                            │
        ┌───────────────────┼───────────────────┐
        │                   │                   │
        ▼                   ▼                   ▼
┌──────────────┐   ┌──────────────┐   ┌──────────────┐
│   ADMIN      │   │   DRIVER     │   │  PASSENGER   │
│  INTERFACE   │   │  INTERFACE   │   │  INTERFACE   │
└──────────────┘   └──────────────┘   └──────────────┘
        │                   │                   │
        └───────────────────┼───────────────────┘
                            │
        ┌───────────────────┼───────────────────┐
        │                   │                   │
        ▼                   ▼                   ▼
┌──────────────┐   ┌──────────────┐   ┌──────────────┐
│   MEMORY     │   │     FILE     │   │   MATCHING   │
│  MANAGER     │   │   HANDLER    │   │   ENGINE     │
└──────────────┘   └──────────────┘   └──────────────┘
        │                   │                   │
        └───────────────────┼───────────────────┘
                            │
                            ▼
        ┌───────────────────────────────────────┐
        │         DATA PERSISTENCE LAYER        │
        │  drivers.txt | passengers.txt         │
        │  flex_rides.txt | matches.txt         │
        │  admin.txt                            │
        └───────────────────────────────────────┘
```

### Design Patterns Used

1. **Singleton Pattern**: Global `app` structure maintains single instance of application state
2. **Repository Pattern**: Separate load/save functions for each data type
3. **Dynamic Array Pattern**: Auto-resizing arrays prevent fixed limits

---

## 📊 Data Structures Deep Dive

### 1. DaySchedule Structure

**Purpose**: Represents a driver's availability for a single day of the week.

```c
typedef struct {
    char start_time[10];  // Format: "HH:MM" (e.g., "08:30")
    char end_time[10];    // Format: "HH:MM" (e.g., "17:00")
    int is_off_day;       // 1 = driver not available, 0 = available
} DaySchedule;
```

**Usage Example**:

```c
// Monday: Available from 8:00 AM to 5:00 PM
DaySchedule monday = {
    .start_time = "08:00",
    .end_time = "17:00",
    .is_off_day = 0
};

// Tuesday: Not available
DaySchedule tuesday = {
    .start_time = "",
    .end_time = "",
    .is_off_day = 1
};
```

**Memory Layout**:

```
┌─────────────┬─────────────┬─────────────┐
│ start_time  │  end_time   │ is_off_day  │
│  (10 bytes) │ (10 bytes)  │  (4 bytes)  │
└─────────────┴─────────────┴─────────────┘
Total: 24 bytes per DaySchedule
```

---

### 2. Driver Structure

**Purpose**: Complete profile of a driver offering rides.

```c
typedef struct {
    char name[50];              // Full name
    char roll[20];              // Student roll number
    char section[10];           // Class section
    char address[100];          // Home address
    char email[50];             // Unique identifier & login
    char phone[15];             // Contact number
    char password[30];          // Login credential
    char car_plate[20];         // Vehicle registration
    int total_seats;            // Total capacity
    int available_seats;        // Remaining seats
    int route_count;            // Number of stops (0-10)
    int is_flexible;            // 0=Fixed, 1=Flexible
    int is_active;              // 1=Active, 0=Deleted
    DaySchedule timetable[5];   // Mon-Fri schedule
    char route[10][100];        // Up to 10 stops
} Driver;
```

**Field Breakdown**:

| Field             | Type           | Size       | Purpose            | Example           |
| ----------------- | -------------- | ---------- | ------------------ | ----------------- |
| `name`            | char[]         | 50 bytes   | Driver's full name | "Ahmed Khan"      |
| `roll`            | char[]         | 20 bytes   | Student ID         | "21K-1234"        |
| `section`         | char[]         | 10 bytes   | Class section      | "BCS-3A"          |
| `address`         | char[]         | 100 bytes  | Pickup location    | "123 Model Town"  |
| `email`           | char[]         | 50 bytes   | Login & identifier | "ahmed@uni.edu"   |
| `phone`           | char[]         | 15 bytes   | Contact            | "+92-300-1234567" |
| `password`        | char[]         | 30 bytes   | Authentication     | "mypass123"       |
| `car_plate`       | char[]         | 20 bytes   | Vehicle ID         | "ABC-123"         |
| `total_seats`     | int            | 4 bytes    | Car capacity       | 4                 |
| `available_seats` | int            | 4 bytes    | Open seats         | 2                 |
| `route_count`     | int            | 4 bytes    | Number of stops    | 5                 |
| `is_flexible`     | int            | 4 bytes    | Ride type flag     | 0 or 1            |
| `is_active`       | int            | 4 bytes    | Account status     | 1                 |
| `timetable`       | DaySchedule[5] | 120 bytes  | Weekly schedule    | See above         |
| `route`           | char[10][100]  | 1000 bytes | Route stops        | See below         |

**Total Size**: ~1,400 bytes per Driver

**Route Example**:

```c
Driver d;
d.route_count = 5;
strcpy(d.route[0], "University Main Gate");
strcpy(d.route[1], "Mall Road");
strcpy(d.route[2], "Liberty Market");
strcpy(d.route[3], "Gulberg");
strcpy(d.route[4], "Model Town");
```

**Visual Representation**:

```
Driver: Ahmed Khan
Route: University ← Mall Road ← Liberty ← Gulberg ← Model Town
       (Stop 0)     (Stop 1)     (Stop 2)   (Stop 3)  (Stop 4)

Timetable:
  Mon: 08:00 - 17:00 ✓
  Tue: 08:00 - 17:00 ✓
  Wed: OFF            ✗
  Thu: 08:00 - 17:00 ✓
  Fri: 08:00 - 14:00 ✓
```

---

### 3. Passenger Structure

**Purpose**: Profile of a student looking for rides.

```c
typedef struct {
    char name[50];
    char roll[20];
    char section[10];
    char address[100];
    char email[50];
    char phone[15];
    char password[30];
    int has_timetable;      // Currently unused
    int is_active;
    DaySchedule timetable[5]; // Currently unused
} Passenger;
```

**Key Differences from Driver**:

- No vehicle information (car_plate, seats)
- No route information
- Simpler structure (~400 bytes vs ~1400 bytes)
- `has_timetable` field reserved for future features

---

### 4. FlexibleRide Structure

**Purpose**: Represents a one-time ride posted by a flexible driver.

```c
typedef struct {
    char driver_email[50];    // Who's offering the ride
    char date[20];            // "DD/MM/YYYY"
    char day[20];             // "Monday", "Tuesday", etc.
    char time[10];            // "HH:MM"
    int direction;            // 0=To Uni, 1=From Uni
    int stop_count;           // Number of stops
    int available_seats;      // Seats available
    int ride_id;              // Unique identifier
    int is_active;            // Active/cancelled
    char stops[10][100];      // Route stops
} FlexibleRide;
```

**Example**:

```c
FlexibleRide ride = {
    .driver_email = "ahmed@uni.edu",
    .date = "25/11/2025",
    .day = "Monday",
    .time = "08:00",
    .direction = 0,  // To University
    .stop_count = 3,
    .available_seats = 3,
    .ride_id = 1001,
    .is_active = 1
};
strcpy(ride.stops[0], "Model Town");
strcpy(ride.stops[1], "Liberty Market");
strcpy(ride.stops[2], "University");
```

**Direction Encoding**:

```
direction = 0  →  To University (Morning commute)
direction = 1  →  From University (Evening return)
```

---

### 5. Match Structure

**Purpose**: Links a passenger to a driver's ride.

```c
typedef struct {
    char driver_email[50];
    char passenger_email[50];
    int is_semester_long;  // 1=Fixed ride, 0=Flexible ride
    int is_active;         // 1=Active, 0=Cancelled
} Match;
```

**Example Matches**:

```c
// Fixed semester-long match
Match m1 = {
    .driver_email = "ahmed@uni.edu",
    .passenger_email = "sara@uni.edu",
    .is_semester_long = 1,
    .is_active = 1
};

// One-time flexible ride match
Match m2 = {
    .driver_email = "ali@uni.edu",
    .passenger_email = "zain@uni.edu",
    .is_semester_long = 0,
    .is_active = 1
};
```

**Relationship Diagram**:

```
Driver (ahmed@uni.edu)
    ├─ Match 1 → Passenger (sara@uni.edu)   [Semester-long]
    ├─ Match 2 → Passenger (fatima@uni.edu) [Semester-long]
    └─ Match 3 → Passenger (usman@uni.edu)  [Flexible]
```

---

### 6. Global Application Structure

**Purpose**: Central data repository for the entire application.

```c
struct {
    // Counters
    int driver_count;
    int passenger_count;
    int flex_ride_count;
    int match_count;

    // Dynamic arrays
    Driver *drivers;
    Passenger *passengers;
    FlexibleRide *flex_rides;
    Match *matches;

    // Capacity tracking
    int driver_capacity;
    int passenger_capacity;
    int ride_capacity;
    int match_capacity;

    // Current session
    int current_driver_idx;
    int current_passenger_idx;
} app = {0};
```

**Memory Growth Example**:

```
Initial State:
  drivers: [capacity=10, count=0]
  passengers: [capacity=10, count=0]

After 10 drivers register:
  drivers: [capacity=10, count=10] → FULL!

After 11th driver registers:
  resize_drivers() called
  drivers: [capacity=20, count=11] → Doubled!

After 20 drivers register:
  drivers: [capacity=20, count=20] → FULL!

After 21st driver registers:
  resize_drivers() called
  drivers: [capacity=40, count=21] → Doubled again!
```

---

## 🧠 Memory Management

### Dynamic Memory Allocation Strategy

The system uses **dynamic arrays** that grow automatically, preventing fixed limits while maintaining memory efficiency.

### Initialization Function

```c
void init_dynamic_arrays() {
    // Set initial capacities
    app.driver_capacity = 10;
    app.passenger_capacity = 10;
    app.ride_capacity = 10;
    app.match_capacity = 20;

    // Allocate memory
    app.drivers = (Driver *)malloc(app.driver_capacity * sizeof(Driver));
    app.passengers = (Passenger *)malloc(app.passenger_capacity * sizeof(Passenger));
    app.flex_rides = (FlexibleRide *)malloc(app.ride_capacity * sizeof(FlexibleRide));
    app.matches = (Match *)malloc(app.match_capacity * sizeof(Match));

    // Zero out memory
    memset(app.drivers, 0, app.driver_capacity * sizeof(Driver));
    memset(app.passengers, 0, app.passenger_capacity * sizeof(Passenger));
    memset(app.flex_rides, 0, app.ride_capacity * sizeof(FlexibleRide));
    memset(app.matches, 0, app.match_capacity * sizeof(Match));
}
```

**Why `memset`?**

- Initializes all bytes to 0
- Ensures strings are null-terminated
- Sets all integers to 0
- Prevents garbage values

### Resize Functions

**Pattern**: All resize functions follow the same strategy - **double the capacity**.

```c
void resize_drivers() {
    app.driver_capacity *= 2;  // Double capacity
    app.drivers = (Driver *)realloc(app.drivers,
                                    app.driver_capacity * sizeof(Driver));
}
```

**Resize Trigger Example**:

```c
void register_driver() {
    // Check if array is full
    if (app.driver_count >= app.driver_capacity)
        resize_drivers();  // Expand before adding

    // Now safe to add new driver
    Driver *d = &app.drivers[app.driver_count];
    // ... collect driver info ...
    app.driver_count++;
}
```

**Memory Growth Visualization**:

```
Capacity: 10 → 20 → 40 → 80 → 160 → 320 → ...
          ×2   ×2   ×2   ×2    ×2     ×2
```

### Memory Cleanup

```c
void free_all_memory() {
    free(app.drivers);
    free(app.passengers);
    free(app.flex_rides);
    free(app.matches);
}
```

**Called at program exit**:

```c
int main() {
    init_dynamic_arrays();
    // ... program runs ...
    save_all_data();
    free_all_memory();  // Clean up before exit
    return 0;
}
```

---

## 💾 File Handling System

### File Structure Overview

```
Project Directory/
├── corelogic.c          (Source code)
├── drivers.txt          (Driver database)
├── passengers.txt       (Passenger database)
├── flex_rides.txt       (Flexible rides)
├── matches.txt          (Driver-Passenger links)
└── admin.txt            (Admin credentials)
```

### File Format Specifications

#### 1. drivers.txt

**Format**: Pipe-delimited (`|`) with semicolon-separated (`;`) routes

```
name|roll|section|address|email|phone|password|car_plate|total_seats|available_seats|route_count|is_flexible|is_active|route_stops
```

**Example Entry**:

```
Ahmed Khan|21K-1234|BCS-3A|Model Town|ahmed@uni.edu|+92-300-1234567|pass123|ABC-123|4|2|3|0|1|University;Mall Road;Model Town
```

**Parsing Code**:

```c
void load_drivers() {
    FILE *f = fopen("drivers.txt", "r");
    if (!f) return;  // File doesn't exist yet

    char line[2000];
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = 0;  // Remove newline

        Driver *d = &app.drivers[app.driver_count];
        char route_str[1000] = "";

        // Parse main fields
        sscanf(line, "%49[^|]|%19[^|]|%9[^|]|%99[^|]|%49[^|]|%14[^|]|%29[^|]|%19[^|]|%d|%d|%d|%d|%d|%999[^|]",
               d->name, d->roll, d->section, d->address, d->email, d->phone,
               d->password, d->car_plate, &d->total_seats, &d->available_seats,
               &d->route_count, &d->is_flexible, &d->is_active, route_str);

        // Parse route stops (semicolon-separated)
        char *route_ptr = route_str;
        for (int i = 0; i < d->route_count && i < MAX_STOPS; i++) {
            char *delim = strchr(route_ptr, ';');
            if (delim) {
                int len = delim - route_ptr;
                strncpy(d->route[i], route_ptr, len);
                d->route[i][len] = 0;
                route_ptr = delim + 1;
            } else {
                strcpy(d->route[i], route_ptr);
                break;
            }
        }

        app.driver_count++;
    }
    fclose(f);
}
```

**Key Parsing Techniques**:

1. `%49[^|]` - Read up to 49 chars until `|` is found
2. `strchr(route_ptr, ';')` - Find semicolon delimiter
3. `strncpy` - Copy substring safely
4. `strcspn(line, "\n")` - Find newline position

#### 2. passengers.txt

**Format**:

```
name|roll|section|address|email|phone|password|has_timetable|is_active
```

**Example**:

```
Sara Ali|21K-5678|BCS-3B|Gulberg|sara@uni.edu|+92-301-9876543|sara123|0|1
```

#### 3. flex_rides.txt

**Format**:

```
driver_email|date|day|time|direction|stop_count|available_seats|ride_id|is_active|stops
```

**Example**:

```
ahmed@uni.edu|25/11/2025|Monday|08:00|0|3|3|1001|1|Model Town;Liberty;University
```

#### 4. matches.txt

**Format**:

```
driver_email|passenger_email|is_semester_long|is_active
```

**Example**:

```
ahmed@uni.edu|sara@uni.edu|1|1
ali@uni.edu|zain@uni.edu|0|1
```

#### 5. admin.txt

**Format**: Space-separated

```
email password
```

**Default**:

```
admin@carpool.com admin123
```

### Save Functions

**Pattern**: All save functions follow similar structure.

```c
void save_drivers() {
    FILE *f = fopen("drivers.txt", "w");
    if (!f) return;

    for (int i = 0; i < app.driver_count; i++) {
        Driver *d = &app.drivers[i];

        // Build route string
        char route_str[1000] = "";
        for (int j = 0; j < d->route_count; j++) {
            strcat(route_str, d->route[j]);
            if (j < d->route_count - 1)
                strcat(route_str, ";");
        }

        // Write formatted line
        fprintf(f, "%s|%s|%s|%s|%s|%s|%s|%s|%d|%d|%d|%d|%d|%s\n",
                d->name, d->roll, d->section, d->address, d->email,
                d->phone, d->password, d->car_plate, d->total_seats,
                d->available_seats, d->route_count, d->is_flexible,
                d->is_active, route_str);
    }
    fclose(f);
}
```

### Batch Operations

```c
void load_all_data() {
    load_drivers();
    load_passengers();
    load_flex_rides();
    load_matches();
}

void save_all_data() {
    save_drivers();
    save_passengers();
    save_flex_rides();
    save_matches();
}
```

**Called at**:

- Program startup: `load_all_data()`
- After any modification: `save_all_data()`
- Program exit: `save_all_data()`

---

## 📚 Complete Function Reference

### Function Categories

The system contains **36 functions** organized into 8 categories:

| Category             | Count | Functions                                                                                                                                                                    |
| -------------------- | ----- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Memory Management    | 6     | `init_dynamic_arrays`, `resize_drivers`, `resize_passengers`, `resize_rides`, `resize_matches`, `free_all_memory`                                                            |
| File I/O             | 10    | `load_drivers`, `save_drivers`, `load_passengers`, `save_passengers`, `load_flex_rides`, `save_flex_rides`, `load_matches`, `save_matches`, `load_all_data`, `save_all_data` |
| Search & Lookup      | 3     | `find_driver_by_email`, `find_passenger_by_email`, `get_next_ride_id`                                                                                                        |
| Authentication       | 1     | `check_admin_login`                                                                                                                                                          |
| UI Helpers           | 2     | `clear_screen`, `pause_screen`                                                                                                                                               |
| Registration         | 2     | `register_driver`, `register_passenger`                                                                                                                                      |
| Driver Operations    | 4     | `add_route`, `add_timetable`, `post_flexible_ride`, `view_my_passengers`                                                                                                     |
| Passenger Operations | 2     | `search_rides`, `view_my_rides`                                                                                                                                              |
| Dashboards           | 4     | `driver_dashboard`, `passenger_dashboard`, `admin_dashboard`, `login`                                                                                                        |
| Main                 | 1     | `main`                                                                                                                                                                       |

### Complete Function Table

| #   | Function Name             | Return Type | Parameters                     | Lines   | Purpose                                                |
| --- | ------------------------- | ----------- | ------------------------------ | ------- | ------------------------------------------------------ |
| 1   | `init_dynamic_arrays`     | void        | none                           | 52-66   | Initialize all dynamic arrays with malloc              |
| 2   | `resize_drivers`          | void        | none                           | 68-72   | Double driver array capacity                           |
| 3   | `resize_passengers`       | void        | none                           | 74-78   | Double passenger array capacity                        |
| 4   | `resize_rides`            | void        | none                           | 80-84   | Double flexible rides array capacity                   |
| 5   | `resize_matches`          | void        | none                           | 86-90   | Double matches array capacity                          |
| 6   | `free_all_memory`         | void        | none                           | 92-98   | Free all allocated memory                              |
| 7   | `load_drivers`            | void        | none                           | 100-144 | Load drivers from drivers.txt                          |
| 8   | `save_drivers`            | void        | none                           | 146-169 | Save drivers to drivers.txt                            |
| 9   | `load_passengers`         | void        | none                           | 171-196 | Load passengers from passengers.txt                    |
| 10  | `save_passengers`         | void        | none                           | 198-212 | Save passengers to passengers.txt                      |
| 11  | `load_flex_rides`         | void        | none                           | 214-258 | Load flexible rides from flex_rides.txt                |
| 12  | `save_flex_rides`         | void        | none                           | 260-283 | Save flexible rides to flex_rides.txt                  |
| 13  | `load_matches`            | void        | none                           | 285-308 | Load matches from matches.txt                          |
| 14  | `save_matches`            | void        | none                           | 310-323 | Save matches to matches.txt                            |
| 15  | `load_all_data`           | void        | none                           | 325-331 | Load all data files                                    |
| 16  | `save_all_data`           | void        | none                           | 333-339 | Save all data files                                    |
| 17  | `find_driver_by_email`    | int         | const char \*email             | 341-349 | Find driver index by email, returns -1 if not found    |
| 18  | `find_passenger_by_email` | int         | const char \*email             | 351-359 | Find passenger index by email, returns -1 if not found |
| 19  | `get_next_ride_id`        | int         | none                           | 361-365 | Generate unique ride ID using static counter           |
| 20  | `check_admin_login`       | int         | const char *em, const char *pw | 367-381 | Verify admin credentials, returns 1 if valid           |
| 21  | `clear_screen`            | void        | none                           | 383-386 | Clear console screen (Windows: cls)                    |
| 22  | `pause_screen`            | void        | none                           | 388-393 | Wait for user to press Enter                           |
| 23  | `register_driver`         | void        | none                           | 395-429 | Register new driver with input collection              |
| 24  | `register_passenger`      | void        | none                           | 431-457 | Register new passenger with input collection           |
| 25  | `add_route`               | void        | none                           | 459-480 | Add/edit route for current driver                      |
| 26  | `add_timetable`           | void        | none                           | 482-515 | Add/edit weekly schedule for current driver            |
| 27  | `post_flexible_ride`      | void        | none                           | 517-549 | Post one-time flexible ride                            |
| 28  | `view_my_passengers`      | void        | none                           | 551-570 | Display passengers matched with current driver         |
| 29  | `driver_dashboard`        | void        | none                           | 572-618 | Main menu for logged-in drivers                        |
| 30  | `search_rides`            | void        | none                           | 620-749 | Search and book rides (passenger function)             |
| 31  | `view_my_rides`           | void        | none                           | 751-815 | View booked rides and co-passengers                    |
| 32  | `passenger_dashboard`     | void        | none                           | 817-840 | Main menu for logged-in passengers                     |
| 33  | `admin_dashboard`         | void        | none                           | 842-896 | Main menu for admin with view-all functions            |
| 34  | `login`                   | void        | none                           | 898-928 | Handle login for all user types                        |
| 35  | `main`                    | int         | none                           | 930-978 | Program entry point                                    |

### Function Call Hierarchy

```
main()
├── init_dynamic_arrays()
├── load_all_data()
│   ├── load_drivers()
│   ├── load_passengers()
│   ├── load_flex_rides()
│   └── load_matches()
├── register_driver()
│   ├── resize_drivers() [if needed]
│   └── save_all_data()
├── register_passenger()
│   ├── resize_passengers() [if needed]
│   └── save_all_data()
├── login()
│   ├── check_admin_login()
│   ├── find_driver_by_email()
│   ├── find_passenger_by_email()
│   ├── admin_dashboard()
│   │   ├── clear_screen()
│   │   └── pause_screen()
│   ├── driver_dashboard()
│   │   ├── add_route()
│   │   │   ├── clear_screen()
│   │   │   ├── save_all_data()
│   │   │   └── pause_screen()
│   │   ├── add_timetable()
│   │   │   ├── clear_screen()
│   │   │   ├── save_all_data()
│   │   │   └── pause_screen()
│   │   ├── post_flexible_ride()
│   │   │   ├── get_next_ride_id()
│   │   │   ├── resize_rides() [if needed]
│   │   │   ├── save_all_data()
│   │   │   └── pause_screen()
│   │   └── view_my_passengers()
│   │       ├── find_passenger_by_email()
│   │       └── pause_screen()
│   └── passenger_dashboard()
│       ├── search_rides()
│       │   ├── find_driver_by_email()
│       │   ├── resize_matches() [if needed]
│       │   ├── save_all_data()
│       │   └── pause_screen()
│       └── view_my_rides()
│           ├── find_driver_by_email()
│           ├── find_passenger_by_email()
│           └── pause_screen()
├── save_all_data()
│   ├── save_drivers()
│   ├── save_passengers()
│   ├── save_flex_rides()
│   └── save_matches()
└── free_all_memory()
```

### Functions by Complexity

#### Simple Functions (1-10 lines)

- `clear_screen()` - 4 lines
- `pause_screen()` - 6 lines
- `get_next_ride_id()` - 5 lines
- `resize_drivers()` - 5 lines
- `resize_passengers()` - 5 lines
- `resize_rides()` - 5 lines
- `resize_matches()` - 5 lines
- `free_all_memory()` - 7 lines
- `load_all_data()` - 7 lines
- `save_all_data()` - 7 lines

#### Medium Functions (11-50 lines)

- `init_dynamic_arrays()` - 15 lines
- `find_driver_by_email()` - 9 lines
- `find_passenger_by_email()` - 9 lines
- `check_admin_login()` - 15 lines
- `save_drivers()` - 24 lines
- `save_passengers()` - 15 lines
- `save_flex_rides()` - 24 lines
- `save_matches()` - 14 lines
- `register_driver()` - 35 lines
- `register_passenger()` - 27 lines
- `add_route()` - 22 lines
- `add_timetable()` - 34 lines
- `post_flexible_ride()` - 33 lines
- `view_my_passengers()` - 20 lines
- `driver_dashboard()` - 47 lines
- `passenger_dashboard()` - 24 lines
- `login()` - 31 lines
- `main()` - 49 lines

#### Complex Functions (51+ lines)

- `load_drivers()` - 45 lines (complex parsing)
- `load_passengers()` - 26 lines
- `load_flex_rides()` - 45 lines (complex parsing)
- `load_matches()` - 24 lines
- `search_rides()` - **130 lines** (most complex function)
- `view_my_rides()` - 65 lines
- `admin_dashboard()` - 55 lines

---

## ⚙️ Core Functions Explained

### Search & Lookup Functions

#### find_driver_by_email()

```c
int find_driver_by_email(const char *email) {
    for (int i = 0; i < app.driver_count; i++) {
        if (app.drivers[i].is_active &&
            strcmp(app.drivers[i].email, email) == 0)
            return i;
    }
    return -1;  // Not found
}
```

**Usage**:

```c
int idx = find_driver_by_email("ahmed@uni.edu");
if (idx >= 0) {
    printf("Found: %s\n", app.drivers[idx].name);
} else {
    printf("Driver not found\n");
}
```

**Why check `is_active`?**

- Deleted accounts have `is_active = 0`
- Prevents accessing "soft-deleted" users
- Maintains data integrity

#### find_passenger_by_email()

```c
int find_passenger_by_email(const char *email) {
    for (int i = 0; i < app.passenger_count; i++) {
        if (app.passengers[i].is_active &&
            strcmp(app.passengers[i].email, email) == 0)
            return i;
    }
    return -1;
}
```

### ID Generation

```c
int get_next_ride_id() {
    static int id = 1;  // Persists between calls
    return id++;
}
```

**How it works**:

```c
int ride1 = get_next_ride_id();  // Returns 1
int ride2 = get_next_ride_id();  // Returns 2
int ride3 = get_next_ride_id();  // Returns 3
```

**Static variable behavior**:

- Initialized once
- Retains value between function calls
- Not reset when function exits

### Authentication

```c
int check_admin_login(const char *em, const char *pw) {
    FILE *f = fopen("admin.txt", "r");
    if (f) {
        char e[50], p[30];
        if (fscanf(f, "%s %s", e, p) == 2 &&
            !strcmp(em, e) && !strcmp(pw, p)) {
            fclose(f);
            return 1;  // Valid admin
        }
        fclose(f);
    }
    return 0;  // Invalid
}
```

### UI Helper Functions

```c
void clear_screen() {
    system("cls");  // Windows-specific
}

void pause_screen() {
    printf("\nPress Enter to continue...");
    getchar();  // Consume leftover newline
    getchar();  // Wait for user input
}
```

**Why two `getchar()`?**

```c
scanf("%d", &choice);  // Leaves '\n' in buffer
getchar();             // Consumes that '\n'
getchar();             // Waits for user to press Enter
```

---

## 🎛️ Dashboard Functions Deep Dive

### Overview

The system has **4 dashboard functions** that serve as the main interfaces for different user types:

1. `driver_dashboard()` - For logged-in drivers
2. `passenger_dashboard()` - For logged-in passengers
3. `admin_dashboard()` - For system administrators
4. `login()` - Authentication gateway

### driver_dashboard()

**Lines**: 572-618 (47 lines)  
**Purpose**: Main menu for drivers after successful login  
**Behavior**: Different menu options based on driver type (Fixed vs Flexible)

**Complete Code**:

```c
void driver_dashboard() {
    int choice;
    do {
        clear_screen();
        printf("=== DRIVER DASHBOARD ===\n");
        printf("Welcome, %s!\n", app.drivers[app.current_driver_idx].name);
        printf("Type: %s\n\n", app.drivers[app.current_driver_idx].is_flexible ? "Flexible" : "Fixed");

        if (!app.drivers[app.current_driver_idx].is_flexible) {
            // FIXED DRIVER MENU
            printf("1. Add/Edit Route\n");
            printf("2. Add/Edit Timetable\n");
            printf("3. View My Passengers\n");
        } else {
            // FLEXIBLE DRIVER MENU
            printf("1. Post Flexible Ride\n");
            printf("2. View My Passengers\n");
        }

        printf("0. Logout\n\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                if (!app.drivers[app.current_driver_idx].is_flexible)
                    add_route();
                else
                    post_flexible_ride();
                break;
            case 2:
                if (!app.drivers[app.current_driver_idx].is_flexible)
                    add_timetable();
                else
                    view_my_passengers();
                break;
            case 3:
                if (!app.drivers[app.current_driver_idx].is_flexible)
                    view_my_passengers();
                break;
        }
    } while (choice != 0);
}
```

**Key Features**:

1. **Dynamic Menu**: Changes based on `is_flexible` flag
2. **Personalized Greeting**: Shows driver's name
3. **Type Display**: Shows "Fixed" or "Flexible"
4. **Loop Until Logout**: Keeps showing menu until user selects 0

**Menu Mapping**:

| Choice | Fixed Driver       | Flexible Driver    |
| ------ | ------------------ | ------------------ |
| 1      | Add/Edit Route     | Post Flexible Ride |
| 2      | Add/Edit Timetable | View My Passengers |
| 3      | View My Passengers | (Invalid)          |
| 0      | Logout             | Logout             |

**Flow Diagram**:

```
driver_dashboard()
    │
    ├─ Display welcome message
    ├─ Check is_flexible flag
    │   │
    │   ├─ If Fixed (is_flexible = 0):
    │   │   ├─ Option 1 → add_route()
    │   │   ├─ Option 2 → add_timetable()
    │   │   └─ Option 3 → view_my_passengers()
    │   │
    │   └─ If Flexible (is_flexible = 1):
    │       ├─ Option 1 → post_flexible_ride()
    │       └─ Option 2 → view_my_passengers()
    │
    └─ Loop until choice = 0
```

---

### passenger_dashboard()

**Lines**: 817-840 (24 lines)  
**Purpose**: Main menu for passengers after successful login  
**Behavior**: Simple menu with two main functions

**Complete Code**:

```c
void passenger_dashboard() {
    int choice;
    do {
        clear_screen();
        printf("=== PASSENGER DASHBOARD ===\n");
        printf("Welcome, %s!\n\n", app.passengers[app.current_passenger_idx].name);
        printf("1. Search Rides\n");
        printf("2. View My Rides & Co-Passengers\n");
        printf("0. Logout\n\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                search_rides();
                break;
            case 2:
                view_my_rides();
                break;
        }
    } while (choice != 0);
}
```

**Menu Options**:

| Choice | Function          | Purpose                             |
| ------ | ----------------- | ----------------------------------- |
| 1      | `search_rides()`  | Search for available rides and book |
| 2      | `view_my_rides()` | View booked rides and co-passengers |
| 0      | Logout            | Return to main menu                 |

**Simpler Than Driver Dashboard**:

- No conditional menu (same for all passengers)
- Only 2 main functions
- Shorter code (24 vs 47 lines)

---

### admin_dashboard()

**Lines**: 842-896 (55 lines)  
**Purpose**: Administrative interface for viewing system data  
**Behavior**: Read-only view of all drivers, passengers, and matches

**Complete Code**:

```c
void admin_dashboard() {
    int choice;
    do {
        clear_screen();
        printf("=== ADMIN DASHBOARD ===\n\n");
        printf("1. View All Drivers\n");
        printf("2. View All Passengers\n");
        printf("3. View All Matches\n");
        printf("0. Logout\n\n");
        printf("Choice: ");
        scanf("%d", &choice);
        clear_screen();

        switch (choice) {
            case 1:
                printf("=== ALL DRIVERS ===\n\n");
                for (int i = 0; i < app.driver_count; i++) {
                    if (app.drivers[i].is_active) {
                        printf("%d. %s | %s | %s | Ph:%s | Seats:%d/%d | Type:%s | Plate:%s\n",
                               i + 1, app.drivers[i].name, app.drivers[i].email,
                               app.drivers[i].address, app.drivers[i].phone,
                               app.drivers[i].available_seats, app.drivers[i].total_seats,
                               app.drivers[i].is_flexible ? "Flex" : "Fixed",
                               app.drivers[i].car_plate);
                    }
                }
                pause_screen();
                break;

            case 2:
                printf("=== ALL PASSENGERS ===\n\n");
                for (int i = 0; i < app.passenger_count; i++) {
                    if (app.passengers[i].is_active) {
                        printf("%d. %s | %s | %s | Ph:%s | Addr:%s\n",
                               i + 1, app.passengers[i].name, app.passengers[i].email,
                               app.passengers[i].roll, app.passengers[i].phone,
                               app.passengers[i].address);
                    }
                }
                pause_screen();
                break;

            case 3:
                printf("=== ALL MATCHES ===\n\n");
                for (int i = 0; i < app.match_count; i++) {
                    if (app.matches[i].is_active)
                        printf("%d. %s <-> %s\n", i + 1,
                               app.matches[i].driver_email,
                               app.matches[i].passenger_email);
                }
                pause_screen();
                break;
        }
    } while (choice != 0);
}
```

**Menu Options**:

| Choice | Display        | Information Shown                               |
| ------ | -------------- | ----------------------------------------------- |
| 1      | All Drivers    | Name, Email, Address, Phone, Seats, Type, Plate |
| 2      | All Passengers | Name, Email, Roll, Phone, Address               |
| 3      | All Matches    | Driver Email ↔ Passenger Email                  |
| 0      | Logout         | Return to main menu                             |

**Key Characteristics**:

1. **Read-Only**: No modification capabilities
2. **Comprehensive View**: Shows all system data
3. **Active Filter**: Only displays `is_active = 1` records
4. **Simple Format**: Easy-to-read list format

**Sample Output**:

```
=== ALL DRIVERS ===

1. Ahmed Khan | ahmed@uni.edu | Model Town | Ph:+92-300-1234567 | Seats:2/4 | Type:Fixed | Plate:ABC-123
2. Ali Raza | ali@uni.edu | Gulberg | Ph:+92-301-9876543 | Seats:3/3 | Type:Flex | Plate:XYZ-789

=== ALL PASSENGERS ===

1. Sara Ali | sara@uni.edu | 21K-5678 | Ph:+92-302-1111111 | Addr:Liberty
2. Fatima Khan | fatima@uni.edu | 21K-9999 | Ph:+92-303-2222222 | Addr:DHA

=== ALL MATCHES ===

1. ahmed@uni.edu <-> sara@uni.edu
2. ahmed@uni.edu <-> fatima@uni.edu
3. ali@uni.edu <-> zain@uni.edu
```

---

### login()

**Lines**: 898-928 (31 lines)  
**Purpose**: Authentication gateway for all user types  
**Behavior**: Checks credentials and redirects to appropriate dashboard

**Complete Code**:

```c
void login() {
    clear_screen();
    printf("=== LOGIN ===\n\n");

    char email[50], password[30];
    printf("Email: ");
    scanf("%s", email);
    printf("Password: ");
    scanf("%s", password);

    // Check admin first
    if (check_admin_login(email, password)) {
        admin_dashboard();
        return;
    }

    // Check driver
    int idx = find_driver_by_email(email);
    if (idx >= 0 && strcmp(app.drivers[idx].password, password) == 0) {
        app.current_driver_idx = idx;
        driver_dashboard();
        return;
    }

    // Check passenger
    idx = find_passenger_by_email(email);
    if (idx >= 0 && strcmp(app.passengers[idx].password, password) == 0) {
        app.current_passenger_idx = idx;
        passenger_dashboard();
        return;
    }

    // Failed
    printf("\n✗ Invalid credentials!\n");
    pause_screen();
}
```

**Authentication Flow**:

```
login()
    │
    ├─ Get email and password
    │
    ├─ 1. Check Admin
    │   └─ If valid → admin_dashboard() → RETURN
    │
    ├─ 2. Check Driver
    │   ├─ find_driver_by_email()
    │   ├─ Compare password
    │   └─ If valid → Set current_driver_idx → driver_dashboard() → RETURN
    │
    ├─ 3. Check Passenger
    │   ├─ find_passenger_by_email()
    │   ├─ Compare password
    │   └─ If valid → Set current_passenger_idx → passenger_dashboard() → RETURN
    │
    └─ 4. All Failed
        └─ Show error message
```

**Priority Order**:

1. **Admin** (highest priority)
2. **Driver**
3. **Passenger**
4. **Invalid** (if none match)

**Why This Order?**

- Admin checked first for security
- Drivers before passengers (arbitrary)
- Early return prevents unnecessary checks

**Session Management**:

```c
// For drivers
app.current_driver_idx = idx;  // Stores which driver is logged in

// For passengers
app.current_passenger_idx = idx;  // Stores which passenger is logged in
```

**Security Note**:

- Passwords compared with `strcmp()` (plain text)
- No encryption or hashing
- Vulnerable to file access attacks

---

### Dashboard Comparison Table

| Feature                | Driver           | Passenger | Admin  | Login           |
| ---------------------- | ---------------- | --------- | ------ | --------------- |
| **Lines of Code**      | 47               | 24        | 55     | 31              |
| **Menu Options**       | 3-4 (dynamic)    | 2         | 3      | N/A             |
| **Conditional Logic**  | Yes (Fixed/Flex) | No        | No     | Yes (user type) |
| **Data Modification**  | Yes              | Yes       | No     | No              |
| **Loops Through Data** | No               | No        | Yes    | No              |
| **Uses Current Index** | Yes              | Yes       | No     | Sets index      |
| **Complexity**         | Medium           | Low       | Medium | Medium          |

---

## 🔄 Program Flow & Algorithms

### Main Program Flow

```c
int main() {
    // 1. Initialize
    init_dynamic_arrays();
    load_all_data();

    // 2. Create admin if needed
    FILE *f = fopen("admin.txt", "r");
    if (!f) {
        f = fopen("admin.txt", "w");
        fprintf(f, "admin@carpool.com admin123");
        fclose(f);
    } else {
        fclose(f);
    }

    // 3. Main menu loop
    int choice;
    do {
        clear_screen();
        printf("========================================\n");
        printf("   UNIVERSITY CARPOOLING SYSTEM\n");
        printf("========================================\n\n");
        printf("1. Register as Driver\n");
        printf("2. Register as Passenger\n");
        printf("3. Login\n");
        printf("0. Exit\n\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: register_driver(); break;
            case 2: register_passenger(); break;
            case 3: login(); break;
            case 0:
                save_all_data();
                free_all_memory();
                printf("\nGoodbye!\n");
                break;
        }
    } while (choice != 0);

    return 0;
}
```

### Registration Flow

#### Driver Registration

```c
void register_driver() {
    clear_screen();
    printf("=== DRIVER REGISTRATION ===\n\n");

    // Ensure capacity
    if (app.driver_count >= app.driver_capacity)
        resize_drivers();

    // Get pointer to new driver slot
    Driver *d = &app.drivers[app.driver_count];

    // Collect information
    printf("Name: ");
    scanf(" %[^\n]", d->name);  // Read with spaces

    printf("Roll Number: ");
    scanf("%s", d->roll);

    printf("Section: ");
    scanf("%s", d->section);

    printf("Address: ");
    scanf(" %[^\n]", d->address);

    printf("Email: ");
    scanf("%s", d->email);

    printf("Phone: ");
    scanf("%s", d->phone);

    printf("Password: ");
    scanf("%s", d->password);

    printf("Car Plate: ");
    scanf("%s", d->car_plate);

    printf("Total Seats: ");
    scanf("%d", &d->total_seats);

    // Initialize
    d->available_seats = d->total_seats;

    printf("Type (0=Fixed, 1=Flexible): ");
    scanf("%d", &d->is_flexible);

    d->is_active = 1;
    d->route_count = 0;

    // Save and confirm
    app.driver_count++;
    save_all_data();

    printf("\n✓ Driver registered successfully!\n");
    pause_screen();
}
```

**Key Points**:

- `scanf(" %[^\n]", d->name)` - Reads entire line including spaces
- `d->available_seats = d->total_seats` - Initially all seats available
- `d->is_active = 1` - Account active by default
- `save_all_data()` - Persist immediately

### Login Flow

```c
void login() {
    clear_screen();
    printf("=== LOGIN ===\n\n");

    char email[50], password[30];
    printf("Email: ");
    scanf("%s", email);
    printf("Password: ");
    scanf("%s", password);

    // Check admin
    if (check_admin_login(email, password)) {
        admin_dashboard();
        return;
    }

    // Check driver
    int idx = find_driver_by_email(email);
    if (idx >= 0 && strcmp(app.drivers[idx].password, password) == 0) {
        app.current_driver_idx = idx;
        driver_dashboard();
        return;
    }

    // Check passenger
    idx = find_passenger_by_email(email);
    if (idx >= 0 && strcmp(app.passengers[idx].password, password) == 0) {
        app.current_passenger_idx = idx;
        passenger_dashboard();
        return;
    }

    // Failed
    printf("\n✗ Invalid credentials!\n");
    pause_screen();
}
```

**Authentication Priority**:

1. Admin (highest priority)
2. Driver
3. Passenger
4. Invalid (if none match)

### Ride Search Algorithm

This is the most complex function in the system.

```c
void search_rides() {
    clear_screen();
    printf("=== SEARCH RIDES ===\n\n");

    int type;
    char stop[100];

    printf("Type (0=Fixed, 1=Flexible): ");
    scanf("%d", &type);

    printf("Stop/Location: ");
    scanf(" %[^\n]", stop);

    printf("\n--- RESULTS ---\n");
    int found = 0;

    // Arrays to track indices
    int driver_indices[100];
    int ride_indices[100];

    if (type == 0) {
        // FIXED RIDES SEARCH
        const char *days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

        for (int i = 0; i < app.driver_count; i++) {
            if (app.drivers[i].is_active &&
                !app.drivers[i].is_flexible &&
                app.drivers[i].available_seats > 0) {

                // Check each stop in route
                for (int j = 0; j < app.drivers[i].route_count; j++) {
                    if (strstr(app.drivers[i].route[j], stop)) {
                        // MATCH FOUND!
                        driver_indices[found] = i;

                        printf("\n%d. %s | Ph:%s | Seats:%d\n",
                               ++found,
                               app.drivers[i].name,
                               app.drivers[i].phone,
                               app.drivers[i].available_seats);

                        // Display route
                        printf("   Route: ");
                        for (int k = 0; k < app.drivers[i].route_count; k++)
                            printf("%s%s", app.drivers[i].route[k],
                                   k < app.drivers[i].route_count - 1 ? " -> " : "\n");

                        // Display weekly schedule
                        printf("   Weekly Schedule:\n");
                        for (int d = 0; d < MAX_DAYS; d++) {
                            if (app.drivers[i].timetable[d].is_off_day) {
                                printf("      %s: OFF\n", days[d]);
                            } else {
                                printf("      %s: %s - %s\n",
                                       days[d],
                                       app.drivers[i].timetable[d].start_time,
                                       app.drivers[i].timetable[d].end_time);
                            }
                        }
                        break;  // Don't show same driver twice
                    }
                }
            }
        }
    } else {
        // FLEXIBLE RIDES SEARCH
        for (int i = 0; i < app.flex_ride_count; i++) {
            if (app.flex_rides[i].is_active &&
                app.flex_rides[i].available_seats > 0) {

                // Check each stop
                for (int j = 0; j < app.flex_rides[i].stop_count; j++) {
                    if (strstr(app.flex_rides[i].stops[j], stop)) {
                        // MATCH FOUND!
                        int didx = find_driver_by_email(app.flex_rides[i].driver_email);
                        if (didx >= 0) {
                            ride_indices[found] = i;
                            driver_indices[found] = didx;

                            printf("\n%d. %s | Ph:%s | Day:%s | Date:%s | Time:%s | %s | Seats:%d\n",
                                   ++found,
                                   app.drivers[didx].name,
                                   app.drivers[didx].phone,
                                   app.flex_rides[i].day,
                                   app.flex_rides[i].date,
                                   app.flex_rides[i].time,
                                   app.flex_rides[i].direction ? "From Uni" : "To Uni",
                                   app.flex_rides[i].available_seats);

                            printf("   Stops: ");
                            for (int k = 0; k < app.flex_rides[i].stop_count; k++)
                                printf("%s%s", app.flex_rides[i].stops[k],
                                       k < app.flex_rides[i].stop_count - 1 ? " -> " : "\n");
                        }
                        break;
                    }
                }
            }
        }
    }

    if (!found) {
        printf("No rides found.\n");
    } else {
        // BOOKING LOGIC
        printf("\nBook ride number (0 to cancel): ");
        int book;
        scanf("%d", &book);

        if (book > 0 && book <= found) {
            if (app.match_count >= app.match_capacity)
                resize_matches();

            Match *m = &app.matches[app.match_count++];
            strcpy(m->passenger_email, app.passengers[app.current_passenger_idx].email);

            int selected_driver_idx = driver_indices[book - 1];
            strcpy(m->driver_email, app.drivers[selected_driver_idx].email);

            m->is_semester_long = !type;
            m->is_active = 1;

            // Decrease available seats
            app.drivers[selected_driver_idx].available_seats--;

            save_all_data();
            printf("\n✓ Ride booked successfully!\n");
        }
    }

    pause_screen();
}
```

**Algorithm Breakdown**:

1. **Input Collection**: Get ride type and location
2. **Search Phase**:
   - Loop through all drivers/rides
   - Check if active and has seats
   - Use `strstr()` for partial matching
   - Store matching indices
3. **Display Phase**:
   - Show all matches with details
   - Include route, schedule, contact info
4. **Booking Phase**:
   - User selects ride number
   - Create match record
   - Decrement available seats
   - Save to database

**Key Function**: `strstr(haystack, needle)`

```c
strstr("Mall Road", "Mall")    → Match found
strstr("Mall Road", "Road")    → Match found
strstr("Mall Road", "Liberty") → NULL (no match)
```

### View My Rides (Passenger)

```c
void view_my_rides() {
    clear_screen();
    printf("=== MY RIDES ===\n\n");
    int cnt = 0;

    for (int i = 0; i < app.match_count; i++) {
        if (app.matches[i].is_active &&
            strcmp(app.matches[i].passenger_email,
                   app.passengers[app.current_passenger_idx].email) == 0) {

            int didx = find_driver_by_email(app.matches[i].driver_email);
            if (didx >= 0) {
                printf("\n%d. %s | Driver: %s | Email: %s | Ph: %s | Seats:%d\n",
                       ++cnt,
                       app.matches[i].is_semester_long ? "Fixed" : "Flex",
                       app.drivers[didx].name,
                       app.drivers[didx].email,
                       app.drivers[didx].phone,
                       app.drivers[didx].available_seats);

                // Show route for fixed rides
                if (app.matches[i].is_semester_long && app.drivers[didx].route_count > 0) {
                    printf("   Route: ");
                    for (int k = 0; k < app.drivers[didx].route_count; k++)
                        printf("%s%s", app.drivers[didx].route[k],
                               k < app.drivers[didx].route_count - 1 ? " -> " : "\n");
                }

                // Show co-passengers
                printf("   Co-Passengers:\n");
                int co_passenger_count = 0;

                for (int j = 0; j < app.match_count; j++) {
                    if (app.matches[j].is_active &&
                        strcmp(app.matches[j].driver_email, app.matches[i].driver_email) == 0 &&
                        strcmp(app.matches[j].passenger_email,
                               app.passengers[app.current_passenger_idx].email) != 0) {

                        int pidx = find_passenger_by_email(app.matches[j].passenger_email);
                        if (pidx >= 0) {
                            printf("      • %s | Ph: %s | Pickup: %s\n",
                                   app.passengers[pidx].name,
                                   app.passengers[pidx].phone,
                                   app.passengers[pidx].address);
                            co_passenger_count++;
                        }
                    }
                }

                if (co_passenger_count == 0) {
                    printf("      No other passengers yet\n");
                }

                printf("\n");
            }
        }
    }

    if (!cnt)
        printf("No rides yet.\n");

    pause_screen();
}
```

**Co-Passenger Logic**:

1. Find all matches with same driver
2. Exclude current passenger
3. Display other passengers' info
4. Helps passengers coordinate pickups

---

## 📝 Code Examples & Snippets

### Example 1: Complete Driver Registration Flow

```c
// User runs program
// Selects option 1: Register as Driver

// System calls:
register_driver();

// Inside register_driver():
if (app.driver_count >= app.driver_capacity)
    resize_drivers();  // Expand if needed

Driver *d = &app.drivers[app.driver_count];

// User enters:
// Name: Ahmed Khan
// Roll: 21K-1234
// Section: BCS-3A
// Address: Model Town, Lahore
// Email: ahmed@uni.edu
// Phone: +92-300-1234567
// Password: ahmed123
// Car Plate: ABC-123
// Total Seats: 4
// Type: 0 (Fixed)

// System sets:
d->available_seats = 4;  // Same as total_seats
d->is_active = 1;
d->route_count = 0;

app.driver_count++;  // Increment counter
save_all_data();     // Save to drivers.txt

// drivers.txt now contains:
// Ahmed Khan|21K-1234|BCS-3A|Model Town, Lahore|ahmed@uni.edu|+92-300-1234567|ahmed123|ABC-123|4|4|0|0|1|
```

### Example 2: Adding Route and Timetable

```c
// Driver logs in
// Selects: Add/Edit Route

add_route();

// User enters:
// Stop 1: University Main Gate
// Stop 2: Mall Road
// Stop 3: Liberty Market
// Stop 4: Model Town
// Stop 5: done

// System stores:
d->route[0] = "University Main Gate";
d->route[1] = "Mall Road";
d->route[2] = "Liberty Market";
d->route[3] = "Model Town";
d->route_count = 4;

// Then selects: Add/Edit Timetable

add_timetable();

// User enters:
// Monday: 08:00 to 17:00
// Tuesday: 08:00 to 17:00
// Wednesday: off
// Thursday: 08:00 to 17:00
// Friday: 08:00 to 14:00

// System stores:
d->timetable[0] = {.start_time="08:00", .end_time="17:00", .is_off_day=0};
d->timetable[1] = {.start_time="08:00", .end_time="17:00", .is_off_day=0};
d->timetable[2] = {.start_time="", .end_time="", .is_off_day=1};
d->timetable[3] = {.start_time="08:00", .end_time="17:00", .is_off_day=0};
d->timetable[4] = {.start_time="08:00", .end_time="14:00", .is_off_day=0};

save_all_data();

// drivers.txt updated:
// Ahmed Khan|21K-1234|BCS-3A|Model Town, Lahore|ahmed@uni.edu|+92-300-1234567|ahmed123|ABC-123|4|4|4|0|1|University Main Gate;Mall Road;Liberty Market;Model Town
```

### Example 3: Passenger Searches and Books

```c
// Passenger logs in
// Selects: Search Rides

search_rides();

// User enters:
// Type: 0 (Fixed)
// Location: Mall Road

// System searches:
for (int i = 0; i < app.driver_count; i++) {
    if (app.drivers[i].is_active &&
        !app.drivers[i].is_flexible &&
        app.drivers[i].available_seats > 0) {

        for (int j = 0; j < app.drivers[i].route_count; j++) {
            if (strstr(app.drivers[i].route[j], "Mall Road")) {
                // FOUND: Ahmed Khan's route includes Mall Road
                // Display his info
            }
        }
    }
}

// Output:
// 1. Ahmed Khan | Ph:+92-300-1234567 | Seats:4
//    Route: University Main Gate -> Mall Road -> Liberty Market -> Model Town
//    Weekly Schedule:
//      Monday: 08:00 - 17:00
//      Tuesday: 08:00 - 17:00
//      Wednesday: OFF
//      Thursday: 08:00 - 17:00
//      Friday: 08:00 - 14:00

// User selects: 1

// System creates match:
Match *m = &app.matches[app.match_count++];
strcpy(m->driver_email, "ahmed@uni.edu");
strcpy(m->passenger_email, "sara@uni.edu");
m->is_semester_long = 1;
m->is_active = 1;

// Update seats:
app.drivers[0].available_seats--;  // 4 -> 3

save_all_data();

// matches.txt now contains:
// ahmed@uni.edu|sara@uni.edu|1|1
```

### Example 4: Flexible Ride Posting

```c
// Flexible driver logs in
// Selects: Post Flexible Ride

post_flexible_ride();

// User enters:
// Day: Monday
// Date: 25/11/2025
// Time: 08:00
// Available Seats: 3
// Direction: 0 (To Uni)
// Number of stops: 3
// Stop 1: Model Town
// Stop 2: Liberty Market
// Stop 3: University

// System creates:
FlexibleRide *r = &app.flex_rides[app.flex_ride_count];
strcpy(r->driver_email, "ali@uni.edu");
strcpy(r->day, "Monday");
strcpy(r->date, "25/11/2025");
strcpy(r->time, "08:00");
r->direction = 0;
r->stop_count = 3;
r->available_seats = 3;
r->ride_id = get_next_ride_id();  // Returns 1
r->is_active = 1;
strcpy(r->stops[0], "Model Town");
strcpy(r->stops[1], "Liberty Market");
strcpy(r->stops[2], "University");

app.flex_ride_count++;
save_all_data();

// flex_rides.txt now contains:
// ali@uni.edu|25/11/2025|Monday|08:00|0|3|3|1|1|Model Town;Liberty Market;University
```

---

---

## 🔤 String Manipulation & Input Handling

### Overview

The system heavily relies on string manipulation and careful input handling. This section explains all the techniques used.

### scanf Format Specifiers

The code uses various `scanf` format specifiers for different input types:

#### Basic Specifiers

| Specifier | Type               | Example                     | Purpose                               |
| --------- | ------------------ | --------------------------- | ------------------------------------- | ---------- | ------------------------- |
| `%s`      | String (no spaces) | `scanf("%s", email)`        | Read single word                      |
| `%d`      | Integer            | `scanf("%d", &seats)`       | Read number                           |
| `%49s`    | Limited string     | `scanf("%49s", name)`       | Read max 49 chars (prevents overflow) |
| `%[^\n]`  | String with spaces | `scanf(" %[^\n]", address)` | Read entire line until newline        |
| `%49[^    | ]`                 | Delimited string            | `sscanf(line, "%49[^                  | ]", name)` | Read until pipe character |

#### Advanced Format: `%[^\n]`

**Purpose**: Read strings that contain spaces

```c
// WITHOUT space prefix - WRONG
scanf("%[^\n]", name);  // Fails if there's leftover \n in buffer

// WITH space prefix - CORRECT
scanf(" %[^\n]", name);  // Skips leading whitespace including \n
```

**Example Usage**:

```c
printf("Name: ");
scanf(" %[^\n]", d->name);  // Can read "Ahmed Khan" (with space)

printf("Address: ");
scanf(" %[^\n]", d->address);  // Can read "123 Model Town, Lahore"
```

**How it works**:

- ` ` (space) = Skip any whitespace (spaces, tabs, newlines)
- `%[^\n]` = Read everything except newline
- Stops at newline but doesn't consume it

---

### sscanf for Parsing

**Purpose**: Parse formatted strings from files

#### Pipe-Delimited Parsing

```c
char line[] = "Ahmed Khan|21K-1234|BCS-3A|Model Town|ahmed@uni.edu";

char name[50], roll[20], section[10], address[100], email[50];

sscanf(line, "%49[^|]|%19[^|]|%9[^|]|%99[^|]|%49[^|]",
       name, roll, section, address, email);

// Result:
// name = "Ahmed Khan"
// roll = "21K-1234"
// section = "BCS-3A"
// address = "Model Town"
// email = "ahmed@uni.edu"
```

**Format Breakdown**:

- `%49[^|]` = Read up to 49 characters until `|` is found
- `|` = Literal pipe character to match
- Repeats for each field

**Complete Example from load_drivers()**:

```c
sscanf(line, "%49[^|]|%19[^|]|%9[^|]|%99[^|]|%49[^|]|%14[^|]|%29[^|]|%19[^|]|%d|%d|%d|%d|%d|%999[^|]",
       d->name, d->roll, d->section, d->address, d->email, d->phone,
       d->password, d->car_plate, &d->total_seats, &d->available_seats,
       &d->route_count, &d->is_flexible, &d->is_active, route_str);
```

**Why specific sizes?**

- `%49[^|]` for `name[50]` - Leaves room for null terminator
- `%19[^|]` for `roll[20]` - Same reason
- `%d` for integers - No size limit needed
- `%999[^|]` for route string - Large buffer for multiple stops

---

### String Functions Used

#### 1. strcpy() - String Copy

```c
char dest[50];
char src[] = "Hello";

strcpy(dest, src);  // dest = "Hello"
```

**Usage in code**:

```c
strcpy(d->route[d->route_count++], stop);  // Copy stop to route array
strcpy(m->driver_email, app.drivers[idx].email);  // Copy email
```

**Warning**: No bounds checking! Can overflow if src > dest size

---

#### 2. strncpy() - Safe String Copy

```c
char dest[50];
char src[] = "Very long string that might overflow";

strncpy(dest, src, 49);  // Copy max 49 chars
dest[49] = '\0';  // Manually add null terminator
```

**Usage in code**:

```c
int len = delim - route_ptr;
strncpy(d->route[i], route_ptr, len);
d->route[i][len] = 0;  // Add null terminator
```

**Safer than strcpy()**: Prevents buffer overflow

---

#### 3. strcmp() - String Compare

```c
int result = strcmp(str1, str2);

// Returns:
//   0  if strings are equal
//  <0  if str1 < str2
//  >0  if str1 > str2
```

**Usage in code**:

```c
if (strcmp(app.drivers[i].email, email) == 0)  // Exact match
if (strcmp(stop, "done") == 0)  // Check if user typed "done"
if (strcmp(app.drivers[idx].password, password) == 0)  // Password check
```

---

#### 4. strstr() - Substring Search

```c
char *result = strstr(haystack, needle);

// Returns:
//   Pointer to first occurrence of needle in haystack
//   NULL if not found
```

**Usage in code**:

```c
if (strstr(app.drivers[i].route[j], stop)) {
    // Route contains the search term
}
```

**Examples**:

```c
strstr("Mall Road", "Mall")    → Points to "Mall Road" (found)
strstr("Mall Road", "Road")    → Points to "Road" (found)
strstr("Mall Road", "Liberty") → NULL (not found)
strstr("Mall Road", "all")     → Points to "all Road" (found - case sensitive!)
```

**Partial Matching**:

- User searches "Mall"
- Matches "Mall Road", "Liberty Mall", "Mall Plaza"
- Very flexible for user searches

---

#### 5. strcat() - String Concatenation

```c
char dest[100] = "Hello";
char src[] = " World";

strcat(dest, src);  // dest = "Hello World"
```

**Usage in code**:

```c
char route_str[1000] = "";
for (int j = 0; j < d->route_count; j++) {
    strcat(route_str, d->route[j]);  // Add stop
    if (j < d->route_count - 1)
        strcat(route_str, ";");  // Add delimiter
}
// Result: "University;Mall Road;Liberty;Model Town"
```

**Warning**: Ensure dest has enough space!

---

#### 6. strlen() - String Length

```c
char str[] = "Hello";
int len = strlen(str);  // len = 5
```

**Not used in current code**, but useful for validation:

```c
if (strlen(d->name) == 0) {
    printf("Name cannot be empty!\n");
}
```

---

#### 7. strchr() - Find Character

```c
char *ptr = strchr(str, ch);

// Returns:
//   Pointer to first occurrence of ch in str
//   NULL if not found
```

**Usage in code**:

```c
char *delim = strchr(route_ptr, ';');  // Find semicolon
if (delim) {
    int len = delim - route_ptr;  // Calculate substring length
    // ... extract substring ...
}
```

**Parsing Semicolon-Separated Values**:

```c
char route_str[] = "University;Mall Road;Liberty";
char *route_ptr = route_str;

// First iteration
char *delim = strchr(route_ptr, ';');  // Points to first ';'
int len = delim - route_ptr;  // len = 10 ("University")
strncpy(d->route[0], route_ptr, len);  // Copy "University"
route_ptr = delim + 1;  // Move past ';'

// Second iteration
delim = strchr(route_ptr, ';');  // Points to second ';'
len = delim - route_ptr;  // len = 9 ("Mall Road")
strncpy(d->route[1], route_ptr, len);  // Copy "Mall Road"
route_ptr = delim + 1;  // Move past ';'

// Third iteration
delim = strchr(route_ptr, ';');  // NULL (no more ';')
strcpy(d->route[2], route_ptr);  // Copy remaining "Liberty"
```

---

#### 8. strcspn() - Find Character Set

```c
size_t pos = strcspn(str, charset);

// Returns:
//   Position of first character in str that matches any in charset
//   Length of str if no match
```

**Usage in code**:

```c
line[strcspn(line, "\n")] = 0;  // Remove newline from fgets()
```

**How it works**:

```c
char line[] = "Hello World\n";

size_t pos = strcspn(line, "\n");  // pos = 11 (position of \n)
line[pos] = 0;  // Replace \n with \0

// Result: line = "Hello World" (no newline)
```

**Why needed?**

- `fgets()` includes the newline character
- We want to remove it before processing
- One-liner solution!

---

### Input Buffer Management

#### The Newline Problem

```c
printf("Enter number: ");
scanf("%d", &num);  // User types "5\n"
// scanf reads 5, leaves \n in buffer

printf("Enter name: ");
scanf("%[^\n]", name);  // FAILS! Reads empty string because \n is still there
```

#### Solution 1: Leading Space

```c
scanf("%d", &num);
scanf(" %[^\n]", name);  // Space skips the leftover \n
```

#### Solution 2: Explicit getchar()

```c
scanf("%d", &num);
getchar();  // Consume the \n
scanf("%[^\n]", name);  // Now works correctly
```

#### Solution 3: Clear Entire Buffer

```c
scanf("%d", &num);
while (getchar() != '\n');  // Clear everything until newline
scanf("%[^\n]", name);
```

**Used in pause_screen()**:

```c
void pause_screen() {
    printf("\nPress Enter to continue...");
    getchar();  // Consume leftover \n from previous scanf
    getchar();  // Wait for user to press Enter
}
```

---

### Pointer Arithmetic in String Parsing

**Example from load_drivers()**:

```c
char route_str[] = "University;Mall Road;Liberty";
char *route_ptr = route_str;  // Points to start

// Find delimiter
char *delim = strchr(route_ptr, ';');  // Points to ';' after "University"

// Calculate length
int len = delim - route_ptr;  // Pointer subtraction = 10

// Extract substring
strncpy(d->route[i], route_ptr, len);
d->route[i][len] = 0;

// Move pointer forward
route_ptr = delim + 1;  // Now points to "Mall Road;Liberty"
```

**Visual Representation**:

```
Initial:
route_str:  "University;Mall Road;Liberty"
             ^
             route_ptr

After strchr:
route_str:  "University;Mall Road;Liberty"
             ^         ^
             route_ptr delim

len = delim - route_ptr = 10

After extraction:
d->route[0] = "University"

After pointer move:
route_str:  "University;Mall Road;Liberty"
                        ^
                        route_ptr (now points here)
```

---

### Common String Patterns in Code

#### Pattern 1: Read and Validate

```c
printf("Email: ");
scanf("%s", email);

// Validation (not in current code, but recommended)
if (strchr(email, '@') == NULL) {
    printf("Invalid email!\n");
    return;
}
```

#### Pattern 2: Build Delimited String

```c
char result[1000] = "";
for (int i = 0; i < count; i++) {
    strcat(result, items[i]);
    if (i < count - 1)
        strcat(result, ";");
}
// Result: "item1;item2;item3"
```

#### Pattern 3: Parse Delimited String

```c
char *ptr = input;
for (int i = 0; i < MAX_ITEMS; i++) {
    char *delim = strchr(ptr, ';');
    if (delim) {
        int len = delim - ptr;
        strncpy(items[i], ptr, len);
        items[i][len] = 0;
        ptr = delim + 1;
    } else {
        strcpy(items[i], ptr);
        break;
    }
}
```

---

### Security Considerations

#### Buffer Overflow Prevention

**Bad**:

```c
char name[50];
scanf("%s", name);  // User can type 100 chars → OVERFLOW!
```

**Good**:

```c
char name[50];
scanf("%49s", name);  // Max 49 chars + null terminator
```

#### Input Sanitization

**Current Issue**: No validation for special characters

```c
// User enters: "Name|With|Pipes"
// Breaks file format when saved!
```

**Recommended Fix**:

```c
int contains_invalid_chars(const char *str) {
    return strchr(str, '|') != NULL || strchr(str, ';') != NULL;
}

if (contains_invalid_chars(d->name)) {
    printf("Name cannot contain '|' or ';' characters!\n");
    return;
}
```

---

### String Function Comparison Table

| Function  | Purpose        | Bounds Check | Null Terminator | Use When                            |
| --------- | -------------- | ------------ | --------------- | ----------------------------------- |
| `strcpy`  | Copy string    | ❌ No        | ✅ Adds         | Source guaranteed smaller than dest |
| `strncpy` | Safe copy      | ✅ Yes       | ⚠️ Sometimes    | Need size limit                     |
| `strcmp`  | Compare        | N/A          | ✅ Needs        | Checking equality                   |
| `strstr`  | Find substring | N/A          | ✅ Needs        | Searching within string             |
| `strcat`  | Concatenate    | ❌ No        | ✅ Adds         | Building strings                    |
| `strlen`  | Get length     | N/A          | ✅ Needs        | Need string size                    |
| `strchr`  | Find character | N/A          | ✅ Needs        | Parsing delimited data              |
| `strcspn` | Find char set  | N/A          | ✅ Needs        | Removing specific chars             |

---

### 1. Soft Delete Pattern

Instead of removing data, the system uses `is_active` flags:

```c
// "Deleting" a driver (not implemented in UI, but the pattern exists)
void delete_driver(int idx) {
    app.drivers[idx].is_active = 0;  // Soft delete
    save_all_data();
}

// All search functions check is_active
int find_driver_by_email(const char *email) {
    for (int i = 0; i < app.driver_count; i++) {
        if (app.drivers[i].is_active &&  // Only active drivers
            strcmp(app.drivers[i].email, email) == 0)
            return i;
    }
    return -1;
}
```

**Benefits**:

- Data preserved for history/auditing
- Can be "undeleted" by setting `is_active = 1`
- Maintains referential integrity

### 2. Index Tracking in Search

The search function uses parallel arrays to track which driver/ride corresponds to each result:

```c
int driver_indices[100];  // Maps result number to driver index
int ride_indices[100];    // Maps result number to ride index

// When displaying result #1
driver_indices[0] = 5;  // This is driver at index 5

// When user books result #1
int selected_driver_idx = driver_indices[0];  // Get actual index
```

**Why needed?**

- Search results are numbered 1, 2, 3...
- But drivers are at arbitrary indices in array
- Mapping ensures correct driver is booked

### 3. Partial String Matching

```c
if (strstr(app.drivers[i].route[j], stop)) {
    // Match found
}
```

**Examples**:

```c
User searches: "Mall"
Matches: "Mall Road", "Liberty Mall", "Mall Plaza"

User searches: "Road"
Matches: "Mall Road", "Canal Road", "Main Road"
```

### 4. Co-Passenger Discovery

Passengers can see who else is riding with the same driver:

```c
// Find all matches with same driver
for (int j = 0; j < app.match_count; j++) {
    if (app.matches[j].is_active &&
        strcmp(app.matches[j].driver_email, current_driver) == 0 &&
        strcmp(app.matches[j].passenger_email, current_passenger) != 0) {
        // This is a co-passenger
    }
}
```

**Benefits**:

- Passengers can coordinate pickups
- Builds community
- Increases trust

---

## 🛠️ Troubleshooting & Best Practices

### Common Issues

#### Issue 1: "Segmentation Fault" on startup

**Cause**: Memory allocation failed

**Solution**:

```c
void init_dynamic_arrays() {
    app.drivers = (Driver *)malloc(app.driver_capacity * sizeof(Driver));

    // Add error checking:
    if (app.drivers == NULL) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }
}
```

#### Issue 2: Data not saving

**Cause**: File permissions or path issues

**Solution**:

```c
void save_drivers() {
    FILE *f = fopen("drivers.txt", "w");
    if (!f) {
        perror("Error opening drivers.txt");  // Shows system error
        return;
    }
    // ... save logic ...
}
```

#### Issue 3: Duplicate emails

**Current behavior**: System allows duplicate emails

**Fix**:

```c
void register_driver() {
    // ... collect email ...

    // Check for duplicates
    if (find_driver_by_email(d->email) >= 0) {
        printf("Email already registered!\n");
        return;
    }

    // ... continue registration ...
}
```

#### Issue 4: Input with pipe characters breaks file format

**Problem**: User enters "Name|With|Pipes"

**Solution**: Validate input or use different delimiter

```c
int contains_pipe(const char *str) {
    return strchr(str, '|') != NULL;
}

// In registration:
printf("Name: ");
scanf(" %[^\n]", d->name);
if (contains_pipe(d->name)) {
    printf("Name cannot contain '|' character!\n");
    return;
}
```

### Best Practices

#### 1. Always Check Return Values

```c
// BAD
FILE *f = fopen("data.txt", "r");
fgets(line, sizeof(line), f);  // Crash if f is NULL

// GOOD
FILE *f = fopen("data.txt", "r");
if (f) {
    fgets(line, sizeof(line), f);
    fclose(f);
}
```

#### 2. Use `sizeof` for Array Sizes

```c
// BAD
char name[50];
scanf("%49s", name);  // Magic number

// GOOD
char name[50];
scanf("%49s", name);  // sizeof(name)-1 = 49
```

#### 3. Clear Input Buffer

```c
// After scanf with numbers
scanf("%d", &choice);
getchar();  // Clear newline

// Or use this pattern
while (getchar() != '\n');  // Clear entire buffer
```

#### 4. Validate Array Bounds

```c
// Before accessing
if (idx >= 0 && idx < app.driver_count) {
    printf("%s\n", app.drivers[idx].name);
}
```

### Performance Considerations

#### Current Complexity

- **Search**: O(n × m) where n = drivers, m = stops per driver
- **Find by email**: O(n) linear search
- **Load/Save**: O(n) for each data type

#### Potential Optimizations

1. **Hash table for email lookup**: O(1) instead of O(n)
2. **Index routes by location**: Faster searches
3. **Binary search**: Requires sorted data

### Security Considerations

⚠️ **Current Vulnerabilities**:

1. **Plain text passwords**: Stored unencrypted
2. **No input sanitization**: SQL injection-like attacks possible
3. **No session management**: Anyone can access any account
4. **File permissions**: Anyone can read/modify data files

🔒 **Recommended Improvements**:

1. Hash passwords (use bcrypt or similar)
2. Validate all input
3. Add session tokens
4. Encrypt data files or use database

---

## 📊 Summary

### System Statistics

| Metric              | Value                        |
| ------------------- | ---------------------------- |
| Total Lines of Code | ~978                         |
| Number of Functions | 35                           |
| Data Structures     | 6                            |
| File Types          | 5                            |
| User Roles          | 3 (Admin, Driver, Passenger) |

### Key Achievements

✅ **Dynamic Memory**: No fixed user limits  
✅ **Data Persistence**: All changes saved automatically  
✅ **Flexible Architecture**: Supports two ride types  
✅ **User-Friendly**: Clear menus and prompts  
✅ **Comprehensive**: Handles registration, search, booking, and management

### Future Enhancement Ideas

1. **Edit Profile**: Allow users to update information
2. **Cancel Booking**: Remove matches
3. **Rating System**: Rate drivers and passengers
4. **Notifications**: Email/SMS for new rides
5. **Map Integration**: Visual route display
6. **Payment Integration**: Handle ride payments
7. **Mobile App**: iOS/Android interface
8. **Database**: Replace text files with SQL
9. **Multi-platform**: Linux/Mac support
10. **Analytics**: Usage statistics and reports

---

**End of Documentation**

_This documentation covers the complete implementation of the University Carpooling System. For questions or contributions, please refer to the source code at `corelogic.c`._
