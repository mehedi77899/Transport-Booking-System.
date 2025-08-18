#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>


#define FILENAME "userinfo.txt"
#define ADMINFILE "admininfo.txt"
#define ADMINCODE "adminsecuritycode.txt"
#define TRANSPORTFILE "transport.txt"
#define BOOKINGFILE "booking.txt"
#define PROMOFILE "adminpromocode.txt"
#define CARDFILE "cardinfo.txt"
#define BNRFILE "bnrinfo.txt"
#define USERPROMOFILE "userpromofile.txt"
#define PAYMENTFILE "paymentfile.txt"
#define REVIEWFILE "reviews.txt"


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
    char startLocation[50];
    char endLocation[50];
};

void cancelSeat(char *username) {
    FILE *bfp = fopen(BOOKINGFILE,"r");
    if(bfp==NULL){
        printCentered("No bookings found!\n");
        return;
    }

    struct Booking {
        char user[50];
        int transportID;
        int seats;
        char start[50];
        char end[50];
    } bookings[100];

    int count = 0;
    while(fscanf(bfp,"%s %d %d %s %s", bookings[count].user, &bookings[count].transportID,
                 &bookings[count].seats, bookings[count].start, bookings[count].end)!=EOF){
        count++;
    }
    fclose(bfp);

    int found = 0;
    printf("\nYour bookings:\n");
    for(int i=0;i<count;i++){
        if(strcmp(bookings[i].user, username)==0){
            found = 1;
            printf("%d. Transport ID: %d, Seats booked: %d, Route: %s to %s\n",
                   i+1, bookings[i].transportID, bookings[i].seats, bookings[i].start, bookings[i].end);
        }
    }

    if(found==0){
        printCentered("You have no bookings!\n");
        return;
    }

    int choice;
    printf("\nEnter the number of the booking you want to cancel: ");
    scanf("%d",&choice);
    choice--;

    if(choice<0 || choice>=count || strcmp(bookings[choice].user, username)!=0){
        printCentered("\n\nInvalid choice!\n");
        return;
    }

    int seatsToCancel;
    printf("Enter number of seats to cancel: ");
    scanf("%d", &seatsToCancel);

    if(seatsToCancel <=0 || seatsToCancel > bookings[choice].seats){
        printCentered("Invalid number of seats!\n");
        return;
    }

    int refundSeats = seatsToCancel;
    int refundAmount = (refundSeats * 500 * 35) / 100;
    bookings[choice].seats -= seatsToCancel;

    if(bookings[choice].seats==0){
        for(int i=choice;i<count-1;i++){
            bookings[i] = bookings[i+1];
        }
        count--;
    }
    printf("You cancelled %d seats.\n", seatsToCancel);
    printf("You will get refund: %d BDT\n", refundAmount);

    int method;
    printf("Select refund method:\n1. Card\n2. Bkash/Nagad\nEnter choice: ");
    scanf("%d", &method);

    if(method==1){
        int cardnum, cardpin;
        printf("Enter your card number: ");
        scanf("%d",&cardnum);
        printf("Enter your card pin: ");
        scanf("%d",&cardpin);

        FILE *card = fopen(CARDFILE,"r");
        if(card==NULL){
            printCentered("Card file not found!\n");
            return;
        }

        int cardNums[100], cardPins[100], cardBalances[100], n=0, foundCard=0;
        while(fscanf(card,"%d %d %d",&cardNums[n], &cardPins[n], &cardBalances[n])==3){
            if(cardNums[n]==cardnum && cardPins[n]==cardpin){
                foundCard=1;
                cardBalances[n]+=refundAmount;
            }
            n++;
        }
        fclose(card);

        if(!foundCard){
            printCentered("Card not found!\n");
            return;
        }

        card = fopen(CARDFILE,"w");
        for(int i=0;i<n;i++){
            fprintf(card,"%d %d %d\n", cardNums[i], cardPins[i], cardBalances[i]);
        }
        fclose(card);
    }
    else if(method==2){
        int mbnum, pin;
        printf("Enter your mobile number: ");
        scanf("%d",&mbnum);
        printf("Enter Pin: ");
        scanf("%d",&pin);

        FILE *mb = fopen(BNRFILE,"r");
        if(mb==NULL){
            printCentered("Mobile file not found!\n");
            return;
        }

        int mbNums[100], mbPins[100], mbBalances[100], n=0, foundMB=0;
        while(fscanf(mb,"%d %d %d",&mbNums[n], &mbPins[n], &mbBalances[n])==3){
            if(mbNums[n]==mbnum && mbPins[n]==pin){
                foundMB=1;
                mbBalances[n]+=refundAmount;
            }
            n++;
        }
        fclose(mb);

        if(!foundMB){
            printCentered("Mobile number not found!\n");
            return;
        }

        mb = fopen(BNRFILE,"w");
        for(int i=0;i<n;i++){
            fprintf(mb,"%d %d %d\n", mbNums[i], mbPins[i], mbBalances[i]);
        }
        fclose(mb);
    }

    bfp = fopen(BOOKINGFILE,"w");
    for(int i=0;i<count;i++){
        fprintf(bfp,"%s %d %d %s %s\n", bookings[i].user, bookings[i].transportID, bookings[i].seats,
                bookings[i].start, bookings[i].end);
    }
    fclose(bfp);

    struct Transport transports[100];
    int tcount = 0;
    FILE *tfp = fopen(TRANSPORTFILE,"r");
    if(tfp!=NULL){
        while(fscanf(tfp,"%d %s %d %d %d %s %s", &transports[tcount].id, transports[tcount].name,
                     &transports[tcount].totalSeats, &transports[tcount].availableSeats,
                     &transports[tcount].price, transports[tcount].startLocation,
                     transports[tcount].endLocation)!=EOF){
            tcount++;
        }
        fclose(tfp);

        for(int i=0;i<tcount;i++){
            if(transports[i].id == bookings[choice].transportID){
                transports[i].availableSeats += seatsToCancel;
                break;
            }
        }

        tfp = fopen(TRANSPORTFILE,"w");
        for(int i=0;i<tcount;i++){
            fprintf(tfp,"%d %s %d %d %d %s %s\n",
                    transports[i].id, transports[i].name,
                    transports[i].totalSeats, transports[i].availableSeats,
                    transports[i].price, transports[i].startLocation, transports[i].endLocation);
        }
        fclose(tfp);
    }


    printf("Refund successful!\n");
}



void showAvailableTransport() {
    char from[50], to[50];
    printf("\nEnter starting location: ");
    scanf("%s", from);
    printf("\nEnter destination: ");
    scanf("%s", to);

    FILE *tfp = fopen(TRANSPORTFILE, "r");
    if (tfp == NULL) {
        printCentered("No transport data found!");
        return;
    }

    struct Transport t;
    int found = 0;

    printCentered("\nAvailable Transports for your route:");
    printf("----------------------------------------------------------------\n");
    printf("ID   Name        Route              Total   Available   Price\n");
    printf("----------------------------------------------------------------\n");

    while (fscanf(tfp, "%d %s %d %d %d %s %s",
                  &t.id, t.name,
                  &t.totalSeats, &t.availableSeats,
                  &t.price,
                  t.startLocation, t.endLocation) != EOF) {
        if (strcmp(t.startLocation, from) == 0 && strcmp(t.endLocation, to) == 0) {
            printf("%-4d %-10s %-10sto %-10s %-6d %-10d %-6d\n",
                   t.id, t.name,
                   t.startLocation, t.endLocation,
                   t.totalSeats, t.availableSeats, t.price);
            found = 1;
        }
    }

    printf("----------------------------------------------------------------\n");

    fclose(tfp);

    if (found == 0) {
        printCentered("\nNo transport available for this route!!\n");
    }
}


void bookSeat(char *username) {
    struct Transport transports[100];
    int count = 0;
    int valid=0;
    char promocode[10], filepromocode[10];
    int filediscount, promoUsed = 0;


    showAvailableTransport();

    FILE *tfp = fopen(TRANSPORTFILE, "r");
    if (tfp == NULL) {
        printCentered("\nNo transport data found!\n");
        return;
    }

    while (fscanf(tfp, "%d %s %d %d %d %s %s",
                  &transports[count].id,
                  transports[count].name,
                  &transports[count].totalSeats,
                  &transports[count].availableSeats,
                  &transports[count].price,
                  transports[count].startLocation,
                  transports[count].endLocation) != EOF) {
        count++;
    }
    fclose(tfp);
    printCentered("\n\n====1** ID means Ac and 2** ID means Non-Ac====\n");
    printf("====3** ID means Slepper-Coach and 4** ID means BussinessClass====\n\n");

    int transportID;
    printf("\nEnter Transport ID to book: ");
    scanf("%d", &transportID);

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (transports[i].id == transportID) {
            found = 1;

            printf("\nSelected Transport: %s (ID: %d)\nRoute: %s to %s\nAvailable Seats: %d\nPrice per seat: %d\n",
                   transports[i].name, transports[i].id,
                   transports[i].startLocation, transports[i].endLocation,
                   transports[i].availableSeats, transports[i].price);

            int seatsToBook;
            printf("Enter number of seats to book: ");
            scanf("%d", &seatsToBook);

            if (seatsToBook <= 0 || seatsToBook > transports[i].availableSeats) {
                printCentered("Invalid seat number or not enough seats available.");
                return;
            }

            int cost = seatsToBook * transports[i].price;

            if(cost>=999 && cost<=1399) {
                    cost -= 75;
            }
            else if(cost>=1400 && cost<=1999){
                    cost -= 150;
            }
            else if(cost>=2000){
                    cost -= 300;
            }

            int p;
            printf("Enter 1 to use promocode: ");
            scanf("%d", &p);
            if(p==1){

                printf("Enter promocode: ");
                scanf("%s", promocode);

                FILE *userPromo = fopen(USERPROMOFILE, "r");
                if(userPromo != NULL){
                    char usedUser[50], usedCode[10];
                    while(fscanf(userPromo,"%s %s", usedUser, usedCode) != EOF){
                        if(strcmp(username, usedUser) == 0 && strcmp(promocode, usedCode) == 0){
                            promoUsed = 1;
                            break;
                        }
                    }
                    fclose(userPromo);
                }

                if(promoUsed==1){
                    printCentered("\nYou have already used this promocode!\n");
                }
                else {
                    FILE *promo = fopen(PROMOFILE, "r");
                    if(promo != NULL){

                        while(fscanf(promo,"%s %d", filepromocode, &filediscount) != EOF){
                            if(strcmp(promocode, filepromocode) == 0){
                                printf("\n\n===You got %d%% discount!!===\n\n", filediscount);
                                cost = cost - (cost * filediscount / 100);
                                valid = 1;
                                break;
                            }
                        }
                        fclose(promo);

                        if(valid==0){
                            printCentered("\nInvalid promocode!\n");
                        }

                    }
                }
            }

            printf("You need to pay %d BDT\n", cost);
            int k;
            printf("1. Pay by Card\n2. Pay by Bkash/Nagad\nEnter choice: ");
            scanf("%d", &k);
            int paymentSuccess = 0;

            if(k==1){
                int cardnum, cardpin;
                printf("Enter your card number: ");
                scanf("%d", &cardnum);
                printf("Enter your card pin: ");
                scanf("%d", &cardpin);
                FILE *card = fopen(CARDFILE,"a");
                fclose(card);

                card = fopen(CARDFILE,"r");
                if(card == NULL){
                    printCentered("Card file not found!\n");
                    return;
                }

                int cardNums[100], cardPins[100], cardBalances[100], n=0, foundCard=0;
                while(fscanf(card,"%d %d %d",&cardNums[n], &cardPins[n], &cardBalances[n])!=EOF){
                    if(cardNums[n]==cardnum && cardPins[n]==cardpin){
                        foundCard=1;
                        if(cost>cardBalances[n]){
                            printCentered("===You do not have sufficient Balance!!===\n");
                            fclose(card);
                            return;
                        }
                        cardBalances[n]-=cost;
                        paymentSuccess=1;
                        printCentered("\n\n===Payment complete===\n\n");
                    }
                    n++;
                }
                fclose(card);

                if(foundCard==0){
                    printCentered("\nCard not found!\n");
                    return;
                }

                card = fopen(CARDFILE,"w");
                for(int i=0;i<n;i++){
                    fprintf(card,"%d %d %d\n", cardNums[i], cardPins[i], cardBalances[i]);
                }
                fclose(card);
            }
            else if(k==2){
                int mbnum, pin;
                printf("Enter your mobile number: ");
                scanf("%d", &mbnum);
                printf("Enter Pin: ");
                scanf("%d", &pin);

                FILE *mb = fopen(BNRFILE,"a");
                fclose(mb);
                mb = fopen(BNRFILE,"r");
                if(mb==NULL){
                    printCentered("Mobile file not found!\n");
                    return;
                }

                int mbNums[100], mbPins[100], mbBalances[100], n=0, foundMB=0;
                while(fscanf(mb,"%d %d %d",&mbNums[n], &mbPins[n], &mbBalances[n])!=EOF){
                    if(mbNums[n]==mbnum && mbPins[n]==pin){
                        foundMB=1;
                        if(cost>mbBalances[n]){
                            printCentered("===You do not have sufficient Balance!!===\n");
                            fclose(mb);
                            return;
                        }
                        mbBalances[n]-=cost;
                        paymentSuccess=1;
                        printCentered("\n\n===Payment complete===\n\n");
                    }
                    n++;
                }
                fclose(mb);

                if(foundMB==0){
                    printCentered("\nMobile number or Pin is wrong!\n");
                    return;
                }

                mb = fopen(BNRFILE,"w");
                for(int i=0;i<n;i++){
                    fprintf(mb,"%d %d %d\n", mbNums[i], mbPins[i], mbBalances[i]);
                }
                fclose(mb);
            }

            if(paymentSuccess==0){
                printCentered("\nPayment failed! Check details.\n");
                return;
            }

            transports[i].availableSeats -= seatsToBook;
            tfp = fopen(TRANSPORTFILE, "w");
            for(int j=0;j<count;j++){
                fprintf(tfp,"%d %s %d %d %d %s %s\n",
                        transports[j].id, transports[j].name,
                        transports[j].totalSeats, transports[j].availableSeats,
                        transports[j].price,
                        transports[j].startLocation, transports[j].endLocation);
            }
            fclose(tfp);

            FILE *bfp = fopen(BOOKINGFILE, "a");
            fprintf(bfp, "%s %d %d %s %s\n", username, transports[i].id, seatsToBook,
                    transports[i].startLocation, transports[i].endLocation);
            fclose(bfp);

            FILE *payHist = fopen(PAYMENTFILE,"a");
            fprintf(payHist,"%s %d %s %d %d\n",
                    username, transports[i].id, transports[i].name,
                    seatsToBook, cost);
            fclose(payHist);
            if(valid==1&&p==1){
            FILE *userPromoOut = fopen(USERPROMOFILE, "a");
            fprintf(userPromoOut, "%s %s\n", username, promocode);
            fclose(userPromoOut);
            }

            printf("Booking successful! Total paid: %d\n", cost);
            break;
        }
    }

    if(found==0){
        printCentered("===Transport ID not found===\n");
    }
}

void giveReview(char username[]) {
    int transportID, found = 0;
    char bookedUser[25], startLocation[50], endLocation[50];
    int bookedID, seats;
    char review[200];

    printf("Enter Transport ID to give review: ");
    scanf("%d", &transportID);

    FILE *bfp = fopen(BOOKINGFILE, "r");
    if (bfp == NULL) {
        printf("No booking records found!\n");
        return;
    }

    while (fscanf(bfp, "%s %d %d %s %s", bookedUser, &bookedID, &seats, startLocation, endLocation) !=EOF) {
        if (strcmp(username, bookedUser) == 0 && bookedID == transportID) {
            found = 1;
            break;
        }
    }
    fclose(bfp);

    if (!found) {
        printf("Sorry! You have not booked this transport, so you can't give review.\n");
        return;
    }

    printf("Write your review: ");
    getchar();
    fgets(review, sizeof(review), stdin);
    review[strcspn(review, "\n")] = 0;

    FILE *rfp = fopen(REVIEWFILE, "a");
    if (rfp == NULL) {
        printf("Error saving review!\n");
        return;
    }
    fprintf(rfp, "%s %d %s\n", username, transportID, review);
    fclose(rfp);

    printf("Review submitted successfully!\n");
}

void showReviews(int transportID) {
    FILE *rfp = fopen(REVIEWFILE, "r");
    if (rfp == NULL) {
        printf("No reviews found!\n");
        return;
    }

    char uname[25], review[200];
    int tid;
    int found = 0;

    printf("\n--- Reviews for Transport ID %d ---\n", transportID);
    while (fscanf(rfp, "%s %d %[^\n]", uname, &tid, review) !=EOF) {
        if (tid == transportID) {
            found = 1;
            printf("%s: %s\n", uname, review);
        }
    }
    if (!found) {
        printf("No reviews yet!\n");
    }
    fclose(rfp);
}


void viewPaymentHistory(char *username) {
    FILE *payHist = fopen(PAYMENTFILE, "r");
    if (payHist == NULL) {
        printCentered("\nNo payment history available!\n");
        return;
    }

    char fileUser[50], transportName[50];
    int transportID, seats, amount;
    int found = 0;

    printf("\n\n========== Payment History ==========\n");
    printf("%-15s %-12s %-15s %-10s %-10s\n",
           "Username", "BusID", "BusName", "Seats", "Amount(BDT)");
    printf("--------------------------------------------------------------\n");

    while (fscanf(payHist, "%s %d %s %d %d",
                  fileUser, &transportID, transportName, &seats, &amount) != EOF) {
        if (strcmp(fileUser, username) == 0) {
            printf("%-15s %-12d %-15s %-10d %-10d\n",
                   fileUser, transportID, transportName, seats, amount);
            found = 1;
        }
    }

    if (found==0) {
        printCentered("\nNo payment history found for this user!\n");
    }

    printf("=====================================\n");
    fclose(payHist);
}


void seatManagement() {
    int transportID, newTotalSeats;
    FILE *tfp = fopen(TRANSPORTFILE, "r");
    if (tfp == NULL) {
        printCentered("No transport data found!\n");
        return;
    }

    struct Transport transports[100];
    int count = 0;
    while (fscanf(tfp, "%d %s %d %d %d %s %s",
                  &transports[count].id,
                  transports[count].name,
                  &transports[count].totalSeats,
                  &transports[count].availableSeats,
                  &transports[count].price,
                  transports[count].startLocation,
                  transports[count].endLocation) != EOF) {
        count++;
    }
    fclose(tfp);

    printCentered("\n\n====Available transports====\n\n");
    for (int i = 0; i < count; i++) {
        printf("%d - %s  Route: %s to %s  Total: %d, Available: %d\n\n",
               transports[i].id, transports[i].name,
               transports[i].startLocation, transports[i].endLocation,
               transports[i].totalSeats, transports[i].availableSeats);
    }

    printf("\n\nEnter Transport ID to change seat capacity: ");
    scanf("%d", &transportID);

    int found = 0;
    for (int i = 0; i < count; i++) {
        if (transports[i].id == transportID) {
            found = 1;
            printf("Enter new total seat capacity: ");
            scanf("%d", &newTotalSeats);

            int bookedSeats = transports[i].totalSeats - transports[i].availableSeats;
            if (newTotalSeats < bookedSeats) {
                printCentered("New capacity cannot be less than seats already booked.\n");
                return;
            }

            transports[i].totalSeats = newTotalSeats;
            transports[i].availableSeats = newTotalSeats - bookedSeats;

            printCentered("Seat capacity updated successfully.\n");
            break;
        }
    }

    if (found == 0) {
        printCentered("Transport ID not found.\n");
        return;
    }

    tfp = fopen(TRANSPORTFILE, "w");
    if (tfp==NULL) {
        printCentered("Failed to open file for writing.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(tfp, "%d %s %d %d %d %s %s\n",
                transports[i].id,
                transports[i].name,
                transports[i].totalSeats,
                transports[i].availableSeats,
                transports[i].price,
                transports[i].startLocation,
                transports[i].endLocation);
    }
    fclose(tfp);
}


void addTransport() {
    struct Transport t;
    struct Transport filetransport;
    printf("Enter new Transport ID: ");
    scanf("%d", &t.id);
    printf("Enter Transport name: ");
    scanf("%s", t.name);
    printf("Enter total seats: ");
    scanf("%d", &t.totalSeats);
    printf("Enter price per seat: ");
    scanf("%d", &t.price);
    printf("Enter start location: ");
    scanf("%s", t.startLocation);
    printf("Enter end location: ");
    scanf("%s", t.endLocation);

    t.availableSeats = t.totalSeats;

    FILE *tfp = fopen(TRANSPORTFILE, "r");


    while (fscanf(tfp, "%d %s %d %d %d %s %s",
                         &filetransport.id, filetransport.name,
                         &filetransport.totalSeats, &filetransport.availableSeats,
                         &filetransport.price,
                         filetransport.startLocation, filetransport.endLocation) != EOF) {
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
    fprintf(tfp, "%d %s %d %d %d %s %s\n",
            t.id, t.name, t.totalSeats, t.availableSeats, t.price,
            t.startLocation, t.endLocation);
    fclose(tfp);

    printCentered("Transport added successfully!");
}

void removeTransport() {
    int removeID;
    printf("Enter Transport ID to remove: ");
    scanf("%d", &removeID);

    FILE *tfp = fopen(TRANSPORTFILE, "r");
    if (tfp==NULL) {
        printCentered("No transport data found!!\n");
        return;
    }

    struct Transport transports[100];
    int count = 0;
    int found = 0;

    while (fscanf(tfp, "%d %s %d %d %d %s %s",
                  &transports[count].id,
                  transports[count].name,
                  &transports[count].totalSeats,
                  &transports[count].availableSeats,
                  &transports[count].price,
                  transports[count].startLocation,
                  transports[count].endLocation) != EOF) {
        if (transports[count].id == removeID) {
            found = 1;
        }
        else {
            count++;
        }
    }
    fclose(tfp);

    if (found == 0) {
        printCentered("Transport ID not found.\n");
        return;
    }

    tfp = fopen(TRANSPORTFILE, "w");
    if (tfp==NULL) {
        printCentered("Failed to open file for writing.\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        fprintf(tfp, "%d %s %d %d %d %s %s\n",
                transports[i].id,
                transports[i].name,
                transports[i].totalSeats,
                transports[i].availableSeats,
                transports[i].price,
                transports[i].startLocation,
                transports[i].endLocation);
    }
    fclose(tfp);

    printCentered("Transport removed successfully!\n");
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

    char username[25],from[25],to[25];
    int bTransportID, seatsBooked;
    int found = 0;

    printCentered("----- Booking History -----");
    printf("Username\tSeats Booked\n");
    printf("----------------------------\n");

    while (fscanf(bfp, "%s %d %d %s %s", username, &bTransportID, &seatsBooked, from, to) != EOF) {
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

    if (found==0) {
        printCentered("You have no booking history.");
    }
}

void printUserDashboard() {
    char *menuTitle = "--- User Dashboard ---";
    char *menuItems[] = {
        "1. Available Transport",
        "2. Book Seat",
        "3. Cancel Booking Seat",
        "4. Booking History",
        "5. Payment History",
        "6. Give Review",
        "7. See Reviews",
        "8. Logout"
    };

    int itemCount = sizeof(menuItems) / sizeof(menuItems[0]);
    int width = 45;
    int consoleWidth = 80;
    int horizontalPadding = (consoleWidth - width) / 2;

    printf("\n\n");

    for (int j = 0; j < horizontalPadding; j++) printf(" ");
    for (int i = 0; i < width; i++) printf("#");
    printf("\n");

    int titleLen = strlen(menuTitle);
    for (int j = 0; j < horizontalPadding; j++) printf(" ");
    printf("# ");
    int titleSpace = (width - titleLen - 3) / 2;
    for (int i = 0; i < titleSpace; i++) printf(" ");
    printf("%s", menuTitle);
    for (int i = 0; i < width - titleLen - titleSpace - 3; i++) printf(" ");
    printf("#\n");

    for (int i = 0; i < itemCount; i++) {
        int len = strlen(menuItems[i]);
        for (int j = 0; j < horizontalPadding; j++) printf(" ");
        printf("# %s", menuItems[i]);
        for (int j = 0; j < width - len - 3; j++) printf(" ");
        printf("#\n");
    }

    for (int j = 0; j < horizontalPadding; j++) printf(" ");
    for (int i = 0; i < width; i++) printf("#");
    printf("\n");
}

void userDashboard(char *username) {
    char choice[4];
    while (1) {
        clearScreen();
        printUserDashboard();
        printf("\nEnter your choice: ");
        scanf("%s", choice);

        switch (choice[0]) {
            case '1':
                clearScreen();
                showAvailableTransport();
                break;
            case '2':
                clearScreen();
                bookSeat(username);
                break;
            case '3':
                clearScreen();
                cancelSeat(username);
                break;
            case '4':
                clearScreen();
                userviewbookinghistory(username);
                break;
            case '5':
                clearScreen();
                 viewPaymentHistory(username);
                break;
            case '6':
                clearScreen();
                giveReview(username);
                break;
            case '7':
                clearScreen();
                int tid;
                printf("Enter Transport ID to see reviews: ");
                scanf("%d", &tid);
                showReviews(tid);
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

void givediscount(){
char promocode[10],filepromocode[10];
int discount, filediscount;
FILE *pcf = fopen(PROMOFILE, "a");
if(pcf==NULL){
    printCentered("File is not open yet!!\n");
    return;
}
printf("Enter a unique promocode: ");
scanf("%s", promocode);
while (fscanf(pcf, "%s %d", filepromocode, &filediscount) != EOF){
  if (strcmp(promocode, filepromocode) == 0) {
    printCentered("Promocode already exist!!\nGive a unique one!!\n");
    fclose(pcf);
    return;
  }
}
printf("Enetr discount ammount: ");
scanf("%d", &discount);
fprintf(pcf,"%s %d\n", promocode, discount);
fclose(pcf);

}

void viewAllPayments() {
    FILE *payHist = fopen(PAYMENTFILE, "r");
    if (payHist == NULL) {
        printCentered("\nNo payment history available!\n");
        return;
    }

    char fileUser[50], transportName[50];
    int transportID, seats, amount;

    printf("\n\n========== All Payment History (Admin) ==========\n");
    printf("%-15s %-12s %-15s %-10s %-10s\n",
           "Username", "BusID", "BusName", "Seats", "Amount(BDT)");
    printf("--------------------------------------------------------------\n");

    while (fscanf(payHist, "%s %d %s %d %d",
                  fileUser, &transportID, transportName, &seats, &amount) != EOF) {
        printf("%-15s %-12d %-15s %-10d %-10d\n",
               fileUser, transportID, transportName, seats, amount);
    }

   printf("--------------------------------------------------------------\n");

    fclose(payHist);
}


void printAdminDashboard() {
    char *menuTitle = "----- Admin Dashboard -----";
    char *menuItems[] = {
        "1. User Details",
        "2. Give Discount",
        "3. Seat Management",
        "4. Payment History",
        "5. Booking History",
        "6. Transport Manage",
        "7. See Review",
        "8. Logout"
    };

    int itemCount = sizeof(menuItems) / sizeof(menuItems[0]);
    int width = 45;
    int consoleWidth = 80;
    int horizontalPadding = (consoleWidth - width) / 2;

    printf("\n\n");

    for (int j = 0; j < horizontalPadding; j++) printf(" ");
    for (int i = 0; i < width; i++) printf("#");
    printf("\n");

    int titleLen = strlen(menuTitle);
    for (int j = 0; j < horizontalPadding; j++) printf(" ");
    printf("# ");
    int titleSpace = (width - titleLen - 3) / 2;
    for (int i = 0; i < titleSpace; i++) printf(" ");
    printf("%s", menuTitle);
    for (int i = 0; i < width - titleLen - titleSpace - 3; i++) printf(" ");
    printf("#\n");

    for (int i = 0; i < itemCount; i++) {
        int len = strlen(menuItems[i]);
        for (int j = 0; j < horizontalPadding; j++) printf(" ");
        printf("# %s", menuItems[i]);
        for (int j = 0; j < width - len - 3; j++) printf(" ");
        printf("#\n");
    }

    for (int j = 0; j < horizontalPadding; j++) printf(" ");
    for (int i = 0; i < width; i++) printf("#");
    printf("\n");
}

void adminDashboard() {
    char choice[4];
    while (1) {
        clearScreen();
        printAdminDashboard();
        printf("Enter your choice: ");
        scanf("%s", choice);

        switch (choice[0]) {
            case '1':
                clearScreen();
                adminViewUserDetails();
                break;
            case '2':
                clearScreen();
                givediscount();
                break;
            case '3':
                clearScreen();
                seatManagement();
                break;
            case '4':
                clearScreen();
                viewAllPayments();
                break;
            case '5':
                clearScreen();
                 adminviewbookinghistory();
                break;
            case '6': {
                clearScreen();
                char adminChoice[4];
                printCentered("\n\n====Transport Management====\n\n");
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
            case '7':
                clearScreen();
                int tid;
                printf("Enter Transport ID to see reviews: ");
                scanf("%d", &tid);
                showReviews(tid);
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

void userregistration() {
    char username[25], userpass[25];
    char fileusername[25], fileuserpass[25];
    int exist = 0;

    FILE *ufp = fopen(FILENAME, "a");
    if (ufp == NULL) {
        printCentered("File is not open yet!!");
    }
    else {
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
        while(1){
            ch = getch();
            if(ch == 13) break;
            if(ch == 8){
                if(i>0){
                    i--;
                    printf("\b \b");
                }
            } else {
                userpass[i++] = ch;
                printf("*");
            }
        }
        userpass[i] = '\0';
        printf("\n");

        ufp = fopen(FILENAME, "a");
        fprintf(ufp, "%s %s\n", username, userpass);
        fclose(ufp);
        printCentered("Registration Successful!!\n");
    }
}

void userlogin() {
    char username[25], userpass[25];
    char fileusername[25], fileuserpass[25];
    int found = 0;

    printf("Enter User name: ");
    scanf("%s", username);

    printf("Enter password: ");
    int i = 0;
    char ch;
    while(1){
        ch = getch();
        if(ch == 13) break;
        if(ch == 8){
            if(i>0){
                i--;
                printf("\b \b");
            }
        } else {
            userpass[i++] = ch;
            printf("*");
        }
    }
    userpass[i] = '\0';
    printf("\n");

    FILE *ufp = fopen(FILENAME, "r");
    if (ufp == NULL) {
        printCentered("No users found. Please register first.");
        return;
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
            userDashboard(username);
        } else {
            printCentered("Invalid username or password.");
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
            scanf("%s", securitycode);

            while (fscanf(scf, "%s", filesecuritycode) != EOF) {
                if (strcmp(securitycode, filesecuritycode) == 0) {
                    flag = 1;
                    break;
                }
            }

            fclose(scf);

            if (flag == 1) {
                printf("Enter Password: ");
                int i = 0;
                char ch;
                while(1){
                    ch = getch();
                    if(ch == 13) break;
                    if(ch == 8){
                        if(i>0){
                            i--;
                            printf("\b \b");
                        }
                    } else {
                        adminpass[i++] = ch;
                        printf("*");
                    }
                }
                adminpass[i] = '\0';
                printf("\n");

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

void adminLogin() {
    char adminname[25], adminpass[25];
    char fileadmin[25], filepass[25];
    int found = 0;

    printf("Enter Admin name: ");
    scanf("%s", adminname);

    printf("Enter password: ");
    int i = 0;
    char ch;
    while(1){
        ch = getch();
        if(ch == 13) break;
        if(ch == 8){
            if(i>0){
                i--;
                printf("\b \b");
            }
        } else {
            adminpass[i++] = ch;
            printf("*");
        }
    }
    adminpass[i] = '\0';
    printf("\n");

    FILE *afp = fopen(ADMINFILE, "r");
    if (afp == NULL) {
        printCentered("No admin found. Please register first.");
        return;
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
        } else {
            printCentered("Invalid admin name or password.");
        }
    }
}


void printAlignedMenu() {
    char *menuTitle = "====== MENU ======";
    char *menuItems[] = {
        "1. User Registration",
        "2. User Login",
        "3. Admin Registration",
        "4. Admin Login",
        "5. Exit"
    };
    int lines = 6;
    int width = 40;
    int consoleWidth = 80;

    int horizontalPadding = (consoleWidth - width) / 2;

    for(int i = 0; i < 2; i++) printf("\n");

    for(int j = 0; j < horizontalPadding; j++) printf(" ");
    for(int i = 0; i < width; i++) printf("#");
    printf("\n");

    int titleLen = strlen(menuTitle);
    for(int j = 0; j < horizontalPadding; j++) printf(" ");
    printf("# ");
    int titleSpace = (width - titleLen - 3) / 2;
    for(int i = 0; i < titleSpace; i++) printf(" ");
    printf("%s", menuTitle);
    for(int i = 0; i < width - titleLen - titleSpace - 3; i++) printf(" ");
    printf("#\n");

    for(int i = 0; i < 5; i++) {
        int len = strlen(menuItems[i]);
        for(int j = 0; j < horizontalPadding; j++) printf(" ");
        printf("# ");
        printf("%s", menuItems[i]);
        for(int j = 0; j < width - len - 3; j++) printf(" ");
        printf("#\n");
    }

    for(int j = 0; j < horizontalPadding; j++) printf(" ");
    for(int i = 0; i < width; i++) printf("#");
    printf("\n");
}

int main() {
    char choice[4];

    while (1) {
        clearScreen();
        printf("                                                                            \n");
    printf("                                                                             \n");
    printf("LLLLLLLLLLL                            AAA               PPPPPPPPPPPPPPPPP   \n");
    printf("L:::::::::L                           A:::A              P::::::::::::::::P  \n");
    printf("L:::::::::L                          A:::::A             P::::::PPPPPP:::::P \n");
    printf("LL:::::::LL                         A:::::::A            PP:::::P     P:::::P\n");
    printf("  L:::::L                          A:::::::::A             P::::P     P:::::P\n");
    printf("  L:::::L                         A:::::A:::::A            P::::P     P:::::P\n");
    printf("  L:::::L                        A:::::A A:::::A           P::::PPPPPP:::::P \n");
    printf("  L:::::L                       A:::::A   A:::::A          P:::::::::::::PP  \n");
    printf("  L:::::L                      A:::::A     A:::::A         P::::PPPPPPPPP    \n");
    printf("  L:::::L                     A:::::AAAAAAAAA:::::A        P::::P            \n");
    printf("  L:::::L                    A:::::::::::::::::::::A       P::::P            \n");
    printf("  L:::::L         LLLLLL    A:::::AAAAAAAAAAAAA:::::A      P::::P            \n");
    printf("LL:::::::LLLLLLLLL:::::L   A:::::A             A:::::A   PP::::::PP          \n");
    printf("L::::::::::::::::::::::L  A:::::A               A:::::A  P::::::::P          \n");
    printf("L::::::::::::::::::::::L A:::::A                 A:::::A P::::::::P          \n");
    printf("LLLLLLLLLLLLLLLLLLLLLLLLAAAAAAA                   AAAAAAAPPPPPPPPPP          \n");
        printCentered("");
        printCentered("You want seat,We want money!!(:");
        printAlignedMenu();
        printf("\nEnter your choice: ");
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

