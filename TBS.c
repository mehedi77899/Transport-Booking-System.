#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FILENAME "userinfo.txt"
#define ADMINFILE "admininfo.txt"
#define ADMINCODE "adminsecuritycode.txt"
#define TRANSPORTFILE "transport.txt"
#define BOOKINGFILE "booking.txt"
#include <conio.h>

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void printCentered(const char *text) {
    int width = 80;
    int len = strlen(text);
    int leftPadding = (width - len) / 2;
    if (leftPadding < 0) leftPadding = 0;

    for (int i = 0; i < leftPadding; i++) {
        putchar(' ');
    }
    printf("%s\n", text);
}

struct Transport {
    int id;
    char name[50];
    int totalSeats;
    int availableSeats;
    int price;
};

struct Booking {
    char username[25];
    int transportID;
    int seatsBooked;
};

void showAvailableTransport() {
    FILE *tfp = fopen(TRANSPORTFILE, "r");
    if (tfp==NULL) {
        printCentered("No transport data found!");
        return;
    }

    int id, total, available, price;
    char name[50];

    printCentered("--- Available Transport ---");
    printf("ID\tName\tTotal\tAvailable\tPrice\n");
    printf("------------------------------------------\n");

    while (fscanf(tfp, "%d %s %d %d %d", &id, name, &total, &available, &price) != EOF) {
        printf("%d\t%s\t%d\t%d\t\t%d\n", id, name, total, available, price);
    }

    fclose(tfp);
}

void showAvailableSeats() {
    int transportID;
    printf("Enter Transport ID to see available seats: ");
    scanf("%d", &transportID);

    FILE *tfp = fopen(TRANSPORTFILE, "r");
    if (tfp==NULL) {
        printCentered("No transport data found!");
        return;
    }

    int id, total, available, price;
    char name[50];
    int found = 0;

    while (fscanf(tfp, "%d %s %d %d %d", &id, name, &total, &available, &price) != EOF) {
        if (id == transportID) {
            printf("Transport: %s\nTotal Seats: %d\nAvailable Seats: %d\nPrice per seat: %d\n",
                   name, total, available, price);
            found = 1;
            break;
        }
    }
    fclose(tfp);

    if (found==0) {
        printCentered("Transport ID not found!");
    }
}

void bookSeat(char *username) {
    struct Transport transports[100];
    int count = 0;

    FILE *tfp = fopen(TRANSPORTFILE, "r");
    if (!tfp) {
        printCentered("No transport data found!");
        return;
    }

    while (fscanf(tfp, "%d %s %d %d %d",
                  &transports[count].id,
                  transports[count].name,
                  &transports[count].totalSeats,
                  &transports[count].availableSeats,
                  &transports[count].price) != EOF) {
        count++;
    }
    fclose(tfp);

    showAvailableTransport();

    int transportID, seatsToBook, found = 0;
    printf("Enter Transport ID to book: ");
    scanf("%d", &transportID);

    for (int i = 0; i < count; i++) {
        if (transports[i].id == transportID) {
            found = 1;
            printf("Enter number of seats to book: ");
            scanf("%d", &seatsToBook);

            if (seatsToBook > 0 && seatsToBook <= transports[i].availableSeats) {
                transports[i].availableSeats -= seatsToBook;

                tfp = fopen(TRANSPORTFILE, "w");
                for (int j = 0; j < count; j++) {
                    fprintf(tfp, "%d %s %d %d %d\n",
                            transports[j].id,
                            transports[j].name,
                            transports[j].totalSeats,
                            transports[j].availableSeats,
                            transports[j].price);
                }
                fclose(tfp);

                FILE *bfp = fopen(BOOKINGFILE, "a");
                fprintf(bfp, "%s %d %d\n", username, transportID, seatsToBook);
                fclose(bfp);

                printf("Booking successful! Total cost: %d\n", seatsToBook * transports[i].price);
            } else {
                printCentered("Invalid seat number or not enough seats available.");
            }
            break;
        }
    }
    if (found==0) {
        printCentered("Transport ID not found.");
    }
}

void seatManagement() {
    int transportID, newTotalSeats;
    FILE *tfp = fopen(TRANSPORTFILE, "r");
    if (tfp==NULL) {
        printCentered("No transport data found!");
        return;
    }

    struct Transport transports[100];
    int count = 0;
    while (fscanf(tfp, "%d %s %d %d %d",
                  &transports[count].id,
                  transports[count].name,
                  &transports[count].totalSeats,
                  &transports[count].availableSeats,
                  &transports[count].price) != EOF) {
        count++;
    }
    fclose(tfp);

    printCentered("Available transports:");
    for (int i = 0; i < count; i++) {
        printf("%d - %s (Total seats: %d, Available seats: %d)\n",
               transports[i].id, transports[i].name,
               transports[i].totalSeats, transports[i].availableSeats);
    }

    printf("Enter Transport ID to change seat capacity: ");
    scanf("%d", &transportID);

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (transports[i].id == transportID) {
            found = 1;
            printf("Enter new total seat capacity: ");
            scanf("%d", &newTotalSeats);

            int bookedSeats = transports[i].totalSeats - transports[i].availableSeats;
            if (newTotalSeats < bookedSeats) {
                printCentered("New capacity cannot be less than seats already booked.");
                return;
            }

            transports[i].totalSeats = newTotalSeats;
            transports[i].availableSeats = newTotalSeats - bookedSeats;

            printCentered("Seat capacity updated successfully.");
            break;
        }
    }

    if (found==0) {
        printCentered("Transport ID not found.");
        return;
    }

    tfp = fopen(TRANSPORTFILE, "w");
    for (int i = 0; i < count; i++) {
        fprintf(tfp, "%d %s %d %d %d\n",
                transports[i].id,
                transports[i].name,
                transports[i].totalSeats,
                transports[i].availableSeats,
                transports[i].price);
    }
    fclose(tfp);
}

void addTransport() {
    struct Transport t;

    printf("Enter new Transport ID: ");
    scanf("%d", &t.id);
    printf("Enter Transport name: ");
    scanf("%s", t.name);
    printf("Enter total seats: ");
    scanf("%d", &t.totalSeats);
    printf("Enter price per seat: ");
    scanf("%d", &t.price);

    t.availableSeats = t.totalSeats;

    FILE *tfp = fopen(TRANSPORTFILE, "r");
    struct Transport filetransport;
    while (tfp && fscanf(tfp, "%d %s %d %d %d",
                         &filetransport.id, filetransport.name, &filetransport.totalSeats, &filetransport.availableSeats, &filetransport.price) != EOF) {
        if (filetransport.id == t.id) {
            printCentered("Transport ID already exists!");
            fclose(tfp);
            return;
        }
    }
    fclose(tfp);

    tfp = fopen(TRANSPORTFILE, "a");
    if (tfp==NULL) {
        printCentered("Failed to open transport file.");
        return;
    }
    fprintf(tfp, "%d %s %d %d %d\n", t.id, t.name, t.totalSeats, t.availableSeats, t.price);
    fclose(tfp);

    printCentered("Transport added successfully!");
}

void removeTransport() {
    int removeID;
    printf("Enter Transport ID to remove: ");
    scanf("%d", &removeID);

    FILE *tfp = fopen(TRANSPORTFILE, "r");
    if (!tfp) {
        printCentered("No transport data found!");
        return;
    }

    struct Transport transports[100];
    int count = 0;
    int found = 0;

    while (fscanf(tfp, "%d %s %d %d %d",
                  &transports[count].id,
                  transports[count].name,
                  &transports[count].totalSeats,
                  &transports[count].availableSeats,
                  &transports[count].price) != EOF) {
        if (transports[count].id == removeID) {
            found = 1;
        } else {
            count++;
        }
    }
    fclose(tfp);

    if (found == 0) {
        printCentered("Transport ID not found.");
        return;
    }

    tfp = fopen(TRANSPORTFILE, "w");
    for (int i = 0; i < count; i++) {
        fprintf(tfp, "%d %s %d %d %d\n",
                transports[i].id,
                transports[i].name,
                transports[i].totalSeats,
                transports[i].availableSeats,
                transports[i].price);
    }
    fclose(tfp);

    printCentered("Transport removed successfully!");
}

void adminViewUserDetails() {
    FILE *ufp = fopen(FILENAME, "r");
    if (ufp == NULL) {
        printCentered("No user data found!");
        return;
    }

    char username[25], password[25];
    int count = 0;

    printCentered("----- Registered Users -----");
    printf("Username\tPassword\n");
    printf("----------------------------\n");

    while (fscanf(ufp, "%s %s", username, password) != EOF) {
        printf("%s\t\t%s\n", username, password);
        count++;
    }
    fclose(ufp);

    if (count == 0) {
        printCentered("No users registered yet!");
    }
}
void adminviewbookinghistory() {
    int transportID;
    printf("Enter Transport ID to view booking history: ");
    scanf("%d", &transportID);

    FILE *bfp = fopen(BOOKINGFILE, "r");
    if (bfp == NULL) {
        printCentered("No booking history found!");
        return;
    }

    char username[25];
    int bTransportID, seatsBooked;
    int found = 0;

    printCentered("----- Booking History -----");
    printf("Username\tSeats Booked\n");
    printf("----------------------------\n");

    while (fscanf(bfp, "%s %d %d", username, &bTransportID, &seatsBooked) != EOF) {
        if (bTransportID == transportID) {
            printf("%s\t\t%d\n", username, seatsBooked);
            found = 1;
        }
    }

    fclose(bfp);

    if (found==0) {
        printCentered("No bookings found for this Transport ID.");
    }

}
void userviewbookinghistory(const char *username) {
    FILE *bfp = fopen(BOOKINGFILE, "r");
    if (bfp == NULL) {
        printCentered("No booking history found!");
        return;
    }

    char fileUsername[25];
    int transportID, seatsBooked;
    int found = 0;

    printCentered("----- Your Booking History -----");
    printf("Transport ID\tSeats Booked\n");
    printf("-------------------------------\n");

    while (fscanf(bfp, "%s %d %d", fileUsername, &transportID, &seatsBooked) != EOF) {
        if (strcmp(username, fileUsername) == 0) {
            printf("%d\t\t%d\n", transportID, seatsBooked);
            found = 1;
        }
    }
    fclose(bfp);

    if (!found) {
        printCentered("You have no booking history.");
    }
}


void userDashboard(char *username) {
    char choice[4];
    while (1) {
        clearScreen();
        printCentered("--- User Dashboard ---");
        printf("1. Available Transport\n");
        printf("2. Available Seat\n");
        printf("3. Book Seat\n");
        printf("4. Cancel Seat\n");
        printf("5. Payment\n");
        printf("6. Booking History\n");
        printf("7. Payment History\n");
        printf("8. Logout\n");
        printf("----------------------\n");
        printf("Enter your choice: ");
        scanf("%s", choice);

        switch (choice[0]) {
            case '1':
                clearScreen();
                showAvailableTransport();
                break;
            case '2':
                clearScreen();
                showAvailableSeats();
                break;
            case '3':
                clearScreen();
                bookSeat(username);
                break;
            case '4':
                clearScreen();
                printCentered("Feature is underdevelopment!!");
                break;
            case '5':
                clearScreen();
                printCentered("Feature is underdevelopment!!");
                break;
            case '6':
                clearScreen();
                userviewbookinghistory(username);
                break;
            case '7':
                clearScreen();
                printCentered("Feature is underdevelopment!!");
                break;
            case '8':
                printCentered("Logging out :(");
                return;
            default:
                printCentered("Invalid choice. Try again!!");
        }
        printf("\nPress Enter to continue...");
        while(getchar()!='\n');
        getchar();
    }
}

void adminDashboard() {
    char choice[4];
    while (1) {
        clearScreen();
        printCentered("----- Admin Dashboard -----");
        printf("1. User Details\n");
        printf("2. Remove Unregistered User\n");
        printf("3. Give Discount\n");
        printf("4. Seat Management\n");
        printf("5. Payment History\n");
        printf("6. Booking History\n");
        printf("7. Transport Manage\n");
        printf("8. Logout\n");
        printf("----------------------\n");
        printf("Enter your choice: ");
        scanf("%s", choice);

        switch (choice[0]) {
            case '1':
                clearScreen();
                adminViewUserDetails();
                break;
            case '2':
                clearScreen();
                printCentered("Feature is underdevelopment!!");
                break;
            case '3':
                clearScreen();
                printCentered("Feature is underdevelopment!!");
                break;
            case '4':
                clearScreen();
                seatManagement();
                break;
            case '5':
                clearScreen();
                printCentered("Feature is underdevelopment!!");
                break;
            case '6':
                clearScreen();
                 adminviewbookinghistory();
                break;
            case '7': {
                clearScreen();
                char adminChoice[4];
                printCentered("Transport Management:");
                printf("1. Add Transport\n2. Remove Transport\nChoose: ");
                scanf("%s", adminChoice);
                if (adminChoice[0] == '1') {
                    addTransport();
                } else if (adminChoice[0] == '2') {
                    removeTransport();
                } else {
                    printCentered("Invalid choice.");
                }
                break;
            }
            case '8':
                printCentered("Logging out :(");
                return;
            default:
                printCentered("Invalid choice. Try again!!");
        }
        printf("\nPress Enter to continue...");
        while(getchar()!='\n');
        getchar();
    }
}

void userregistration() {
    char username[25], userpass[25];
    char fileusername[25], fileuserpass[25];
    int exist = 0;

    FILE *ufp = fopen(FILENAME, "a");
    if (ufp == NULL) {
        printCentered("File is not open yet!!");
    } else {
        fclose(ufp);
        printf("Enter User name: ");
        scanf("%s", username);
        ufp = fopen(FILENAME, "r");

        while (fscanf(ufp, "%s %s", fileusername, fileuserpass) != EOF) {
            if (strcmp(username, fileusername) == 0) {
                exist = 1;
                fclose(ufp);
                break;
            }
        }

        if (exist == 1) {
            printCentered("User name already exists!! Try again!!");
            return;
        }

        printf("Enter Password: ");
        int i = 0;
        char ch;
        while ((ch = getch()) != '\r') {
            if (ch == 8) {
                if (i > 0) {
                    i--;
                    printf("\b \b");
                }
            } else {
                userpass[i++] = ch;
                printf("*");
            }
        }
        userpass[i] = '\0';

        ufp = fopen(FILENAME, "a");
        fprintf(ufp, "%s %s\n", username, userpass);
        fclose(ufp);

        printCentered("Registration Successful!!");
        }
}


int userlogin() {
    char username[25], userpass[25];
    char fileusername[25], fileuserpass[25];
    int found = 0;

    printf("Enter User name: ");
    scanf("%s", username);

    printf("Enter Password: ");
    int i = 0;
    char ch;

    while ((ch = getch()) != '\r') {
        if (ch == 8) {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            userpass[i++] = ch;
            printf("*");
        }
    }
    userpass[i] = '\0';

    FILE *ufp = fopen(FILENAME, "r");
    if (ufp == NULL) {
        printCentered("No users found. Please register first.");
        return 0;
    } else {

        while (fscanf(ufp, "%s %s", fileusername, fileuserpass) != EOF) {
            if (strcmp(username, fileusername) == 0 && strcmp(userpass, fileuserpass) == 0) {
                found = 1;
                break;
            }
        }
        fclose(ufp);

        if (found == 1) {
            printCentered("Login successful!!");

            return 1;
        } else {
            printCentered("Invalid username or password.");
            return 0;
      }
   }
}


void adminRegistration() {
    char adminname[25], adminpass[25], securitycode[25];
    char fileadmin[25], filepass[25], filesecuritycode[25];
    int exist = 0, flag = 0;

    FILE *afp = fopen(ADMINFILE, "a");
    fclose(afp);
    FILE *scf = fopen(ADMINCODE, "a");
    fclose(scf);

    afp = fopen(ADMINFILE, "r");
    if (afp == NULL) {
        printCentered("File is not open yet!!");
        return;
    } else {
        printf("Enter Admin name: ");
        scanf("%s", adminname);

        while (fscanf(afp, "%s %s", fileadmin, filepass) != EOF) {
            if (strcmp(adminname, fileadmin) == 0) {
                exist = 1;
                fclose(afp);
                break;
            }
        }

        if (exist == 1) {
            printCentered("Admin name already exists!! Try again!!");
            return;
        }

        FILE *scf = fopen(ADMINCODE, "r");
        if (scf == NULL) {
            printCentered("Security code file not found!!");
            return;
        } else {
            printf("Enter Security code: ");
            int i = 0;
            char ch;
            while ((ch = getch()) != '\r') {
                if (ch == 8) {
                    if (i > 0) {
                        i--;
                        printf("\b \b");
                    }
                } else {
                    securitycode[i++] = ch;
                    printf("*");
                }
            }
            securitycode[i] = '\0';

            fclose(scf);

            while (fscanf(scf, "%s", filesecuritycode) != EOF) {
                if (strcmp(securitycode, filesecuritycode) == 0) {
                    flag = 1;
                    break;
                }
            }

            if (flag == 1) {

                printf("\nEnter Password: ");
                i = 0;
                while ((ch = getch()) != '\r') {
                    if (ch == 8) {
                        if (i > 0) {
                            i--;
                            printf("\b \b");
                        }
                    } else {
                        adminpass[i++] = ch;
                        printf("*");
                    }
                }
                adminpass[i] = '\0';


                afp = fopen(ADMINFILE, "a");
                fprintf(afp, "%s %s %s\n", adminname, adminpass, securitycode);
                fclose(afp);

                printCentered("Admin Registration Successful!!");
            } else {
                printCentered("Invalid security code!!");
            }
            }
            }
}

int adminLogin() {
    char adminname[25], adminpass[25];
    char fileadmin[25], filepass[25];
    int found = 0;

    printf("Enter Admin name: ");
    scanf("%s", adminname);


    printf("Enter Password: ");
    int i = 0;
    char ch;
    while ((ch = getch()) != '\r') {
        if (ch == 8) {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            adminpass[i++] = ch;
            printf("*");
        }
    }
    adminpass[i] = '\0';

    FILE *afp = fopen(ADMINFILE, "r");
    if (afp == NULL) {
        printCentered("No admin found. Please register first.");
        return 0;
    } else {
        while (fscanf(afp, "%s %s", fileadmin, filepass) != EOF) {
            if (strcmp(adminname, fileadmin) == 0 && strcmp(adminpass, filepass) == 0) {
                found = 1;
                break;
            }
        }
        fclose(afp);

        if (found == 1) {
            printCentered("Admin Login successful!!");
            adminDashboard();
            return 1;
        } else {
            printCentered("Invalid admin name or password.");
            return 0;
            }
 }
}

int main() {
    char choice[4];

    while (1) {
        clearScreen();
        printCentered("        ,----,                                  ");
        printCentered("      ,/   .`|                                  ");
        printCentered("    ,`   .'  :     ,---,.    .--.--.            ");
        printCentered("  ;    ;     /   ,'  .'  \\  /  /    '.          ");
        printCentered(".'_,/    ,'  ,---.' .' | |  :  /`. /          ");
        printCentered("|    :     |   |   |  |: | ;  |  |--`           ");
        printCentered(";    |.';  ;   :   :  :  / |  :  ;_             ");
        printCentered("`----'  |  |   :   |    ;   \\  \\    `.          ");
        printCentered("    '   :  ;   |   :     \\   `----.   \\         ");
        printCentered("    |   |  '   |   |   . |   __ \\  \\  |         ");
        printCentered("    '   :  |   '   :  '; |  /  /`--'  /         ");
        printCentered("    ;   |.'    |   |  | ;  '--'.     /          ");
        printCentered("    '---'      |   :   /     `--'---'           ");
        printCentered("               |   | ,'                         ");
        printCentered("               `----'                           ");
        printCentered("");
        printCentered("You want seat,We want money!!(:");
        printCentered("");
        printCentered("====== MENU ======");
        printf("1. User Registration\n");
        printf("2. User Login\n");
        printf("3. Admin Registration\n");
        printf("4. Admin Login\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%s", choice);
switch (choice[0]) {
            case '1':
                clearScreen();
                printCentered("\nWelcome for registration!!\n");
                userregistration();
                break;
            case '2':
                clearScreen();
                 printf("\n\n\n");
                userlogin();
                break;
            case '3':
                clearScreen();
                printCentered("\nAdmin registration!!\n");
                adminRegistration();
                break;
            case '4':
                clearScreen();
                 printf("\n\n\n");
                adminLogin();
                break;
            case '5':
                clearScreen();

                printCentered("Ok, Bye!!");
                printCentered(" Have a good day sir!!\n");
                exit(0);
            default:
                clearScreen();
                printf("\n\n\n");
                printCentered("Invalid choice. Try again!!\n");
        }
        printCentered("\nPress Enter to continue...");
        while(getchar()!='\n');
        getchar();
    }
return 0;
}
