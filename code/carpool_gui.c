#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STOPS 10
#define MAX_DAYS 5

// ============ DATA STRUCTURES ============
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
    GtkWidget *main_window;
    GtkWidget *content_box;
} app = {0};

// ============ DATA MANAGEMENT (from corelogic.c) ============
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
               p->name, p->roll, p->section, p->address, p->email, p->phone, p->password, &p->has_timetable, &p->is_active);
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
        fprintf(f, "%s|%s|%s|%s|%s|%s|%s|%d|%d\n", p->name, p->roll, p->section, p->address, p->email, p->phone, p->password, p->has_timetable, p->is_active);
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
               r->driver_email, r->date, r->day, r->time, &r->direction, &r->stop_count, &r->available_seats, &r->ride_id, &r->is_active, stops_str);
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
                r->driver_email, r->date, r->day, r->time, r->direction, r->stop_count, r->available_seats, r->ride_id, r->is_active, stops_str);
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
        sscanf(line, "%49[^|]|%49[^|]|%d|%d", m->driver_email, m->passenger_email, &m->is_semester_long, &m->is_active);
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
        fprintf(f, "%s|%s|%d|%d\n", m->driver_email, m->passenger_email, m->is_semester_long, m->is_active);
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

// ============ GUI HELPERS ============
void show_message(const char *title, const char *message, GtkMessageType type)
{
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(app.main_window), GTK_DIALOG_MODAL, type, GTK_BUTTONS_OK, "%s", message);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void clear_content()
{
    GList *children = gtk_container_get_children(GTK_CONTAINER(app.content_box));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);
}

// Forward declarations
void show_login_screen();
void show_driver_dashboard();
void show_passenger_dashboard();
void show_admin_dashboard();

// ============ DRIVER REGISTRATION ============
typedef struct
{
    GtkWidget *name, *roll, *section, *address, *email, *phone, *password, *car_plate, *total_seats, *is_flexible;
} DriverRegData;

void on_driver_register_submit(GtkWidget *widget, gpointer data)
{
    DriverRegData *reg = (DriverRegData *)data;

    if (app.driver_count >= app.driver_capacity)
        resize_drivers();
    Driver *d = &app.drivers[app.driver_count];

    strcpy(d->name, gtk_entry_get_text(GTK_ENTRY(reg->name)));
    strcpy(d->roll, gtk_entry_get_text(GTK_ENTRY(reg->roll)));
    strcpy(d->section, gtk_entry_get_text(GTK_ENTRY(reg->section)));
    strcpy(d->address, gtk_entry_get_text(GTK_ENTRY(reg->address)));
    strcpy(d->email, gtk_entry_get_text(GTK_ENTRY(reg->email)));
    strcpy(d->phone, gtk_entry_get_text(GTK_ENTRY(reg->phone)));
    strcpy(d->password, gtk_entry_get_text(GTK_ENTRY(reg->password)));
    strcpy(d->car_plate, gtk_entry_get_text(GTK_ENTRY(reg->car_plate)));
    d->total_seats = atoi(gtk_entry_get_text(GTK_ENTRY(reg->total_seats)));
    d->available_seats = d->total_seats;
    d->is_flexible = gtk_combo_box_get_active(GTK_COMBO_BOX(reg->is_flexible));
    d->is_active = 1;
    d->route_count = 0;

    app.driver_count++;
    save_all_data();

    show_message("Success", "Driver registered successfully!", GTK_MESSAGE_INFO);
    show_login_screen();
    free(reg);
}

void show_driver_registration()
{
    clear_content();

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(app.content_box), scroll, TRUE, TRUE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 15);
    gtk_widget_set_margin_top(grid, 20);
    gtk_widget_set_margin_bottom(grid, 20);
    gtk_widget_set_margin_start(grid, 20);
    gtk_widget_set_margin_end(grid, 20);
    gtk_container_add(GTK_CONTAINER(scroll), grid);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large' weight='bold'>Driver Registration</span>");
    gtk_grid_attach(GTK_GRID(grid), title, 0, 0, 2, 1);

    DriverRegData *reg = g_new(DriverRegData, 1);

    const char *labels[] = {"Name:", "Roll Number:", "Section:", "Address:", "Email:", "Phone:", "Password:", "Car Plate:", "Total Seats:", "Type:"};
    GtkWidget **entries[] = {&reg->name, &reg->roll, &reg->section, &reg->address, &reg->email, &reg->phone, &reg->password, &reg->car_plate, &reg->total_seats, &reg->is_flexible};

    for (int i = 0; i < 10; i++)
    {
        GtkWidget *label = gtk_label_new(labels[i]);
        gtk_widget_set_halign(label, GTK_ALIGN_END);
        gtk_grid_attach(GTK_GRID(grid), label, 0, i + 1, 1, 1);

        if (i == 9)
        {
            *entries[i] = gtk_combo_box_text_new();
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(*entries[i]), "Fixed Route");
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(*entries[i]), "Flexible Rides");
            gtk_combo_box_set_active(GTK_COMBO_BOX(*entries[i]), 0);
        }
        else
        {
            *entries[i] = gtk_entry_new();
            gtk_entry_set_width_chars(GTK_ENTRY(*entries[i]), 30);
            if (i == 6)
                gtk_entry_set_visibility(GTK_ENTRY(*entries[i]), FALSE);
        }
        gtk_grid_attach(GTK_GRID(grid), *entries[i], 1, i + 1, 1, 1);
    }

    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(btn_box, GTK_ALIGN_CENTER);

    GtkWidget *submit_btn = gtk_button_new_with_label("Register");
    gtk_widget_set_size_request(submit_btn, 120, 35);
    g_signal_connect(submit_btn, "clicked", G_CALLBACK(on_driver_register_submit), reg);
    gtk_box_pack_start(GTK_BOX(btn_box), submit_btn, FALSE, FALSE, 0);

    GtkWidget *back_btn = gtk_button_new_with_label("Back");
    gtk_widget_set_size_request(back_btn, 120, 35);
    g_signal_connect(back_btn, "clicked", G_CALLBACK(show_login_screen), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), back_btn, FALSE, FALSE, 0);

    gtk_grid_attach(GTK_GRID(grid), btn_box, 0, 11, 2, 1);

    gtk_widget_show_all(app.content_box);
}

// ============ PASSENGER REGISTRATION ============
typedef struct
{
    GtkWidget *name, *roll, *section, *address, *email, *phone, *password;
} PassengerRegData;

void on_passenger_register_submit(GtkWidget *widget, gpointer data)
{
    PassengerRegData *reg = (PassengerRegData *)data;

    if (app.passenger_count >= app.passenger_capacity)
        resize_passengers();
    Passenger *p = &app.passengers[app.passenger_count];

    strcpy(p->name, gtk_entry_get_text(GTK_ENTRY(reg->name)));
    strcpy(p->roll, gtk_entry_get_text(GTK_ENTRY(reg->roll)));
    strcpy(p->section, gtk_entry_get_text(GTK_ENTRY(reg->section)));
    strcpy(p->address, gtk_entry_get_text(GTK_ENTRY(reg->address)));
    strcpy(p->email, gtk_entry_get_text(GTK_ENTRY(reg->email)));
    strcpy(p->phone, gtk_entry_get_text(GTK_ENTRY(reg->phone)));
    strcpy(p->password, gtk_entry_get_text(GTK_ENTRY(reg->password)));
    p->is_active = 1;

    app.passenger_count++;
    save_all_data();

    show_message("Success", "Passenger registered successfully!", GTK_MESSAGE_INFO);
    show_login_screen();
    free(reg);
}

void show_passenger_registration()
{
    clear_content();

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(app.content_box), scroll, TRUE, TRUE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 15);
    gtk_widget_set_margin_top(grid, 20);
    gtk_widget_set_margin_bottom(grid, 20);
    gtk_widget_set_margin_start(grid, 20);
    gtk_widget_set_margin_end(grid, 20);
    gtk_container_add(GTK_CONTAINER(scroll), grid);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='x-large' weight='bold'>Passenger Registration</span>");
    gtk_grid_attach(GTK_GRID(grid), title, 0, 0, 2, 1);

    PassengerRegData *reg = g_new(PassengerRegData, 1);

    const char *labels[] = {"Name:", "Roll Number:", "Section:", "Address:", "Email:", "Phone:", "Password:"};
    GtkWidget **entries[] = {&reg->name, &reg->roll, &reg->section, &reg->address, &reg->email, &reg->phone, &reg->password};

    for (int i = 0; i < 7; i++)
    {
        GtkWidget *label = gtk_label_new(labels[i]);
        gtk_widget_set_halign(label, GTK_ALIGN_END);
        gtk_grid_attach(GTK_GRID(grid), label, 0, i + 1, 1, 1);

        *entries[i] = gtk_entry_new();
        gtk_entry_set_width_chars(GTK_ENTRY(*entries[i]), 30);
        if (i == 6)
            gtk_entry_set_visibility(GTK_ENTRY(*entries[i]), FALSE);
        gtk_grid_attach(GTK_GRID(grid), *entries[i], 1, i + 1, 1, 1);
    }

    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(btn_box, GTK_ALIGN_CENTER);

    GtkWidget *submit_btn = gtk_button_new_with_label("Register");
    gtk_widget_set_size_request(submit_btn, 120, 35);
    g_signal_connect(submit_btn, "clicked", G_CALLBACK(on_passenger_register_submit), reg);
    gtk_box_pack_start(GTK_BOX(btn_box), submit_btn, FALSE, FALSE, 0);

    GtkWidget *back_btn = gtk_button_new_with_label("Back");
    gtk_widget_set_size_request(back_btn, 120, 35);
    g_signal_connect(back_btn, "clicked", G_CALLBACK(show_login_screen), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), back_btn, FALSE, FALSE, 0);

    gtk_grid_attach(GTK_GRID(grid), btn_box, 0, 8, 2, 1);

    gtk_widget_show_all(app.content_box);
}

// ============ LOGIN SCREEN ============
typedef struct
{
    GtkWidget *email, *password;
} LoginData;

void on_login_submit(GtkWidget *widget, gpointer data)
{
    LoginData *login = (LoginData *)data;
    const char *email = gtk_entry_get_text(GTK_ENTRY(login->email));
    const char *password = gtk_entry_get_text(GTK_ENTRY(login->password));

    if (check_admin_login(email, password))
    {
        show_admin_dashboard();
        return;
    }

    int idx = find_driver_by_email(email);
    if (idx >= 0 && strcmp(app.drivers[idx].password, password) == 0)
    {
        app.current_driver_idx = idx;
        show_driver_dashboard();
        return;
    }

    idx = find_passenger_by_email(email);
    if (idx >= 0 && strcmp(app.passengers[idx].password, password) == 0)
    {
        app.current_passenger_idx = idx;
        show_passenger_dashboard();
        return;
    }

    show_message("Error", "Invalid credentials!", GTK_MESSAGE_ERROR);
}

void show_login_screen()
{
    clear_content();

    GtkWidget *center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_valign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(center_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(app.content_box), center_box, TRUE, TRUE, 0);

    GtkWidget *card = gtk_box_new(GTK_ORIENTATION_VERTICAL, 20);
    gtk_widget_set_margin_top(card, 40);
    gtk_widget_set_margin_bottom(card, 40);
    gtk_widget_set_margin_start(card, 40);
    gtk_widget_set_margin_end(card, 40);
    gtk_box_pack_start(GTK_BOX(center_box), card, FALSE, FALSE, 0);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='xx-large' weight='bold'>🚗 University Carpooling</span>");
    gtk_box_pack_start(GTK_BOX(card), title, FALSE, FALSE, 0);

    GtkWidget *subtitle = gtk_label_new("Login to your account");
    gtk_box_pack_start(GTK_BOX(card), subtitle, FALSE, FALSE, 0);

    LoginData *login = g_new(LoginData, 1);

    GtkWidget *form = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(form), 12);
    gtk_grid_set_column_spacing(GTK_GRID(form), 10);
    gtk_box_pack_start(GTK_BOX(card), form, FALSE, FALSE, 0);

    GtkWidget *email_label = gtk_label_new("Email:");
    gtk_widget_set_halign(email_label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(form), email_label, 0, 0, 1, 1);
    login->email = gtk_entry_new();
    gtk_entry_set_width_chars(GTK_ENTRY(login->email), 25);
    gtk_grid_attach(GTK_GRID(form), login->email, 1, 0, 1, 1);

    GtkWidget *password_label = gtk_label_new("Password:");
    gtk_widget_set_halign(password_label, GTK_ALIGN_END);
    gtk_grid_attach(GTK_GRID(form), password_label, 0, 1, 1, 1);
    login->password = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(login->password), FALSE);
    gtk_entry_set_width_chars(GTK_ENTRY(login->password), 25);
    gtk_grid_attach(GTK_GRID(form), login->password, 1, 1, 1, 1);

    GtkWidget *login_btn = gtk_button_new_with_label("Login");
    gtk_widget_set_size_request(login_btn, 200, 40);
    g_signal_connect(login_btn, "clicked", G_CALLBACK(on_login_submit), login);
    gtk_box_pack_start(GTK_BOX(card), login_btn, FALSE, FALSE, 0);

    GtkWidget *sep = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(card), sep, FALSE, FALSE, 0);

    GtkWidget *reg_label = gtk_label_new("Don't have an account?");
    gtk_box_pack_start(GTK_BOX(card), reg_label, FALSE, FALSE, 0);

    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(btn_box, GTK_ALIGN_CENTER);

    GtkWidget *driver_reg_btn = gtk_button_new_with_label("Register as Driver");
    g_signal_connect(driver_reg_btn, "clicked", G_CALLBACK(show_driver_registration), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), driver_reg_btn, FALSE, FALSE, 0);

    GtkWidget *passenger_reg_btn = gtk_button_new_with_label("Register as Passenger");
    g_signal_connect(passenger_reg_btn, "clicked", G_CALLBACK(show_passenger_registration), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), passenger_reg_btn, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(card), btn_box, FALSE, FALSE, 0);

    gtk_widget_show_all(app.content_box);
}

// ============ DRIVER - ADD ROUTE ============
typedef struct
{
    GtkWidget *entries[MAX_STOPS];
    int count;
} RouteData;

void on_add_route_submit(GtkWidget *widget, gpointer data)
{
    RouteData *rd = (RouteData *)data;
    Driver *d = &app.drivers[app.current_driver_idx];
    d->route_count = 0;

    // Clear all existing routes first
    for (int i = 0; i < MAX_STOPS; i++)
    {
        d->route[i][0] = '\0';
    }

    // Add new routes
    for (int i = 0; i < rd->count; i++)
    {
        const char *stop = gtk_entry_get_text(GTK_ENTRY(rd->entries[i]));
        if (strlen(stop) > 0 && d->route_count < MAX_STOPS)
            strcpy(d->route[d->route_count++], stop);
    }

    if (d->route_count == 0)
    {
        show_message("Error", "Please add at least one stop!", GTK_MESSAGE_ERROR);
        free(rd);
        return;
    }

    save_all_data();
    show_message("Success", "Route saved successfully!", GTK_MESSAGE_INFO);
    show_driver_dashboard();
    free(rd);
}

void show_add_route()
{
    clear_content();
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(app.content_box), scroll, TRUE, TRUE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_margin_top(grid, 20);
    gtk_widget_set_margin_start(grid, 20);
    gtk_container_add(GTK_CONTAINER(scroll), grid);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>Add/Edit Route</span>");
    gtk_grid_attach(GTK_GRID(grid), title, 0, 0, 2, 1);

    RouteData *rd = g_new(RouteData, 1);
    Driver *d = &app.drivers[app.current_driver_idx];
    rd->count = MAX_STOPS;

    for (int i = 0; i < rd->count; i++)
    {
        char label[20];
        sprintf(label, "Stop %d:", i + 1);
        GtkWidget *lbl = gtk_label_new(label);
        gtk_widget_set_halign(lbl, GTK_ALIGN_END);
        gtk_grid_attach(GTK_GRID(grid), lbl, 0, i + 1, 1, 1);
        rd->entries[i] = gtk_entry_new();
        gtk_entry_set_width_chars(GTK_ENTRY(rd->entries[i]), 40);
        gtk_entry_set_placeholder_text(GTK_ENTRY(rd->entries[i]), "Leave empty if not needed");
        if (i < d->route_count)
            gtk_entry_set_text(GTK_ENTRY(rd->entries[i]), d->route[i]);
        gtk_grid_attach(GTK_GRID(grid), rd->entries[i], 1, i + 1, 1, 1);
    }

    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *save_btn = gtk_button_new_with_label("Save Route");
    gtk_widget_set_size_request(save_btn, 120, 35);
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_add_route_submit), rd);
    gtk_box_pack_start(GTK_BOX(btn_box), save_btn, FALSE, FALSE, 0);

    GtkWidget *back_btn = gtk_button_new_with_label("Back");
    gtk_widget_set_size_request(back_btn, 120, 35);
    g_signal_connect(back_btn, "clicked", G_CALLBACK(show_driver_dashboard), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), back_btn, FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(grid), btn_box, 0, rd->count + 1, 2, 1);

    gtk_widget_show_all(app.content_box);
}

// ============ DRIVER - ADD TIMETABLE ============
typedef struct
{
    GtkWidget *start[MAX_DAYS], *end[MAX_DAYS], *off[MAX_DAYS];
} TimetableData;

void on_timetable_submit(GtkWidget *widget, gpointer data)
{
    TimetableData *td = (TimetableData *)data;
    Driver *d = &app.drivers[app.current_driver_idx];
    for (int i = 0; i < MAX_DAYS; i++)
    {
        d->timetable[i].is_off_day = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(td->off[i]));
        if (!d->timetable[i].is_off_day)
        {
            strcpy(d->timetable[i].start_time, gtk_entry_get_text(GTK_ENTRY(td->start[i])));
            strcpy(d->timetable[i].end_time, gtk_entry_get_text(GTK_ENTRY(td->end[i])));
        }
    }
    save_all_data();
    show_message("Success", "Timetable saved!", GTK_MESSAGE_INFO);
    show_driver_dashboard();
    free(td);
}

void show_add_timetable()
{
    clear_content();
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(app.content_box), scroll, TRUE, TRUE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 12);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_margin_top(grid, 20);
    gtk_widget_set_margin_start(grid, 20);
    gtk_container_add(GTK_CONTAINER(scroll), grid);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>Weekly Timetable</span>");
    gtk_grid_attach(GTK_GRID(grid), title, 0, 0, 4, 1);

    const char *days[] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    TimetableData *td = g_new(TimetableData, 1);
    Driver *d = &app.drivers[app.current_driver_idx];

    for (int i = 0; i < MAX_DAYS; i++)
    {
        GtkWidget *day_lbl = gtk_label_new(days[i]);
        gtk_widget_set_halign(day_lbl, GTK_ALIGN_END);
        gtk_grid_attach(GTK_GRID(grid), day_lbl, 0, i + 1, 1, 1);

        td->start[i] = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(td->start[i]), "09:00");
        gtk_entry_set_width_chars(GTK_ENTRY(td->start[i]), 8);
        if (!d->timetable[i].is_off_day && strlen(d->timetable[i].start_time) > 0)
            gtk_entry_set_text(GTK_ENTRY(td->start[i]), d->timetable[i].start_time);
        gtk_grid_attach(GTK_GRID(grid), td->start[i], 1, i + 1, 1, 1);

        td->end[i] = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(td->end[i]), "17:00");
        gtk_entry_set_width_chars(GTK_ENTRY(td->end[i]), 8);
        if (!d->timetable[i].is_off_day && strlen(d->timetable[i].end_time) > 0)
            gtk_entry_set_text(GTK_ENTRY(td->end[i]), d->timetable[i].end_time);
        gtk_grid_attach(GTK_GRID(grid), td->end[i], 2, i + 1, 1, 1);

        td->off[i] = gtk_check_button_new_with_label("Off Day");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(td->off[i]), d->timetable[i].is_off_day);
        gtk_grid_attach(GTK_GRID(grid), td->off[i], 3, i + 1, 1, 1);
    }

    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *save_btn = gtk_button_new_with_label("Save Timetable");
    gtk_widget_set_size_request(save_btn, 140, 35);
    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_timetable_submit), td);
    gtk_box_pack_start(GTK_BOX(btn_box), save_btn, FALSE, FALSE, 0);

    GtkWidget *back_btn = gtk_button_new_with_label("Back");
    gtk_widget_set_size_request(back_btn, 120, 35);
    g_signal_connect(back_btn, "clicked", G_CALLBACK(show_driver_dashboard), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), back_btn, FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(grid), btn_box, 0, MAX_DAYS + 1, 4, 1);

    gtk_widget_show_all(app.content_box);
}

// ============ DRIVER - POST FLEXIBLE RIDE ============
typedef struct
{
    GtkWidget *day, *date, *time, *seats, *direction, *stops[MAX_STOPS];
    int stop_count;
} FlexRideData;

void on_flex_ride_submit(GtkWidget *widget, gpointer data)
{
    FlexRideData *frd = (FlexRideData *)data;
    if (app.flex_ride_count >= app.ride_capacity)
        resize_rides();
    FlexibleRide *r = &app.flex_rides[app.flex_ride_count];
    strcpy(r->driver_email, app.drivers[app.current_driver_idx].email);
    strcpy(r->day, gtk_entry_get_text(GTK_ENTRY(frd->day)));
    strcpy(r->date, gtk_entry_get_text(GTK_ENTRY(frd->date)));
    strcpy(r->time, gtk_entry_get_text(GTK_ENTRY(frd->time)));
    r->available_seats = atoi(gtk_entry_get_text(GTK_ENTRY(frd->seats)));
    r->direction = gtk_combo_box_get_active(GTK_COMBO_BOX(frd->direction));
    r->stop_count = 0;
    for (int i = 0; i < frd->stop_count; i++)
    {
        const char *stop = gtk_entry_get_text(GTK_ENTRY(frd->stops[i]));
        if (strlen(stop) > 0)
            strcpy(r->stops[r->stop_count++], stop);
    }
    static int next_id = 1000;
    r->ride_id = next_id++;
    r->is_active = 1;
    app.flex_ride_count++;
    save_all_data();
    show_message("Success", "Flexible ride posted!", GTK_MESSAGE_INFO);
    show_driver_dashboard();
    free(frd);
}

void show_post_flex_ride()
{
    clear_content();
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(app.content_box), scroll, TRUE, TRUE, 0);

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_widget_set_margin_top(grid, 20);
    gtk_widget_set_margin_start(grid, 20);
    gtk_container_add(GTK_CONTAINER(scroll), grid);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>Post Flexible Ride</span>");
    gtk_grid_attach(GTK_GRID(grid), title, 0, 0, 2, 1);

    FlexRideData *frd = g_new(FlexRideData, 1);
    int row = 1;

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Day:"), 0, row, 1, 1);
    frd->day = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(frd->day), "Monday");
    gtk_grid_attach(GTK_GRID(grid), frd->day, 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Date:"), 0, row, 1, 1);
    frd->date = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(frd->date), "15/11/2025");
    gtk_grid_attach(GTK_GRID(grid), frd->date, 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Time:"), 0, row, 1, 1);
    frd->time = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(frd->time), "09:00");
    gtk_grid_attach(GTK_GRID(grid), frd->time, 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Seats:"), 0, row, 1, 1);
    frd->seats = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(frd->seats), "3");
    gtk_grid_attach(GTK_GRID(grid), frd->seats, 1, row++, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Direction:"), 0, row, 1, 1);
    frd->direction = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(frd->direction), "To University");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(frd->direction), "From University");
    gtk_combo_box_set_active(GTK_COMBO_BOX(frd->direction), 0);
    gtk_grid_attach(GTK_GRID(grid), frd->direction, 1, row++, 1, 1);

    frd->stop_count = 4;
    for (int i = 0; i < frd->stop_count; i++)
    {
        char lbl[20];
        sprintf(lbl, "Stop %d:", i + 1);
        gtk_grid_attach(GTK_GRID(grid), gtk_label_new(lbl), 0, row, 1, 1);
        frd->stops[i] = gtk_entry_new();
        gtk_entry_set_width_chars(GTK_ENTRY(frd->stops[i]), 30);
        gtk_grid_attach(GTK_GRID(grid), frd->stops[i], 1, row++, 1, 1);
    }

    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    GtkWidget *post_btn = gtk_button_new_with_label("Post Ride");
    gtk_widget_set_size_request(post_btn, 120, 35);
    g_signal_connect(post_btn, "clicked", G_CALLBACK(on_flex_ride_submit), frd);
    gtk_box_pack_start(GTK_BOX(btn_box), post_btn, FALSE, FALSE, 0);

    GtkWidget *back_btn = gtk_button_new_with_label("Back");
    gtk_widget_set_size_request(back_btn, 120, 35);
    g_signal_connect(back_btn, "clicked", G_CALLBACK(show_driver_dashboard), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), back_btn, FALSE, FALSE, 0);
    gtk_grid_attach(GTK_GRID(grid), btn_box, 0, row, 2, 1);

    gtk_widget_show_all(app.content_box);
}

// ============ DRIVER - VIEW PASSENGERS ============
void show_my_passengers()
{
    clear_content();
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(app.content_box), scroll, TRUE, TRUE, 0);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_top(vbox, 20);
    gtk_widget_set_margin_start(vbox, 20);
    gtk_container_add(GTK_CONTAINER(scroll), vbox);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>My Passengers</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);

    int cnt = 0;
    for (int i = 0; i < app.match_count; i++)
    {
        if (app.matches[i].is_active && strcmp(app.matches[i].driver_email, app.drivers[app.current_driver_idx].email) == 0)
        {
            int pidx = find_passenger_by_email(app.matches[i].passenger_email);
            if (pidx >= 0)
            {
                char info[300];
                sprintf(info, "<b>%s</b>\nEmail: %s | Phone: %s\nAddress: %s",
                        app.passengers[pidx].name, app.passengers[pidx].email,
                        app.passengers[pidx].phone, app.passengers[pidx].address);
                GtkWidget *lbl = gtk_label_new(NULL);
                gtk_label_set_markup(GTK_LABEL(lbl), info);
                gtk_label_set_xalign(GTK_LABEL(lbl), 0);
                gtk_box_pack_start(GTK_BOX(vbox), lbl, FALSE, FALSE, 5);
                gtk_box_pack_start(GTK_BOX(vbox), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);
                cnt++;
            }
        }
    }
    if (!cnt)
        gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("No passengers yet."), FALSE, FALSE, 0);

    GtkWidget *back_btn = gtk_button_new_with_label("Back");
    gtk_widget_set_size_request(back_btn, 120, 35);
    g_signal_connect(back_btn, "clicked", G_CALLBACK(show_driver_dashboard), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), back_btn, FALSE, FALSE, 10);

    gtk_widget_show_all(app.content_box);
}

// ============ DRIVER DASHBOARD ============
void show_driver_dashboard()
{
    clear_content();
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(app.content_box), vbox, TRUE, TRUE, 0);

    char welcome[150];
    sprintf(welcome, "<span size='x-large' weight='bold'>Driver Dashboard</span>\n<span size='large'>Welcome, %s!</span>\n<span>Type: %s</span>",
            app.drivers[app.current_driver_idx].name,
            app.drivers[app.current_driver_idx].is_flexible ? "Flexible" : "Fixed Route");
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), welcome);
    gtk_label_set_justify(GTK_LABEL(header), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, FALSE, 10);

    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

    if (!app.drivers[app.current_driver_idx].is_flexible)
    {
        GtkWidget *route_btn = gtk_button_new_with_label("Add/Edit Route");
        gtk_widget_set_size_request(route_btn, 250, 40);
        g_signal_connect(route_btn, "clicked", G_CALLBACK(show_add_route), NULL);
        gtk_box_pack_start(GTK_BOX(btn_box), route_btn, FALSE, FALSE, 0);

        GtkWidget *timetable_btn = gtk_button_new_with_label("Add/Edit Timetable");
        gtk_widget_set_size_request(timetable_btn, 250, 40);
        g_signal_connect(timetable_btn, "clicked", G_CALLBACK(show_add_timetable), NULL);
        gtk_box_pack_start(GTK_BOX(btn_box), timetable_btn, FALSE, FALSE, 0);
    }
    else
    {
        GtkWidget *flex_btn = gtk_button_new_with_label("Post Flexible Ride");
        gtk_widget_set_size_request(flex_btn, 250, 40);
        g_signal_connect(flex_btn, "clicked", G_CALLBACK(show_post_flex_ride), NULL);
        gtk_box_pack_start(GTK_BOX(btn_box), flex_btn, FALSE, FALSE, 0);
    }

    GtkWidget *passengers_btn = gtk_button_new_with_label("View My Passengers");
    gtk_widget_set_size_request(passengers_btn, 250, 40);
    g_signal_connect(passengers_btn, "clicked", G_CALLBACK(show_my_passengers), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), passengers_btn, FALSE, FALSE, 0);

    GtkWidget *logout_btn = gtk_button_new_with_label("Logout");
    gtk_widget_set_size_request(logout_btn, 250, 40);
    g_signal_connect(logout_btn, "clicked", G_CALLBACK(show_login_screen), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), logout_btn, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), btn_box, FALSE, FALSE, 0);
    gtk_widget_show_all(app.content_box);
}

// ============ PASSENGER - SEARCH RIDES ============
typedef struct
{
    int type;
    char selected_driver[50];
    int selected_ride_idx;
    GtkWidget *type_combo;
    GtkWidget *location_entry;
    GtkWidget *results_box;
    GtkWidget *scroll;
} SearchData;

void on_book_ride(GtkWidget *widget, gpointer data)
{
    SearchData *sd = (SearchData *)data;
    if (strlen(sd->selected_driver) == 0)
    {
        show_message("Error", "Please select a ride first!", GTK_MESSAGE_WARNING);
        return;
    }
    if (app.match_count >= app.match_capacity)
        resize_matches();
    Match *m = &app.matches[app.match_count++];
    strcpy(m->passenger_email, app.passengers[app.current_passenger_idx].email);
    strcpy(m->driver_email, sd->selected_driver);
    m->is_semester_long = !sd->type;
    m->is_active = 1;

    int didx = find_driver_by_email(sd->selected_driver);
    if (didx >= 0)
        app.drivers[didx].available_seats--;

    save_all_data();
    show_message("Success", "Ride booked successfully!", GTK_MESSAGE_INFO);
    show_passenger_dashboard();
}

void on_ride_selected(GtkWidget *widget, gpointer data)
{
    SearchData *sd = (SearchData *)data;
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
    {
        const char *driver_email = (const char *)g_object_get_data(G_OBJECT(widget), "driver_email");
        strcpy(sd->selected_driver, driver_email);
        sd->selected_ride_idx = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "ride_idx"));
    }
}

void do_search_callback(GtkWidget *widget, gpointer data)
{
    SearchData *sd = (SearchData *)data;

    // Clear previous results
    GList *children = gtk_container_get_children(GTK_CONTAINER(sd->results_box));
    for (GList *iter = children; iter != NULL; iter = g_list_next(iter))
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    g_list_free(children);

    int type = gtk_combo_box_get_active(GTK_COMBO_BOX(sd->type_combo));
    const char *stop = gtk_entry_get_text(GTK_ENTRY(sd->location_entry));
    sd->type = type;
    sd->selected_driver[0] = '\0';

    GSList *group = NULL;
    int found = 0;

    if (type == 0)
    {
        const char *days[] = {"Mon", "Tue", "Wed", "Thu", "Fri"};
        for (int i = 0; i < app.driver_count; i++)
        {
            if (app.drivers[i].is_active && !app.drivers[i].is_flexible && app.drivers[i].available_seats > 0 && app.drivers[i].route_count > 0)
            {
                for (int j = 0; j < app.drivers[i].route_count; j++)
                {
                    if (strlen(stop) == 0 || strstr(app.drivers[i].route[j], stop))
                    {
                        char info[1500];
                        int len = snprintf(info, sizeof(info), "<b>%s</b> | Ph: %s | Seats: %d\n<small>Route: ",
                                           app.drivers[i].name, app.drivers[i].phone, app.drivers[i].available_seats);

                        for (int k = 0; k < app.drivers[i].route_count && len < sizeof(info) - 100; k++)
                        {
                            len += snprintf(info + len, sizeof(info) - len, "%s", app.drivers[i].route[k]);
                            if (k < app.drivers[i].route_count - 1 && len < sizeof(info) - 20)
                                len += snprintf(info + len, sizeof(info) - len, " → ");
                        }

                        if (len < sizeof(info) - 200)
                            len += snprintf(info + len, sizeof(info) - len, "\nSchedule: ");

                        for (int d = 0; d < MAX_DAYS && len < sizeof(info) - 100; d++)
                        {
                            if (!app.drivers[i].timetable[d].is_off_day)
                            {
                                len += snprintf(info + len, sizeof(info) - len, "%s(%s-%s) ",
                                                days[d], app.drivers[i].timetable[d].start_time, app.drivers[i].timetable[d].end_time);
                            }
                        }

                        if (len < sizeof(info) - 20)
                            snprintf(info + len, sizeof(info) - len, "</small>");

                        GtkWidget *radio = gtk_radio_button_new_with_label(group, "");
                        group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
                        g_object_set_data_full(G_OBJECT(radio), "driver_email", g_strdup(app.drivers[i].email), g_free);
                        g_object_set_data(G_OBJECT(radio), "ride_idx", GINT_TO_POINTER(-1));
                        g_signal_connect(radio, "toggled", G_CALLBACK(on_ride_selected), sd);

                        GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
                        gtk_box_pack_start(GTK_BOX(hbox), radio, FALSE, FALSE, 0);
                        GtkWidget *lbl = gtk_label_new(NULL);
                        gtk_label_set_markup(GTK_LABEL(lbl), info);
                        gtk_label_set_xalign(GTK_LABEL(lbl), 0);
                        gtk_box_pack_start(GTK_BOX(hbox), lbl, TRUE, TRUE, 0);
                        gtk_box_pack_start(GTK_BOX(sd->results_box), hbox, FALSE, FALSE, 5);
                        found++;
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
                    if (strlen(stop) == 0 || strstr(app.flex_rides[i].stops[j], stop))
                    {
                        int didx = find_driver_by_email(app.flex_rides[i].driver_email);
                        if (didx >= 0)
                        {
                            char info[1200];
                            int len = snprintf(info, sizeof(info), "<b>%s</b> | Ph: %s | <b>%s</b> @ %s | Seats: %d | %s\n<small>Stops: ",
                                               app.drivers[didx].name, app.drivers[didx].phone, app.flex_rides[i].day,
                                               app.flex_rides[i].time, app.flex_rides[i].available_seats,
                                               app.flex_rides[i].direction ? "From Uni" : "To Uni");

                            for (int k = 0; k < app.flex_rides[i].stop_count && len < sizeof(info) - 50; k++)
                            {
                                len += snprintf(info + len, sizeof(info) - len, "%s", app.flex_rides[i].stops[k]);
                                if (k < app.flex_rides[i].stop_count - 1 && len < sizeof(info) - 20)
                                    len += snprintf(info + len, sizeof(info) - len, " → ");
                            }

                            if (len < sizeof(info) - 20)
                                snprintf(info + len, sizeof(info) - len, "</small>");

                            GtkWidget *radio = gtk_radio_button_new_with_label(group, "");
                            group = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio));
                            g_object_set_data_full(G_OBJECT(radio), "driver_email", g_strdup(app.flex_rides[i].driver_email), g_free);
                            g_object_set_data(G_OBJECT(radio), "ride_idx", GINT_TO_POINTER(i));
                            g_signal_connect(radio, "toggled", G_CALLBACK(on_ride_selected), sd);

                            GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
                            gtk_box_pack_start(GTK_BOX(hbox), radio, FALSE, FALSE, 0);
                            GtkWidget *lbl = gtk_label_new(NULL);
                            gtk_label_set_markup(GTK_LABEL(lbl), info);
                            gtk_label_set_xalign(GTK_LABEL(lbl), 0);
                            gtk_box_pack_start(GTK_BOX(hbox), lbl, TRUE, TRUE, 0);
                            gtk_box_pack_start(GTK_BOX(sd->results_box), hbox, FALSE, FALSE, 5);
                            found++;
                            break;
                        }
                    }
                }
            }
        }
    }

    if (!found)
        gtk_box_pack_start(GTK_BOX(sd->results_box), gtk_label_new("No rides found."), FALSE, FALSE, 10);
    gtk_widget_show_all(sd->scroll);
}

void show_search_rides()
{
    clear_content();
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_top(vbox, 20);
    gtk_widget_set_margin_start(vbox, 20);
    gtk_widget_set_margin_end(vbox, 20);
    gtk_box_pack_start(GTK_BOX(app.content_box), vbox, TRUE, TRUE, 0);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>Search Rides</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);

    GtkWidget *form = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(form), gtk_label_new("Type:"), FALSE, FALSE, 0);
    GtkWidget *type_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(type_combo), "Fixed Route");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(type_combo), "Flexible Rides");
    gtk_combo_box_set_active(GTK_COMBO_BOX(type_combo), 0);
    gtk_box_pack_start(GTK_BOX(form), type_combo, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(form), gtk_label_new("Location:"), FALSE, FALSE, 5);
    GtkWidget *location_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(location_entry), "Enter stop/location");
    gtk_box_pack_start(GTK_BOX(form), location_entry, TRUE, TRUE, 0);

    GtkWidget *search_btn = gtk_button_new_with_label("Search");
    gtk_box_pack_start(GTK_BOX(form), search_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), form, FALSE, FALSE, 0);

    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scroll, -1, 300);
    gtk_box_pack_start(GTK_BOX(vbox), scroll, TRUE, TRUE, 0);

    GtkWidget *results_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_add(GTK_CONTAINER(scroll), results_box);

    SearchData *sd = g_new0(SearchData, 1);
    sd->type_combo = type_combo;
    sd->location_entry = location_entry;
    sd->results_box = results_box;
    sd->scroll = scroll;

    g_signal_connect(search_btn, "clicked", G_CALLBACK(do_search_callback), sd);

    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(btn_box, GTK_ALIGN_CENTER);
    GtkWidget *book_btn = gtk_button_new_with_label("Book Selected Ride");
    gtk_widget_set_size_request(book_btn, 180, 35);
    g_signal_connect(book_btn, "clicked", G_CALLBACK(on_book_ride), sd);
    gtk_box_pack_start(GTK_BOX(btn_box), book_btn, FALSE, FALSE, 0);

    GtkWidget *back_btn = gtk_button_new_with_label("Back");
    gtk_widget_set_size_request(back_btn, 120, 35);
    g_signal_connect(back_btn, "clicked", G_CALLBACK(show_passenger_dashboard), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), back_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), btn_box, FALSE, FALSE, 0);

    gtk_widget_show_all(app.content_box);
}

// ============ PASSENGER - VIEW MY RIDES ============
void show_my_rides()
{
    clear_content();
    GtkWidget *scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(app.content_box), scroll, TRUE, TRUE, 0);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_widget_set_margin_top(vbox, 20);
    gtk_widget_set_margin_start(vbox, 20);
    gtk_container_add(GTK_CONTAINER(scroll), vbox);

    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<span size='large' weight='bold'>My Rides &amp; Co-Passengers</span>");
    gtk_box_pack_start(GTK_BOX(vbox), title, FALSE, FALSE, 0);

    int cnt = 0;
    for (int i = 0; i < app.match_count; i++)
    {
        if (app.matches[i].is_active && strcmp(app.matches[i].passenger_email, app.passengers[app.current_passenger_idx].email) == 0)
        {
            int didx = find_driver_by_email(app.matches[i].driver_email);
            if (didx >= 0)
            {
                char info[600];
                sprintf(info, "<b>Ride %d - %s</b>\nDriver: %s | Phone: %s | Email: %s",
                        ++cnt, app.matches[i].is_semester_long ? "Fixed Route" : "Flexible",
                        app.drivers[didx].name, app.drivers[didx].phone, app.drivers[didx].email);

                if (app.matches[i].is_semester_long && app.drivers[didx].route_count > 0)
                {
                    strcat(info, "\n<small>Route: ");
                    for (int k = 0; k < app.drivers[didx].route_count; k++)
                    {
                        strcat(info, app.drivers[didx].route[k]);
                        if (k < app.drivers[didx].route_count - 1)
                            strcat(info, " → ");
                    }
                    strcat(info, "</small>");
                }

                GtkWidget *lbl = gtk_label_new(NULL);
                gtk_label_set_markup(GTK_LABEL(lbl), info);
                gtk_label_set_xalign(GTK_LABEL(lbl), 0);
                gtk_box_pack_start(GTK_BOX(vbox), lbl, FALSE, FALSE, 0);

                GtkWidget *co_label = gtk_label_new("<b>Co-Passengers:</b>");
                gtk_label_set_use_markup(GTK_LABEL(co_label), TRUE);
                gtk_label_set_xalign(GTK_LABEL(co_label), 0);
                gtk_widget_set_margin_start(co_label, 20);
                gtk_box_pack_start(GTK_BOX(vbox), co_label, FALSE, FALSE, 0);

                int co_cnt = 0;
                for (int j = 0; j < app.match_count; j++)
                {
                    if (app.matches[j].is_active && strcmp(app.matches[j].driver_email, app.matches[i].driver_email) == 0 &&
                        strcmp(app.matches[j].passenger_email, app.passengers[app.current_passenger_idx].email) != 0)
                    {
                        int pidx = find_passenger_by_email(app.matches[j].passenger_email);
                        if (pidx >= 0)
                        {
                            char co_info[250];
                            sprintf(co_info, "<small>• %s | Ph: %s | Pickup: %s</small>",
                                    app.passengers[pidx].name, app.passengers[pidx].phone, app.passengers[pidx].address);
                            GtkWidget *co_lbl = gtk_label_new(NULL);
                            gtk_label_set_markup(GTK_LABEL(co_lbl), co_info);
                            gtk_label_set_xalign(GTK_LABEL(co_lbl), 0);
                            gtk_widget_set_margin_start(co_lbl, 40);
                            gtk_box_pack_start(GTK_BOX(vbox), co_lbl, FALSE, FALSE, 0);
                            co_cnt++;
                        }
                    }
                }
                if (!co_cnt)
                {
                    GtkWidget *none = gtk_label_new("<small>No other passengers yet</small>");
                    gtk_label_set_use_markup(GTK_LABEL(none), TRUE);
                    gtk_label_set_xalign(GTK_LABEL(none), 0);
                    gtk_widget_set_margin_start(none, 40);
                    gtk_box_pack_start(GTK_BOX(vbox), none, FALSE, FALSE, 0);
                }
                gtk_box_pack_start(GTK_BOX(vbox), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);
            }
        }
    }

    if (!cnt)
        gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("No rides yet."), FALSE, FALSE, 0);

    GtkWidget *back_btn = gtk_button_new_with_label("Back");
    gtk_widget_set_size_request(back_btn, 120, 35);
    g_signal_connect(back_btn, "clicked", G_CALLBACK(show_passenger_dashboard), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), back_btn, FALSE, FALSE, 10);

    gtk_widget_show_all(app.content_box);
}

// ============ PASSENGER DASHBOARD ============
void show_passenger_dashboard()
{
    clear_content();
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_valign(vbox, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(vbox, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(app.content_box), vbox, TRUE, TRUE, 0);

    char welcome[150];
    sprintf(welcome, "<span size='x-large' weight='bold'>Passenger Dashboard</span>\n<span size='large'>Welcome, %s!</span>",
            app.passengers[app.current_passenger_idx].name);
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), welcome);
    gtk_label_set_justify(GTK_LABEL(header), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(GTK_BOX(vbox), header, FALSE, FALSE, 10);

    GtkWidget *btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);

    GtkWidget *search_btn = gtk_button_new_with_label("Search Rides");
    gtk_widget_set_size_request(search_btn, 250, 40);
    g_signal_connect(search_btn, "clicked", G_CALLBACK(show_search_rides), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), search_btn, FALSE, FALSE, 0);

    GtkWidget *rides_btn = gtk_button_new_with_label("View My Rides & Co-Passengers");
    gtk_widget_set_size_request(rides_btn, 250, 40);
    g_signal_connect(rides_btn, "clicked", G_CALLBACK(show_my_rides), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), rides_btn, FALSE, FALSE, 0);

    GtkWidget *logout_btn = gtk_button_new_with_label("Logout");
    gtk_widget_set_size_request(logout_btn, 250, 40);
    g_signal_connect(logout_btn, "clicked", G_CALLBACK(show_login_screen), NULL);
    gtk_box_pack_start(GTK_BOX(btn_box), logout_btn, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), btn_box, FALSE, FALSE, 0);
    gtk_widget_show_all(app.content_box);
}

// ============ ADMIN DASHBOARD ============
void show_admin_dashboard()
{
    clear_content();
    GtkWidget *notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(app.content_box), notebook, TRUE, TRUE, 0);

    // Drivers Tab
    GtkWidget *drivers_scroll = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *drivers_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_top(drivers_box, 15);
    gtk_widget_set_margin_start(drivers_box, 15);
    gtk_container_add(GTK_CONTAINER(drivers_scroll), drivers_box);

    for (int i = 0; i < app.driver_count; i++)
    {
        if (app.drivers[i].is_active)
        {
            char info[400];
            sprintf(info, "<b>%s</b> | %s | Roll: %s | Ph: %s\nSeats: %d/%d | Type: %s | Plate: %s",
                    app.drivers[i].name, app.drivers[i].email, app.drivers[i].roll, app.drivers[i].phone,
                    app.drivers[i].available_seats, app.drivers[i].total_seats,
                    app.drivers[i].is_flexible ? "Flexible" : "Fixed", app.drivers[i].car_plate);
            GtkWidget *lbl = gtk_label_new(NULL);
            gtk_label_set_markup(GTK_LABEL(lbl), info);
            gtk_label_set_xalign(GTK_LABEL(lbl), 0);
            gtk_box_pack_start(GTK_BOX(drivers_box), lbl, FALSE, FALSE, 5);
            gtk_box_pack_start(GTK_BOX(drivers_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);
        }
    }
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), drivers_scroll, gtk_label_new("Drivers"));

    // Passengers Tab
    GtkWidget *passengers_scroll = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *passengers_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_top(passengers_box, 15);
    gtk_widget_set_margin_start(passengers_box, 15);
    gtk_container_add(GTK_CONTAINER(passengers_scroll), passengers_box);

    for (int i = 0; i < app.passenger_count; i++)
    {
        if (app.passengers[i].is_active)
        {
            char info[300];
            sprintf(info, "<b>%s</b> | %s | Roll: %s | Ph: %s\nAddress: %s",
                    app.passengers[i].name, app.passengers[i].email, app.passengers[i].roll,
                    app.passengers[i].phone, app.passengers[i].address);
            GtkWidget *lbl = gtk_label_new(NULL);
            gtk_label_set_markup(GTK_LABEL(lbl), info);
            gtk_label_set_xalign(GTK_LABEL(lbl), 0);
            gtk_box_pack_start(GTK_BOX(passengers_box), lbl, FALSE, FALSE, 5);
            gtk_box_pack_start(GTK_BOX(passengers_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);
        }
    }
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), passengers_scroll, gtk_label_new("Passengers"));

    // Matches Tab
    GtkWidget *matches_scroll = gtk_scrolled_window_new(NULL, NULL);
    GtkWidget *matches_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_margin_top(matches_box, 15);
    gtk_widget_set_margin_start(matches_box, 15);
    gtk_container_add(GTK_CONTAINER(matches_scroll), matches_box);

    for (int i = 0; i < app.match_count; i++)
    {
        if (app.matches[i].is_active)
        {
            char info[200];
            sprintf(info, "<b>Match %d</b> | Driver: %s ↔ Passenger: %s\nType: %s",
                    i + 1, app.matches[i].driver_email, app.matches[i].passenger_email,
                    app.matches[i].is_semester_long ? "Semester Long" : "Flexible");
            GtkWidget *lbl = gtk_label_new(NULL);
            gtk_label_set_markup(GTK_LABEL(lbl), info);
            gtk_label_set_xalign(GTK_LABEL(lbl), 0);
            gtk_box_pack_start(GTK_BOX(matches_box), lbl, FALSE, FALSE, 5);
            gtk_box_pack_start(GTK_BOX(matches_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 0);
        }
    }
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), matches_scroll, gtk_label_new("Matches"));

    GtkWidget *logout_btn = gtk_button_new_with_label("Logout");
    gtk_widget_set_size_request(logout_btn, 120, 35);
    g_signal_connect(logout_btn, "clicked", G_CALLBACK(show_login_screen), NULL);
    gtk_box_pack_start(GTK_BOX(app.content_box), logout_btn, FALSE, FALSE, 10);

    gtk_widget_show_all(app.content_box);
}

// ============ MAIN APPLICATION ============
static void activate(GtkApplication *gtk_app, gpointer user_data)
{
    app.main_window = gtk_application_window_new(gtk_app);
    gtk_window_set_title(GTK_WINDOW(app.main_window), "Carpooling System");
    gtk_window_set_default_size(GTK_WINDOW(app.main_window), 800, 600);
    gtk_window_set_position(GTK_WINDOW(app.main_window), GTK_WIN_POS_CENTER);

    app.content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(app.main_window), app.content_box);

    show_login_screen();
    gtk_widget_show_all(app.main_window);
}

int main(int argc, char **argv)
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

    GtkApplication *gtk_app = gtk_application_new("com.university.carpool", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(gtk_app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(gtk_app), argc, argv);
    g_object_unref(gtk_app);

    save_all_data();
    free_all_memory();
    return status;
}
