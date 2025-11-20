#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STOPS 10
#define MAX_DAYS 5

typedef struct
{
    char start_time[10], end_time[10];
    int is_off_day;
} DaySchedule;

typedef struct
{
    char name[50], roll[20], section[10], address[100], email[50], phone[15], password[30], car_plate[20];
    int total_seats, available_seats, route_count, is_flexible, is_active;
    DaySchedule timetable[MAX_DAYS];
    char route[MAX_STOPS][100];
} Driver;

typedef struct
{
    char name[50], roll[20], section[10], address[100], email[50], phone[15], password[30];
    int has_timetable, is_active;
    DaySchedule timetable[MAX_DAYS];
} Passenger;

typedef struct
{
    char driver_email[50], date[20], day[20], time[10], stops[MAX_STOPS][100];
    int direction, stop_count, available_seats, ride_id, is_active;
} FlexibleRide;

typedef struct
{
    char driver_email[50], passenger_email[50];
    int is_semester_long, is_active;
} Match;

struct
{
    int driver_count, passenger_count, flex_ride_count, match_count;
    Driver *drivers;
    Passenger *passengers;
    FlexibleRide *flex_rides;
    Match *matches;
    int driver_capacity, passenger_capacity, ride_capacity, match_capacity;
    int current_driver_idx, current_passenger_idx;
} app = {0};

void init_dynamic_arrays()
{
    app.driver_capacity = 10;
    app.passenger_capacity = 10;
    app.ride_capacity = 10;
    app.match_capacity = 20;
    app.drivers = (Driver *)malloc(app.driver_capacity * sizeof(Driver));
    app.passengers = (Passenger *)malloc(app.passenger_capacity * sizeof(Passenger));
    app.flex_rides = (FlexibleRide *)malloc(app.ride_capacity * sizeof(FlexibleRide));
    app.matches = (Match *)malloc(app.match_capacity * sizeof(Match));
    memset(app.drivers, 0, app.driver_capacity * sizeof(Driver));
    memset(app.passengers, 0, app.passenger_capacity * sizeof(Passenger));
    memset(app.flex_rides, 0, app.ride_capacity * sizeof(FlexibleRide));
    memset(app.matches, 0, app.match_capacity * sizeof(Match));
}

void resize_drivers()
{
    app.driver_capacity *= 2;
    app.drivers = (Driver *)realloc(app.drivers, app.driver_capacity * sizeof(Driver));
}

void resize_passengers()
{
    app.passenger_capacity *= 2;
    app.passengers = (Passenger *)realloc(app.passengers, app.passenger_capacity * sizeof(Passenger));
}

void resize_rides()
{
    app.ride_capacity *= 2;
    app.flex_rides = (FlexibleRide *)realloc(app.flex_rides, app.ride_capacity * sizeof(FlexibleRide));
}

void resize_matches()
{
    app.match_capacity *= 2;
    app.matches = (Match *)realloc(app.matches, app.match_capacity * sizeof(Match));
}

void free_all_memory()
{
    free(app.drivers);
    free(app.passengers);
    free(app.flex_rides);
    free(app.matches);
}

void load_drivers()
{
    FILE *f = fopen("drivers.txt", "r");
    if (!f)
        return;

    app.driver_count = 0;
    char line[2000];

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;

        if (app.driver_count >= app.driver_capacity)
            resize_drivers();

        Driver *d = &app.drivers[app.driver_count];

        char route_str[1000] = "";
        sscanf(line, "%49[^|]|%19[^|]|%9[^|]|%99[^|]|%49[^|]|%14[^|]|%29[^|]|%19[^|]|%d|%d|%d|%d|%d|%999[^|]",
               d->name, d->roll, d->section, d->address, d->email, d->phone, d->password, d->car_plate,
               &d->total_seats, &d->available_seats, &d->route_count, &d->is_flexible, &d->is_active, route_str);

        char *route_ptr = route_str;
        for (int i = 0; i < d->route_count && i < MAX_STOPS; i++)
        {
            char *delim = strchr(route_ptr, ';');
            if (delim)
            {
                int len = delim - route_ptr;
                strncpy(d->route[i], route_ptr, len);
                d->route[i][len] = 0;
                route_ptr = delim + 1;
            }
            else
            {
                strcpy(d->route[i], route_ptr);
                break;
            }
        }

        app.driver_count++;
    }
    fclose(f);
}

void save_drivers()
{
    FILE *f = fopen("drivers.txt", "w");
    if (!f)
        return;

    for (int i = 0; i < app.driver_count; i++)
    {
        Driver *d = &app.drivers[i];

        char route_str[1000] = "";
        for (int j = 0; j < d->route_count; j++)
        {
            strcat(route_str, d->route[j]);
            if (j < d->route_count - 1)
                strcat(route_str, ";");
        }

        fprintf(f, "%s|%s|%s|%s|%s|%s|%s|%s|%d|%d|%d|%d|%d|%s\n",
                d->name, d->roll, d->section, d->address, d->email, d->phone, d->password, d->car_plate,
                d->total_seats, d->available_seats, d->route_count, d->is_flexible, d->is_active, route_str);
    }
    fclose(f);
}

void load_passengers()
{
    FILE *f = fopen("passengers.txt", "r");
    if (!f)
        return;

    app.passenger_count = 0;
    char line[1000];

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;

        if (app.passenger_count >= app.passenger_capacity)
            resize_passengers();

        Passenger *p = &app.passengers[app.passenger_count];

        sscanf(line, "%49[^|]|%19[^|]|%9[^|]|%99[^|]|%49[^|]|%14[^|]|%29[^|]|%d|%d",
               p->name, p->roll, p->section, p->address, p->email, p->phone, p->password,
               &p->has_timetable, &p->is_active);

        app.passenger_count++;
    }
    fclose(f);
}

void save_passengers()
{
    FILE *f = fopen("passengers.txt", "w");
    if (!f)
        return;

    for (int i = 0; i < app.passenger_count; i++)
    {
        Passenger *p = &app.passengers[i];
        fprintf(f, "%s|%s|%s|%s|%s|%s|%s|%d|%d\n",
                p->name, p->roll, p->section, p->address, p->email, p->phone, p->password,
                p->has_timetable, p->is_active);
    }
    fclose(f);
}

void load_flex_rides()
{
    FILE *f = fopen("flex_rides.txt", "r");
    if (!f)
        return;

    app.flex_ride_count = 0;
    char line[2000];

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;

        if (app.flex_ride_count >= app.ride_capacity)
            resize_rides();

        FlexibleRide *r = &app.flex_rides[app.flex_ride_count];

        char stops_str[1000] = "";
        sscanf(line, "%49[^|]|%19[^|]|%19[^|]|%9[^|]|%d|%d|%d|%d|%d|%999[^|]",
               r->driver_email, r->date, r->day, r->time, &r->direction, &r->stop_count,
               &r->available_seats, &r->ride_id, &r->is_active, stops_str);

        char *stop_ptr = stops_str;
        for (int i = 0; i < r->stop_count && i < MAX_STOPS; i++)
        {
            char *delim = strchr(stop_ptr, ';');
            if (delim)
            {
                int len = delim - stop_ptr;
                strncpy(r->stops[i], stop_ptr, len);
                r->stops[i][len] = 0;
                stop_ptr = delim + 1;
            }
            else
            {
                strcpy(r->stops[i], stop_ptr);
                break;
            }
        }

        app.flex_ride_count++;
    }
    fclose(f);
}

void save_flex_rides()
{
    FILE *f = fopen("flex_rides.txt", "w");
    if (!f)
        return;

    for (int i = 0; i < app.flex_ride_count; i++)
    {
        FlexibleRide *r = &app.flex_rides[i];

        char stops_str[1000] = "";
        for (int j = 0; j < r->stop_count; j++)
        {
            strcat(stops_str, r->stops[j]);
            if (j < r->stop_count - 1)
                strcat(stops_str, ";");
        }

        fprintf(f, "%s|%s|%s|%s|%d|%d|%d|%d|%d|%s\n",
                r->driver_email, r->date, r->day, r->time, r->direction, r->stop_count,
                r->available_seats, r->ride_id, r->is_active, stops_str);
    }
    fclose(f);
}

void load_matches()
{
    FILE *f = fopen("matches.txt", "r");
    if (!f)
        return;

    app.match_count = 0;
    char line[500];

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = 0;

        if (app.match_count >= app.match_capacity)
            resize_matches();

        Match *m = &app.matches[app.match_count];
        sscanf(line, "%49[^|]|%49[^|]|%d|%d",
               m->driver_email, m->passenger_email, &m->is_semester_long, &m->is_active);

        app.match_count++;
    }
    fclose(f);
}

void save_matches()
{
    FILE *f = fopen("matches.txt", "w");
    if (!f)
        return;

    for (int i = 0; i < app.match_count; i++)
    {
        Match *m = &app.matches[i];
        fprintf(f, "%s|%s|%d|%d\n",
                m->driver_email, m->passenger_email, m->is_semester_long, m->is_active);
    }
    fclose(f);
}

void load_all_data()
{
    load_drivers();
    load_passengers();
    load_flex_rides();
    load_matches();
}

void save_all_data()
{
    save_drivers();
    save_passengers();
    save_flex_rides();
    save_matches();
}

int find_driver_by_email(const char *email)
{
    for (int i = 0; i < app.driver_count; i++)
    {
        if (app.drivers[i].is_active && strcmp(app.drivers[i].email, email) == 0)
            return i;
    }
    return -1;
}

int find_passenger_by_email(const char *email)
{
    for (int i = 0; i < app.passenger_count; i++)
    {
        if (app.passengers[i].is_active && strcmp(app.passengers[i].email, email) == 0)
            return i;
    }
    return -1;
}

int get_next_ride_id()
{
    static int id = 1;
    return id++;
}

int check_admin_login(const char *em, const char *pw)
{
    FILE *f = fopen("admin.txt", "r");
    if (f)
    {
        char e[50], p[30];
        if (fscanf(f, "%s %s", e, p) == 2 && !strcmp(em, e) && !strcmp(pw, p))
        {
            fclose(f);
            return 1;
        }
        fclose(f);
    }
    return 0;
}

void clear_screen()
{
    system("cls");
}

void pause_screen()
{
    printf("\nPress Enter to continue...");
    getchar();
    getchar();
}

void register_driver()
{
    clear_screen();
    printf("=== DRIVER REGISTRATION ===\n\n");
    if (app.driver_count >= app.driver_capacity)
        resize_drivers();
    Driver *d = &app.drivers[app.driver_count];
    printf("Name: ");
    scanf(" %[^\n]", d->name);
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
    d->available_seats = d->total_seats;
    printf("Type (0=Fixed, 1=Flexible): ");
    scanf("%d", &d->is_flexible);
    d->is_active = 1;
    d->route_count = 0;
    app.driver_count++;
    save_all_data();
    printf("\n✓ Driver registered successfully!\n");
    pause_screen();
}

void register_passenger()
{
    clear_screen();
    printf("=== PASSENGER REGISTRATION ===\n\n");
    if (app.passenger_count >= app.passenger_capacity)
        resize_passengers();
    Passenger *p = &app.passengers[app.passenger_count];
    printf("Name: ");
    scanf(" %[^\n]", p->name);
    printf("Roll Number: ");
    scanf("%s", p->roll);
    printf("Section: ");
    scanf("%s", p->section);
    printf("Address: ");
    scanf(" %[^\n]", p->address);
    printf("Email: ");
    scanf("%s", p->email);
    printf("Phone: ");
    scanf("%s", p->phone);
    printf("Password: ");
    scanf("%s", p->password);
    p->is_active = 1;
    app.passenger_count++;
    save_all_data();
    printf("\n✓ Passenger registered successfully!\n");
    pause_screen();
}

void add_route()
{
    clear_screen();
    Driver *d = &app.drivers[app.current_driver_idx];
    printf("=== ADD/EDIT ROUTE ===\n\n");
    printf("Current Route:\n");
    for (int i = 0; i < d->route_count; i++)
        printf("%d. %s\n", i + 1, d->route[i]);
    printf("\nEnter new stops (type 'done' to finish):\n");
    while (d->route_count < MAX_STOPS)
    {
        char stop[100];
        printf("Stop %d: ", d->route_count + 1);
        scanf(" %[^\n]", stop);
        if (strcmp(stop, "done") == 0)
            break;
        strcpy(d->route[d->route_count++], stop);
    }
    save_all_data();
    printf("\n✓ Route saved!\n");
    pause_screen();
}

void add_timetable()
{
    clear_screen();
    Driver *d = &app.drivers[app.current_driver_idx];
    printf("=== ADD/EDIT TIMETABLE ===\n\n");
    printf("Enter your weekly schedule (5 days)\n");
    printf("Leave start_time as 'off' for days you're not available\n\n");

    const char *days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

    for (int i = 0; i < MAX_DAYS; i++)
    {
        printf("--- %s ---\n", days[i]);
        printf("Start Time (HH:MM or 'off'): ");
        scanf("%s", d->timetable[i].start_time);

        if (strcmp(d->timetable[i].start_time, "off") == 0)
        {
            d->timetable[i].is_off_day = 1;
            strcpy(d->timetable[i].end_time, "");
        }
        else
        {
            d->timetable[i].is_off_day = 0;
            printf("End Time (HH:MM): ");
            scanf("%s", d->timetable[i].end_time);
        }
        printf("\n");
    }

    save_all_data();
    printf("✓ Timetable saved!\n");
    pause_screen();
}

void post_flexible_ride()
{
    clear_screen();
    printf("=== POST FLEXIBLE RIDE ===\n\n");
    if (app.flex_ride_count >= app.ride_capacity)
        resize_rides();
    FlexibleRide *r = &app.flex_rides[app.flex_ride_count];
    strcpy(r->driver_email, app.drivers[app.current_driver_idx].email);

    printf("Day (e.g., Monday, Tuesday): ");
    scanf(" %[^\n]", r->day);
    printf("Date (DD/MM/YYYY): ");
    scanf("%s", r->date);
    printf("Time (HH:MM): ");
    scanf("%s", r->time);
    printf("Available Seats: ");
    scanf("%d", &r->available_seats);
    printf("Direction (0=To Uni, 1=From Uni): ");
    scanf("%d", &r->direction);
    printf("Number of stops: ");
    scanf("%d", &r->stop_count);
    for (int i = 0; i < r->stop_count && i < MAX_STOPS; i++)
    {
        printf("Stop %d: ", i + 1);
        scanf(" %[^\n]", r->stops[i]);
    }
    r->ride_id = get_next_ride_id();
    r->is_active = 1;
    app.flex_ride_count++;
    save_all_data();
    printf("\n✓ Ride posted successfully!\n");
    pause_screen();
}

void view_my_passengers()
{
    clear_screen();
    printf("=== MY PASSENGERS ===\n\n");
    int cnt = 0;
    for (int i = 0; i < app.match_count; i++)
    {
        if (app.matches[i].is_active && strcmp(app.matches[i].driver_email, app.drivers[app.current_driver_idx].email) == 0)
        {
            int pidx = find_passenger_by_email(app.matches[i].passenger_email);
            if (pidx >= 0)
            {
                printf("%d. %s | %s | %s\n", ++cnt, app.passengers[pidx].name, app.passengers[pidx].email, app.passengers[pidx].phone);
            }
        }
    }
    if (!cnt)
        printf("No passengers yet.\n");
    pause_screen();
}

void driver_dashboard()
{
    int choice;
    do
    {
        clear_screen();
        printf("=== DRIVER DASHBOARD ===\n");
        printf("Welcome, %s!\n", app.drivers[app.current_driver_idx].name);
        printf("Type: %s\n\n", app.drivers[app.current_driver_idx].is_flexible ? "Flexible" : "Fixed");

        if (!app.drivers[app.current_driver_idx].is_flexible)
        {
            printf("1. Add/Edit Route\n");
            printf("2. Add/Edit Timetable\n");
            printf("3. View My Passengers\n");
        }
        else
        {
            printf("1. Post Flexible Ride\n");
            printf("2. View My Passengers\n");
        }

        printf("0. Logout\n\n");
        printf("Choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
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

void search_rides()
{
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

    // Add arrays to track which driver/ride each result corresponds to
    int driver_indices[100]; // Track driver index for each result
    int ride_indices[100];   // Track ride index for flexible rides

    if (type == 0)
    {
        const char *days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

        for (int i = 0; i < app.driver_count; i++)
        {
            if (app.drivers[i].is_active && !app.drivers[i].is_flexible && app.drivers[i].available_seats > 0)
            {
                for (int j = 0; j < app.drivers[i].route_count; j++)
                {
                    if (strstr(app.drivers[i].route[j], stop))
                    {
                        driver_indices[found] = i; // Store the driver index

                        printf("\n%d. %s | Ph:%s | Seats:%d\n",
                               ++found,
                               app.drivers[i].name,
                               app.drivers[i].phone,
                               app.drivers[i].available_seats);

                        printf("   Route: ");
                        for (int k = 0; k < app.drivers[i].route_count; k++)
                            printf("%s%s", app.drivers[i].route[k],
                                   k < app.drivers[i].route_count - 1 ? " -> " : "\n");

                        printf("   Weekly Schedule:\n");
                        for (int d = 0; d < MAX_DAYS; d++)
                        {
                            if (app.drivers[i].timetable[d].is_off_day)
                            {
                                printf("      %s: OFF\n", days[d]);
                            }
                            else
                            {
                                printf("      %s: %s - %s\n",
                                       days[d],
                                       app.drivers[i].timetable[d].start_time,
                                       app.drivers[i].timetable[d].end_time);
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < app.flex_ride_count; i++)
        {
            if (app.flex_rides[i].is_active && app.flex_rides[i].available_seats > 0)
            {
                for (int j = 0; j < app.flex_rides[i].stop_count; j++)
                {
                    if (strstr(app.flex_rides[i].stops[j], stop))
                    {
                        int didx = find_driver_by_email(app.flex_rides[i].driver_email);
                        if (didx >= 0)
                        {
                            ride_indices[found] = i;      // Store the ride index
                            driver_indices[found] = didx; // Store the driver index

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

    if (!found)
        printf("No rides found.\n");
    else
    {
        printf("\nBook ride number (0 to cancel): ");
        int book;
        scanf("%d", &book);
        if (book > 0 && book <= found)
        {
            if (app.match_count >= app.match_capacity)
                resize_matches();
            Match *m = &app.matches[app.match_count++];
            strcpy(m->passenger_email, app.passengers[app.current_passenger_idx].email);

            // ✅ FIX: Set the driver email from the stored index
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

void view_my_rides()
{
    clear_screen();
    printf("=== MY RIDES ===\n\n");
    int cnt = 0;

    for (int i = 0; i < app.match_count; i++)
    {
        if (app.matches[i].is_active && strcmp(app.matches[i].passenger_email, app.passengers[app.current_passenger_idx].email) == 0)
        {
            int didx = find_driver_by_email(app.matches[i].driver_email);
            if (didx >= 0)
            {
                printf("\n%d. %s | Driver: %s | Email: %s | Ph: %s | Seats:%d\n",
                       ++cnt,
                       app.matches[i].is_semester_long ? "Fixed" : "Flex",
                       app.drivers[didx].name,
                       app.drivers[didx].email,
                       app.drivers[didx].phone,
                       app.drivers[didx].available_seats);

                if (app.matches[i].is_semester_long && app.drivers[didx].route_count > 0)
                {
                    printf("   Route: ");
                    for (int k = 0; k < app.drivers[didx].route_count; k++)
                        printf("%s%s", app.drivers[didx].route[k],
                               k < app.drivers[didx].route_count - 1 ? " -> " : "\n");
                }

                printf("   Co-Passengers:\n");
                int co_passenger_count = 0;

                for (int j = 0; j < app.match_count; j++)
                {
                    if (app.matches[j].is_active &&
                        strcmp(app.matches[j].driver_email, app.matches[i].driver_email) == 0 &&
                        strcmp(app.matches[j].passenger_email, app.passengers[app.current_passenger_idx].email) != 0)
                    {
                        int pidx = find_passenger_by_email(app.matches[j].passenger_email);
                        if (pidx >= 0)
                        {
                            printf("      • %s | Ph: %s | Pickup: %s\n",
                                   app.passengers[pidx].name,
                                   app.passengers[pidx].phone,
                                   app.passengers[pidx].address);
                            co_passenger_count++;
                        }
                    }
                }

                if (co_passenger_count == 0)
                {
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

void passenger_dashboard()
{
    int choice;
    do
    {
        clear_screen();
        printf("=== PASSENGER DASHBOARD ===\n");
        printf("Welcome, %s!\n\n", app.passengers[app.current_passenger_idx].name);
        printf("1. Search Rides\n");
        printf("2. View My Rides & Co-Passengers\n");
        printf("0. Logout\n\n");
        printf("Choice: ");
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            search_rides();
            break;
        case 2:
            view_my_rides();
            break;
        }
    } while (choice != 0);
}

void admin_dashboard()
{
    int choice;
    do
    {
        clear_screen();
        printf("=== ADMIN DASHBOARD ===\n\n");
        printf("1. View All Drivers\n");
        printf("2. View All Passengers\n");
        printf("3. View All Matches\n");
        printf("0. Logout\n\n");
        printf("Choice: ");
        scanf("%d", &choice);
        clear_screen();
        switch (choice)
        {
        case 1:
            printf("=== ALL DRIVERS ===\n\n");
            for (int i = 0; i < app.driver_count; i++)
            {
                if (app.drivers[i].is_active)
                {
                    printf("%d. %s | %s | %s | Ph:%s | Seats:%d/%d | Type:%s | Plate:%s\n",
                           i + 1, app.drivers[i].name, app.drivers[i].email, app.drivers[i].address,
                           app.drivers[i].phone, app.drivers[i].available_seats, app.drivers[i].total_seats,
                           app.drivers[i].is_flexible ? "Flex" : "Fixed", app.drivers[i].car_plate);
                }
            }
            pause_screen();
            break;
        case 2:
            printf("=== ALL PASSENGERS ===\n\n");
            for (int i = 0; i < app.passenger_count; i++)
            {
                if (app.passengers[i].is_active)
                {
                    printf("%d. %s | %s | %s | Ph:%s | Addr:%s\n",
                           i + 1, app.passengers[i].name, app.passengers[i].email,
                           app.passengers[i].roll, app.passengers[i].phone, app.passengers[i].address);
                }
            }
            pause_screen();
            break;
        case 3:
            printf("=== ALL MATCHES ===\n\n");
            for (int i = 0; i < app.match_count; i++)
            {
                if (app.matches[i].is_active)
                    printf("%d. %s <-> %s\n", i + 1, app.matches[i].driver_email, app.matches[i].passenger_email);
            }
            pause_screen();
            break;
        }
    } while (choice != 0);
}

void login()
{
    clear_screen();
    printf("=== LOGIN ===\n\n");
    char email[50], password[30];
    printf("Email: ");
    scanf("%s", email);
    printf("Password: ");
    scanf("%s", password);
    if (check_admin_login(email, password))
    {
        admin_dashboard();
        return;
    }
    int idx = find_driver_by_email(email);
    if (idx >= 0 && strcmp(app.drivers[idx].password, password) == 0)
    {
        app.current_driver_idx = idx;
        driver_dashboard();
        return;
    }
    idx = find_passenger_by_email(email);
    if (idx >= 0 && strcmp(app.passengers[idx].password, password) == 0)
    {
        app.current_passenger_idx = idx;
        passenger_dashboard();
        return;
    }
    printf("\n✗ Invalid credentials!\n");
    pause_screen();
}

int main()
{
    init_dynamic_arrays();
    load_all_data();
    FILE *f = fopen("admin.txt", "r");
    if (!f)
    {
        f = fopen("admin.txt", "w");
        fprintf(f, "admin@carpool.com admin123");
        fclose(f);
    }
    else
        fclose(f);
    int choice;
    do
    {
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
        switch (choice)
        {
        case 1:
            register_driver();
            break;
        case 2:
            register_passenger();
            break;
        case 3:
            login();
            break;
        case 0:
            save_all_data();
            free_all_memory();
            printf("\nGoodbye!\n");
            break;
        default:
            printf("\n✗ Invalid choice!\n");
            pause_screen();
        }
    } while (choice != 0);
    return 0;
}