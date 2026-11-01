/*
 * PERSONAL FINANCIAL DASHBOARD
 * JIIT, Sector 128, Noida - B.Tech I Semester Project
 *
 * Uses: arrays, strings, structures, functions, loops,
 *       if-else, switch-case and file handling.
 *
 * The screen is cleared after every operation and a short
 * message about the previous operation is shown at the top.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX 100                    /* maximum number of transactions */
#define FILE_PREFIX "transactions_"

/* One transaction = type + category + description + amount */
struct Transaction
{
    char type[10];                 /* "Income" or "Expense" */
    char category[20];             /* Food, Travel, Education etc. */
    char description[50];
    float amount;
};

struct Transaction t[MAX];         /* array of structures */
int count = 0;                     /* number of records stored */
char name[30];                     /* user's name */
char filename[100];                /* separate file for each user */
char message[100] = "";            /* message about the previous operation */

/* ---------- function declarations ---------- */
void clearScreen();
void pauseScreen();
void showHeader();
void loadFromFile();
void createUserFilename();
void saveToFile();
void addTransaction(char type[]);
void displayAll();
void showSummary();
void searchTransaction();

/* ---------- main ---------- */
int main()
{
    int choice;

    clearScreen();
    printf("====================================\n");
    printf("     PERSONAL FINANCIAL DASHBOARD\n");
    printf("====================================\n");
    printf("Enter your name: ");
    if (scanf(" %29[^\n]", name) != 1) return 1;
    createUserFilename();

    loadFromFile();                /* get old records from file */

    do
    {
        clearScreen();
        showHeader();

        /* show what happened in the previous operation */
        if (strlen(message) > 0)
        {
            printf("\n>> %s\n", message);
            strcpy(message, "");   /* empty it after showing once */
        }

        printf("\n------------- MENU -------------\n");
        printf("1. Add Income\n");
        printf("2. Add Expense\n");
        printf("3. Display All Transactions\n");
        printf("4. Show Total Income, Expense & Balance\n");
        printf("5. Search Transaction\n");
        printf("6. Save and Exit\n");
        printf("--------------------------------\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                addTransaction("Income");
                break;
            case 2:
                addTransaction("Expense");
                break;
            case 3:
                displayAll();
                pauseScreen();
                strcpy(message, "Displayed all transactions.");
                break;
            case 4:
                showSummary();
                pauseScreen();
                strcpy(message, "Summary calculated.");
                break;
            case 5:
                searchTransaction();
                pauseScreen();
                strcpy(message, "Search completed.");
                break;
            case 6:
                saveToFile();
                clearScreen();
                printf("\nRecords saved. Thank you, %s!\n", name);
                break;
            default:
                strcpy(message, "Invalid choice! Please enter 1 to 6.");
        }
    } while (choice != 6);

    return 0;
}

/* Create a safe, user-specific filename. Names are case-insensitive. */
void createUserFilename()
{
    char safeName[60];
    int i, j = 0;
    for (i = 0; name[i] != '\0' && j < 50; i++)
    {
        unsigned char ch = (unsigned char)name[i];
        if (isalnum(ch))
            safeName[j++] = (char)tolower(ch);
        else if (ch == ' ' || ch == '_' || ch == '-')
            safeName[j++] = '_';
    }
    safeName[j] = '\0';
    if (j == 0) strcpy(safeName, "user");
    snprintf(filename, sizeof(filename), "%s%s.txt", FILE_PREFIX, safeName);
}

/* ---------- clear the screen (works on Windows and Linux/Mac) ---------- */
void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* ---------- wait until the user presses Enter ---------- */
void pauseScreen()
{
    printf("\nPress Enter to continue...");
    while (getchar() != '\n');     /* remove leftover newline */
    getchar();                     /* wait for Enter */
}

/* ---------- heading shown at the top of every screen ---------- */
void showHeader()
{
    printf("====================================\n");
    printf("     PERSONAL FINANCIAL DASHBOARD\n");
    printf("====================================\n");
    printf("User: %s\n", name);
}

/* ---------- add an income or expense record ---------- */
void addTransaction(char type[])
{
    if (count >= MAX)
    {
        strcpy(message, "Memory full! Cannot add more records.");
        return;
    }

    printf("\nEnter category (Food/Travel/Education/...): ");
    scanf(" %19[^\n]", t[count].category);

    printf("Enter description: ");
    scanf(" %49[^\n]", t[count].description);

    printf("Enter amount: ");
    scanf("%f", &t[count].amount);

    if (t[count].amount <= 0)
    {
        strcpy(message, "Amount must be greater than 0. Record not added.");
        return;
    }

    strcpy(t[count].type, type);

    /* build the message for the next screen */
    sprintf(message, "%s of Rs. %.2f added successfully (%s).",
            type, t[count].amount, t[count].category);

    count++;
}

/* ---------- display all records ---------- */
void displayAll()
{
    int i;

    clearScreen();
    showHeader();

    if (count == 0)
    {
        printf("\nNo transactions found.\n");
        return;
    }

    printf("\n%-5s %-10s %-15s %-25s %10s\n",
           "No.", "Type", "Category", "Description", "Amount");
    printf("-------------------------------------------------------------------\n");

    for (i = 0; i < count; i++)
    {
        printf("%-5d %-10s %-15s %-25s %10.2f\n",
               i + 1, t[i].type, t[i].category,
               t[i].description, t[i].amount);
    }
}

/* ---------- total income, total expense, balance ---------- */
void showSummary()
{
    int i;
    float income = 0, expense = 0, balance;

    for (i = 0; i < count; i++)
    {
        if (strcmp(t[i].type, "Income") == 0)
            income = income + t[i].amount;
        else
            expense = expense + t[i].amount;
    }

    balance = income - expense;

    clearScreen();
    showHeader();

    printf("\n========== SUMMARY ==========\n");
    printf("Total Income   : %.2f\n", income);
    printf("Total Expenses : %.2f\n", expense);
    printf("Balance        : %.2f\n", balance);
    printf("=============================\n");

    if (balance < 0)
        printf("Warning: You are spending more than you earn!\n");
    else if (balance == 0)
        printf("Your income and expenses are equal.\n");
    else
        printf("Good job! You are saving money.\n");
}

/* ---------- search by category or description ---------- */
void searchTransaction()
{
    int i, found = 0;
    char key[50];

    printf("\nEnter category or description to search: ");
    scanf(" %49[^\n]", key);

    clearScreen();
    showHeader();

    printf("\nSearch results for \"%s\":\n", key);
    printf("\n%-5s %-10s %-15s %-25s %10s\n",
           "No.", "Type", "Category", "Description", "Amount");
    printf("-------------------------------------------------------------------\n");

    for (i = 0; i < count; i++)
    {
        /* strstr checks if 'key' is present inside the text */
        if (strstr(t[i].category, key) != NULL ||
            strstr(t[i].description, key) != NULL)
        {
            printf("%-5d %-10s %-15s %-25s %10.2f\n",
                   i + 1, t[i].type, t[i].category,
                   t[i].description, t[i].amount);
            found = 1;
        }
    }

    if (found == 0)
        printf("No matching transaction found.\n");
}

/* ---------- save all records to a file ---------- */
void saveToFile()
{
    FILE *fp;
    int i;

    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Error: could not open file for saving.\n");
        return;
    }

    /* one field per line so that spaces in text are safe */
    for (i = 0; i < count; i++)
    {
        fprintf(fp, "%s\n%s\n%s\n%f\n",
                t[i].type, t[i].category,
                t[i].description, t[i].amount);
    }

    fclose(fp);
}

/* ---------- load old records from the file ---------- */
void loadFromFile()
{
    FILE *fp;

    fp = fopen(filename, "r");
    if (fp == NULL)
        return;                    /* first run: no file yet */

    while (count < MAX &&
           fscanf(fp, " %9[^\n] %19[^\n] %49[^\n] %f",
                  t[count].type, t[count].category,
                  t[count].description, &t[count].amount) == 4)
    {
        count++;
    }

    fclose(fp);

    if (count > 0)
        sprintf(message, "%d old record(s) loaded from file.", count);
}
