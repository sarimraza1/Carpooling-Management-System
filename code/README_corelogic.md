# University Carpooling System - Core Logic

## Purpose

This is a **University Carpooling System** that helps students share rides to and from university. Drivers can offer rides, and passengers can search and book available rides. The system manages two types of rides:
- **Fixed Rides**: Regular semester-long rides with a weekly schedule
- **Flexible Rides**: One-time rides for specific dates

## What It Achieves

- Drivers can register and create profiles with their car and route details
- Passengers can register and search for rides that match their needs
- Admin can view all drivers, passengers, and ride bookings
- Automatic matching between drivers and passengers
- Data persistence (saves all information to files)
- Secure login with email and password

## Input/Output Expectations

### Input
- User registration details (name, email, password, phone, address)
- Route information (list of stops)
- Schedule information (weekly timetable for fixed rides)
- Ride details (date, time, direction for flexible rides)
- Search criteria (location/stop name)

### Output
- List of available rides matching search criteria
- Driver and passenger information
- Route details with stops
- Weekly schedules
- Co-passenger information
- Booking confirmations

---

## Data Structures

### 1. DaySchedule
Stores schedule information for one day of the week.

```
Structure:
- start_time: When the ride starts (e.g., "08:00")
- end_time: When the ride ends (e.g., "09:00")
- is_off_day: 1 if not available, 0 if available
```

### 2. Driver
Stores all information about a driver.

```
Structure:
- name: Driver's full name
- roll: Student roll number
- section: Class section
- address: Home address
- email: Email (used for login)
- phone: Contact number
- password: Login password
- car_plate: Car registration number
- total_seats: Total seats in car
- available_seats: Remaining available seats
- route_count: Number of stops in route
- is_flexible: 0 for fixed rides, 1 for flexible rides
- is_active: 1 if account is active, 0 if deleted
- timetable: Array of 5 DaySchedule (Monday to Friday)
- route: Array of up to 10 stops
```

### 3. Passenger
Stores all information about a passenger.

```
Structure:
- name: Passenger's full name
- roll: Student roll number
- section: Class section
- address: Home address
- email: Email (used for login)
- phone: Contact number
- password: Login password
- has_timetable: Whether passenger has schedule (not used)
- is_active: 1 if account is active, 0 if deleted
- timetable: Array of 5 DaySchedule (not actively used)
```

### 4. FlexibleRide
Stores information about one-time flexible rides.

```
Structure:
- driver_email: Email of driver posting the ride
- date: Date of ride (DD/MM/YYYY)
- day: Day name (Monday, Tuesday, etc.)
- time: Time of ride (HH:MM)
- direction: 0 for "To Uni", 1 for "From Uni"
- stop_count: Number of stops
- available_seats: Available seats for this ride
- ride_id: Unique identifier for the ride
- is_active: 1 if ride is active, 0 if cancelled
- stops: Array of up to 10 stop locations
```

### 5. Match
Stores the connection between a driver and passenger.

```
Structure:
- driver_email: Email of the driver
- passenger_email: Email of the passenger
- is_semester_long: 1 for fixed rides, 0 for flexible rides
- is_active: 1 if match is active, 0 if cancelled
```

### 6. Global App Structure
Stores all application data in memory.

```
Structure:
- driver_count: Number of drivers
- passenger_count: Number of passengers
- flex_ride_count: Number of flexible rides
- match_count: Number of matches
- drivers: Pointer to dynamic array of drivers
- passengers: Pointer to dynamic array of passengers
- flex_rides: Pointer to dynamic array of flexible rides
- matches: Pointer to dynamic array of matches
- driver_capacity: Current capacity of driver array
- passenger_capacity: Current capacity of passenger array
- ride_capacity: Current capacity of ride array
- match_capacity: Current capacity of match array
- current_driver_idx: Index of currently logged-in driver
- current_passenger_idx: Index of currently logged-in passenger
```

---

## Functions

### Memory Management Functions

**1. init_dynamic_arrays()**
- Initializes all dynamic arrays
- Allocates initial memory for 10 drivers, 10 passengers, 10 rides, 20 matches
- Uses `malloc()` to allocate memory
- Uses `memset()` to clear all memory to zero

**2. resize_drivers()**
- Doubles the capacity of the driver array when full
- Uses `realloc()` to expand memory

**3. resize_passengers()**
- Doubles the capacity of the passenger array when full
- Uses `realloc()` to expand memory

**4. resize_rides()**
- Doubles the capacity of the flexible rides array when full
- Uses `realloc()` to expand memory

**5. resize_matches()**
- Doubles the capacity of the matches array when full
- Uses `realloc()` to expand memory

**6. free_all_memory()**
- Frees all allocated memory before program exits
- Uses `free()` on all dynamic arrays

### File Handling Functions

**7. load_drivers()**
- Reads driver data from "drivers.txt"
- Parses pipe-separated (|) values
- Parses semicolon-separated (;) route stops
- Returns silently if file doesn't exist

**8. save_drivers()**
- Writes all driver data to "drivers.txt"
- Formats data with pipe separators (|)
- Formats route stops with semicolon separators (;)

**9. load_passengers()**
- Reads passenger data from "passengers.txt"
- Parses pipe-separated (|) values

**10. save_passengers()**
- Writes all passenger data to "passengers.txt"
- Formats data with pipe separators (|)

**11. load_flex_rides()**
- Reads flexible ride data from "flex_rides.txt"
- Parses pipe-separated (|) values
- Parses semicolon-separated (;) stops

**12. save_flex_rides()**
- Writes flexible ride data to "flex_rides.txt"
- Formats data with pipe separators (|)
- Formats stops with semicolon separators (;)

**13. load_matches()**
- Reads match data from "matches.txt"
- Parses pipe-separated (|) values

**14. save_matches()**
- Writes match data to "matches.txt"
- Formats data with pipe separators (|)

**15. load_all_data()**
- Calls all load functions to load entire database

**16. save_all_data()**
- Calls all save functions to save entire database

### Search and Helper Functions

**17. find_driver_by_email(email)**
- Searches for active driver with given email
- Returns index if found, -1 if not found

**18. find_passenger_by_email(email)**
- Searches for active passenger with given email
- Returns index if found, -1 if not found

**19. get_next_ride_id()**
- Generates unique ID for flexible rides
- Uses static variable that increments

**20. check_admin_login(email, password)**
- Verifies admin credentials from "admin.txt"
- Returns 1 if correct, 0 if wrong

### UI Helper Functions

**21. clear_screen()**
- Clears the console screen
- Uses `system("cls")` for Windows

**22. pause_screen()**
- Waits for user to press Enter
- Uses two `getchar()` calls to consume newline

### Registration Functions

**23. register_driver()**
- Collects driver information from user
- Creates new driver account
- Initializes available_seats to total_seats
- Saves data to file
- Resizes array if needed

**24. register_passenger()**
- Collects passenger information from user
- Creates new passenger account
- Saves data to file
- Resizes array if needed

### Driver Functions

**25. add_route()**
- Allows driver to add/edit their route
- Accepts up to 10 stops
- Type "done" to finish
- Shows current route first

**26. add_timetable()**
- Allows driver to set weekly schedule
- Asks for start and end times for each day (Monday-Friday)
- Type "off" for days not available

**27. post_flexible_ride()**
- Allows flexible drivers to post one-time rides
- Collects day, date, time, seats, direction, and stops
- Generates unique ride ID
- Saves to file

**28. view_my_passengers()**
- Shows all passengers matched with current driver
- Displays name, email, and phone

**29. driver_dashboard()**
- Main menu for drivers after login
- Different options for fixed vs flexible drivers
- Fixed drivers: Add route, timetable, view passengers
- Flexible drivers: Post ride, view passengers

### Passenger Functions

**30. search_rides()**
- Searches for rides based on location and type
- For fixed rides: Shows route and weekly schedule
- For flexible rides: Shows date, time, and direction
- Allows booking by selecting ride number
- Creates match and decrements available seats

**31. view_my_rides()**
- Shows all rides booked by current passenger
- Displays driver information
- Shows co-passengers in the same ride
- Shows route for fixed rides

**32. passenger_dashboard()**
- Main menu for passengers after login
- Options: Search rides, view my rides

### Admin Functions

**33. admin_dashboard()**
- Admin main menu
- View all drivers with details
- View all passengers with details
- View all matches (driver-passenger pairs)

### Login and Main Functions

**34. login()**
- Handles login for admin, driver, or passenger
- Checks credentials against stored data
- Redirects to appropriate dashboard

**35. main()**
- Program entry point
- Initializes memory and loads data
- Creates default admin account if not exists
- Shows main menu (Register Driver, Register Passenger, Login, Exit)
- Saves data and frees memory on exit

---

## File Handling Details

### Files Used

1. **drivers.txt**
   - Stores all driver information
   - Format: `name|roll|section|address|email|phone|password|car_plate|total_seats|available_seats|route_count|is_flexible|is_active|route_stops`
   - Route stops separated by semicolons (;)

2. **passengers.txt**
   - Stores all passenger information
   - Format: `name|roll|section|address|email|phone|password|has_timetable|is_active`

3. **flex_rides.txt**
   - Stores all flexible ride postings
   - Format: `driver_email|date|day|time|direction|stop_count|available_seats|ride_id|is_active|stops`
   - Stops separated by semicolons (;)

4. **matches.txt**
   - Stores all driver-passenger matches
   - Format: `driver_email|passenger_email|is_semester_long|is_active`

5. **admin.txt**
   - Stores admin credentials
   - Format: `email password`
   - Default: `admin@carpool.com admin123`

### File Operations

- **Reading**: Uses `fopen()` with "r" mode, `fgets()` for lines, `sscanf()` for parsing
- **Writing**: Uses `fopen()` with "w" mode, `fprintf()` for formatted output
- **Error Handling**: Checks if file exists before reading, returns silently if not
- **Data Persistence**: All changes immediately saved to files

---

## Dynamic Memory Usage

### Initial Allocation
```
Drivers array: 10 slots × sizeof(Driver)
Passengers array: 10 slots × sizeof(Passenger)
Flexible rides array: 10 slots × sizeof(FlexibleRide)
Matches array: 20 slots × sizeof(Match)
```

### Automatic Expansion
- When any array reaches capacity, it doubles in size
- Example: 10 → 20 → 40 → 80 → 160...
- Uses `realloc()` to expand without losing existing data

### Memory Cleanup
- All memory freed before program exits
- Uses `free()` for each dynamic array
- Prevents memory leaks

### Advantages
- No fixed limits on users or rides
- Memory efficient (starts small, grows as needed)
- Prevents array overflow errors

---

## Program Flow

### Flow Diagram

```
                        START
                          |
                          v
              +----------------------+
              | Initialize Memory    |
              | Load All Data        |
              | Create Admin Account |
              +----------------------+
                          |
                          v
              +----------------------+
              |    MAIN MENU         |
              | 1. Register Driver   |
              | 2. Register Passenger|
              | 3. Login             |
              | 0. Exit              |
              +----------------------+
                    |     |     |
        +-----------+     |     +------------+
        |                 |                  |
        v                 v                  v
  +------------+    +----------+      +-------------+
  | Register   |    | Register |      | Login       |
  | Driver     |    | Passenger|      | (Email/Pass)|
  +------------+    +----------+      +-------------+
        |                 |             |    |    |
        |                 |             |    |    |
        v                 v             v    v    v
  +------------+    +----------+    +-----+  |  +-------+
  | Save Data  |    | Save Data|    |Admin|  |  |Passenger|
  +------------+    +----------+    |Dash |  |  |Dash    |
        |                 |         +-----+  |  +-------+
        |                 |            |     |     |
        +--------+--------+            |     |     |
                 |                     |     |     |
                 v                     |     |     |
          Back to Main                 |     v     |
                                       |  +------+ |
                                       |  |Driver| |
                                       |  |Dash  | |
                                       |  +------+ |
                                       |     |     |
              +------------------------+     |     |
              |                              |     |
              v                              v     v
      +---------------+              +-------------------+
      | View Drivers  |              | Driver Functions: |
      | View Passengers|             | - Add Route       |
      | View Matches  |              | - Add Timetable   |
      +---------------+              | - Post Flex Ride  |
              |                      | - View Passengers |
              |                      +-------------------+
              |                              |
              |                              |
              |                      +-------------------+
              |                      |Passenger Functions|
              |                      | - Search Rides    |
              |                      | - View My Rides   |
              |                      | - See Co-Passengers|
              |                      +-------------------+
              |                              |
              +------------+-----------------+
                           |
                           v
                    +-------------+
                    | Save Data   |
                    | Free Memory |
                    +-------------+
                           |
                           v
                         EXIT
```

### Detailed Execution Flow

#### 1. Program Initialization
1. Call `init_dynamic_arrays()` - allocate memory
2. Call `load_all_data()` - load from files
3. Check if admin.txt exists, create if not
4. Display main menu

#### 2. Registration Flow (Driver)
1. User selects "Register as Driver"
2. Check if array is full, resize if needed
3. Collect all driver information
4. Set default values (available_seats = total_seats, is_active = 1)
5. Increment driver_count
6. Call `save_all_data()` to persist
7. Return to main menu

#### 3. Registration Flow (Passenger)
1. User selects "Register as Passenger"
2. Check if array is full, resize if needed
3. Collect all passenger information
4. Set default values (is_active = 1)
5. Increment passenger_count
6. Call `save_all_data()` to persist
7. Return to main menu

#### 4. Login Flow
1. User enters email and password
2. Check if admin (call `check_admin_login()`)
   - If yes, go to admin dashboard
3. Search in drivers array (call `find_driver_by_email()`)
   - If found and password matches, go to driver dashboard
4. Search in passengers array (call `find_passenger_by_email()`)
   - If found and password matches, go to passenger dashboard
5. If none match, show error

#### 5. Driver Dashboard Flow (Fixed Driver)
1. Display menu with options
2. **Add/Edit Route**:
   - Show current route
   - Accept new stops (max 10)
   - Save to file
3. **Add/Edit Timetable**:
   - Loop through 5 days
   - Accept start/end times or "off"
   - Save to file
4. **View My Passengers**:
   - Search matches array for current driver
   - Find passenger details
   - Display list

#### 6. Driver Dashboard Flow (Flexible Driver)
1. Display menu with options
2. **Post Flexible Ride**:
   - Collect ride details (day, date, time, seats, direction)
   - Collect stops
   - Generate unique ride ID
   - Save to file
3. **View My Passengers**: Same as fixed driver

#### 7. Passenger Dashboard Flow
1. Display menu with options
2. **Search Rides**:
   - Choose ride type (fixed or flexible)
   - Enter location to search
   - System searches all rides for matching stops
   - Display matching rides with details
   - User selects ride to book
   - Create match record
   - Save to file
3. **View My Rides**:
   - Search matches for current passenger
   - Display ride details for each match
   - Show co-passengers in same rides

#### 8. Admin Dashboard Flow
1. Display menu with options
2. **View All Drivers**: Loop through drivers, display active ones
3. **View All Passengers**: Loop through passengers, display active ones
4. **View All Matches**: Loop through matches, display active connections

#### 9. Exit Flow
1. User selects exit from main menu
2. Call `save_all_data()` to ensure all changes saved
3. Call `free_all_memory()` to clean up
4. Program terminates

---

## Key Features

### 1. Email-Based Authentication
- Each driver and passenger has unique email
- Password-protected accounts
- Admin has separate credentials

### 2. Dynamic Route Management
- Drivers can add up to 10 stops
- Easy to modify routes
- Supports partial matching for searches

### 3. Flexible and Fixed Rides
- Fixed: Semester-long with weekly schedule
- Flexible: One-time rides for specific dates
- Different interfaces for each type

### 4. Smart Matching
- Links passengers to drivers
- Tracks semester-long and one-time bookings
- Shows co-passengers to encourage carpooling

### 5. Seat Management
- Tracks available vs total seats
- Prevents overbooking
- Updates automatically when booking

### 6. Admin Oversight
- View all system data
- Monitor driver-passenger matches
- System-wide visibility

---

## Limitations and Considerations

1. **No password encryption** - Passwords stored in plain text
2. **Windows-specific** - Uses `cls` for clear screen (Windows only)
3. **No delete functionality** - Cannot remove accounts or rides (uses is_active flag)
4. **Basic search** - Only searches by location name, no advanced filters
5. **No edit after creation** - Cannot modify match after booking
6. **File format** - Pipe-delimited files could be corrupted by user input containing pipes
7. **No input validation** - Trusts user to enter correct format (dates, times)

---

## Compilation and Execution

### Compile
```bash
gcc corelogic.c -o corelogic
```

### Run
```bash
./corelogic
```

### First Time Setup
- Admin account created automatically
- Login: admin@carpool.com / admin123
- Register at least one driver and passenger to test functionality

---

## Data Flow Example

### Example: Passenger Books a Ride

1. **Passenger logs in** → System finds passenger in array
2. **Selects "Search Rides"** → Passenger dashboard
3. **Enters location "Mall Road"** → search_rides() function
4. **System searches**:
   - Loops through all drivers (fixed) OR flex_rides (flexible)
   - Checks each stop in route
   - Uses `strstr()` to find partial matches
   - Filters by available_seats > 0
5. **Displays matching rides** with full details
6. **Passenger selects ride #2**
7. **System creates match**:
   - Adds new Match record
   - Sets driver_email and passenger_email
   - Sets is_semester_long based on ride type
8. **System updates**:
   - Decrements available_seats for driver (fixed) or ride (flexible)
9. **Saves to files**:
   - matches.txt gets new entry
   - drivers.txt or flex_rides.txt updated with new seat count
10. **Confirmation displayed**

This demonstrates the complete data flow from user input through search, matching, and persistence.

