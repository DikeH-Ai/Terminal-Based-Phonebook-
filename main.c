#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

#define MAX_PERSONS 100000
contact *array[MAX_PERSONS];
// Function prototypes

FILE* opencsv();
void displayMenu();
int addcontact(FILE *fptr);
int delete_contact(FILE *fptr);
int adv_search(char name[], FILE *fptr);
int rewrite(FILE *ptr, FILE *fptr, char *tmp_ptr);
int contact_list(FILE *fptr);
int delete_all_contact(FILE *fptr);
int hash_checksum(char *name);
int hash_load(FILE *fptr);
//entry point
int main(void) {

    FILE *fptr = opencsv();

    int choice;
    do {
        displayMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addcontact(fptr);
                break;
            case 2:
                delete_contact(fptr);
                break;
            case 3:
                hash_load(fptr);
                break;
            case 4:
                contact_list(fptr);
                break;
            case 5:
                delete_all_contact(fptr);
                break;
            case 6:
                printf("Exiting the program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 6);
    fclose(fptr);
    return 0;
}

void displayMenu() {
    printf("\n--- Phonebook Menu ---\n");
    printf("1. Add a contact\n");
    printf("2. Delete contacts\n");
    printf("3. Search contacts\n");
    printf("4. Display all contacts\n");
    printf("5. Delete all contacts\n");
    printf("6. Exit\n");
}

// Function to create CSV file
FILE* opencsv() {
    FILE *ptr;
    ptr = fopen("Phonebook.csv", "a+");

    if (ptr == NULL) {
        fprintf(stderr, "Error: Failed to open the file.\n");
    }

    char buffer[5];
    if (fgets(buffer, sizeof(buffer), ptr) == NULL) {
        fprintf(ptr, "Name,Number,Email\n");
    }

    return ptr;
}

// Function to add contacts to CSV file
int addcontact(FILE *fptr) {
    fptr = fopen("Phonebook.csv", "a");
    // Declare a contact
    contact newcontact;

    // Request for information
    // Name
    printf("Contact name: \n");
    fflush(stdin); // flush stdin
    // printf(" (The contact name should not exceed 42 characters.): ");
    fgets(newcontact.name, sizeof(newcontact.name), stdin);
    //remove newline character
    size_t namelength = strlen(newcontact.name);
    if (namelength > 0 && newcontact.name[namelength - 1] == '\n') {
        newcontact.name[namelength - 1] = '\0';
    }
    printf("%s\n", newcontact.name);
    // Number
    printf("\nContact number (no spaces allowed): ");
    fgets(newcontact.number, sizeof(newcontact.number), stdin);
    //remove \n
    size_t number_length = strlen(newcontact.number);
    if (number_length > 0 && newcontact.number[number_length - 1] == '\n') {
        newcontact.number[number_length - 1] = '\0';
    }

    // Email
    printf("Contact email: ");
    fgets(newcontact.email, sizeof(newcontact.email), stdin);
    size_t email_length = strlen(newcontact.email);
    if (email_length > 0 && newcontact.email[email_length - 1] == '\n') {
        newcontact.email[email_length - 1] = '\0';
    }
    // Print to file
    int infolen = fprintf(fptr, "%s,%s,%s\n", newcontact.name, newcontact.number, newcontact.email);
    fclose(fptr);
    return 0;
}

//Function to delete contact
int delete_contact(FILE *fptr){
    //open phonebook csv file in read mode
    fptr = fopen("Phonebook.csv", "r+");
    if (fptr == NULL)
    {
        /* code */
        perror("Fatal Error: Unable to open the database");
        return (1);
    }
    //create a new temp csv file/ open i write mode
    FILE *ptr = fopen("tempfile.csv", "w+");
    if (ptr == NULL)
    {
        /* code */
        perror("Fatal Error: Temp file creation failed");
        return (1);
    }
    //contact variable
    contact del;
    int status = 0;
    do
    {
        fflush(stdin);
        status = 0;
        rewind(fptr);
        //Request for contact to be deleted
        printf("Please enter the name of the contact to delete: ");
        fgets(del.name, sizeof(del.name), stdin);
        //Remove  newline character from input
        del.name[strcspn(del.name,"\n")] = '\0';
        /* code */
        //get each line from the phonebook database file
        //variables 
        char *line_ptr = (char *)malloc(sizeof(char) * 1000);
        if (line_ptr == NULL)
        {
            fprintf(stderr, "Error: Failed to allocate memory for line.\n");
            return (1);
        }

        char *tmp_ptr = (char *)malloc(sizeof(char) * 1000);
        if (line_ptr == NULL)
        {
            fprintf(stderr, "Error: Failed to allocate memory for line copy.\n");
            return (1);
        }

        // size_t n;
        //strtok variables
        char *limiter = ",";
        char *fullname;
       
        //fgets gets line
        while (fgets(line_ptr, 1000, fptr) != NULL)
        {
            strcpy(tmp_ptr,line_ptr);

            fullname = strtok(line_ptr, limiter);
            if (fullname == NULL)
            {
                fprintf(stderr, "Unable to Tokenize strings\n");
                return (1);
            }

            fullname[strcspn(fullname, "\n")] = '\0';
            
            if (strcasecmp(fullname, del.name) == 0)
            {
                printf("Deleted : %s\n", tmp_ptr);
                if (rewrite(ptr, fptr, tmp_ptr) == 1)
                {
                    fprintf(stderr, "Deleting process failed\n");
                }
                status = 1;
                break;
            }
            
        }
        if (status != 1)
        {
            status = adv_search(del.name, fptr);
        }


  
        free(line_ptr);
        free(tmp_ptr);
    } while (status == 2);
    fclose(fptr);
    fclose(ptr);
}

int adv_search(char name[], FILE *fptr){
    //move FILE pointer to file start
    rewind(fptr);
    //read file
    printf("Initiating advanced search...\n");
    //variables
    char each_line[1000];
    char tmp_line[1000];
    char *fullname;
    char *firstname;
    char *secondname;
    int found = 0;

    while (fgets(each_line, 1000, fptr) != NULL)
    {
        strcpy(tmp_line, each_line);

        fullname = strtok(each_line, ",");
        if (fullname == NULL)
        {
            fprintf(stderr, "Unable to Tokenize strings\n");
            return (1);
        }

        //first name appearance 
        if ((firstname = strtok(fullname, " ")) != NULL)
        {
            //check for appearance
            if (strcasecmp(firstname, name) == 0)
            {
                printf("Found: %s\n", tmp_line);
                found++;
            }
        }
        if ((secondname = strtok(NULL, " ")) != NULL)
        {
            /* code */
            if (strcasecmp(secondname, name) == 0)
            {
                printf("Found: %s", tmp_line);
                found++;
            }
        }
    }
    if (found > 0)
    {
        return (2);
    }else{
        printf("Unable to Locate Contact... Does Contact exist ? ");
    }
    return (0);
    
}


int rewrite(FILE *ptr, FILE *fptr, char *tmp_ptr){
    // set file pointer at the top
    rewind(fptr);
    // holder array
    char transfer[1000];
    // copy all data to new file
    while (fgets(transfer, 1000, fptr) != NULL)
    {
        if (strcasecmp(transfer, tmp_ptr) != 0)
        {
            fprintf(ptr, "%s", transfer);
        }
    }
    // truncate old phonebook file
    if (ftruncate(fileno(fptr), 0) != 0)
    {
        perror("Failed to truncate");
        return (1);
    }
    // rewind every file pointer to the top
    rewind(ptr);
    rewind(fptr);
    // paste back new data from temp file to phonebook
    while (fgets(transfer, 1000, ptr) != NULL)
    {
        fprintf(fptr, "%s", transfer);
    }
    return (0);
}

// print contact list.
int contact_list(FILE *fptr){
    fptr = fopen("Phonebook.csv", "r+");

    if (fptr == NULL)
    {
        /* code */
        printf("Contact is empty.");
        return (1);
    }
    
    char eachline[1000];

    while (fgets(eachline, sizeof(eachline), fptr) != NULL)
    {
        /* code */
        printf("%s\n", eachline);
    }
    return (0);
}

// delete all contacts.

int delete_all_contact(FILE *fptr){
    if (ftruncate(fileno(fptr), 0) != 0)
    {
        perror("Failed to delete all contacts");
        return (1);
    }else{
        printf("All contacts deleted.s");
    }
    return (0);
}

// search for contact 

int hash_load(FILE *fptr){
    
    rewind(fptr);

    //init array to NULL
    for (size_t i = 0; i < MAX_PERSONS; i++)
    {
        /* code */
        array[i] = NULL;
    }

    //load data
    // get name
    char *line_ptr = (char *)malloc(sizeof(char) * 1000);
    if (line_ptr == NULL)
    {
         fprintf(stderr, "Error: Failed to allocate memory for line.\n");
         return (1);
    }
    //fgets gets line
    while (fgets(line_ptr, 1000, fptr) != NULL)
    {
        contact loaded;
        regmatch_t match[2]; // one capturing group
        // load csv data to contact container using regex
        regex_t regex;
        // init loaded values
        strcpy(loaded.name, "");
        strcpy(loaded.number, "");
        strcpy(loaded.email, "");

        char *pattern = "^([^,]+)";
        // compile regex expression pattern for name
        if (regcomp(&regex, pattern, REG_EXTENDED | REG_ICASE) != 0)
        {
            printf("Failed to compile regex pattern");
            return (1);
        }
        // matching name
        if (regexec(&regex, line_ptr, 2, match, 0) == 0)
        {

            int start = match[1].rm_so;
            int end = match[1].rm_eo;
            int length = end - start;
            char string[length + 1];
            strncpy(string, line_ptr + start, length);
            string[length] = '\0';
            if (strcasecmp("Name", string) == 0)
            {
                continue;
            }
            strcpy(loaded.name, string);
            // printf("%s, ", string);
        }else{
            // printf("Failed to match pattern.");
        }
        regfree(&regex);

        // match number

        regmatch_t number_match[2]; // one capturing group
        // load csv data to contact container using regex
        regex_t number_regex;
        char *number_pattern = ",(\\d+),";
        // compile regex expression pattern for name
        if (regcomp(&number_regex, number_pattern, REG_EXTENDED | REG_ICASE) != 0)
        {
            printf("Failed to compile Number regex pattern");
            return (1);
        }
        // matching name
        if (regexec(&number_regex, line_ptr, 2, number_match, 0) == 0)
        {

            int start = number_match[1].rm_so;
            int end = number_match[1].rm_eo;
            int length = end - start;
            char string[length + 1];
            strncpy(string, line_ptr + start, length);
            string[length] = '\0';
            strcpy(loaded.number, string);
            // printf("%s, ", string);
        }else{
            // printf("Failed to match number pattern.");
        }
        regfree(&number_regex);

        // match email

        regmatch_t email_match[2]; // one capturing group
        // load csv data to contact container using regex
        regex_t email_regex;
        char *email_pattern = ",([A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,})";
        // compile regex expression pattern for name
        if (regcomp(&email_regex, email_pattern, REG_EXTENDED | REG_ICASE) != 0)
        {
            printf("Failed to compile email regex pattern");
            return (1);
        }
        // matching name
        if (regexec(&email_regex, line_ptr, 2, email_match, 0) == 0)
        {

            int start = email_match[1].rm_so;
            int end = email_match[1].rm_eo;
            int length = end - start;
            char string[length + 1];
            strncpy(string, line_ptr + start, length);
            string[length] = '\0';
            strcpy(loaded.email, string);
            // printf("%s\n", string);
        }else{
            // printf("Failed to match email pattern.\n");
        }
        regfree(&email_regex);
        // hash function
        int hash_value;
        hash_value = hash_checksum(loaded.name);
        // printf("Hash value: %d", hash_value);
        if (array[hash_value] == NULL)
        {
            array[hash_value] = malloc(sizeof(contact));
            array[hash_value]->next = NULL;
            if (array[hash_value] == NULL)
            {
                printf("Failed to create contact space");
                return (1);
            }
            memcpy(array[hash_value], &loaded, sizeof(contact)); // Copy the loaded contact to the allocated memory
            // printf("\n%d, %s, %s, %s\n", hash_value, array[hash_value]->name, array[hash_value]->number, array[hash_value]->email);
        }else{
            
            contact *temp = array[hash_value];
            while (temp->next != NULL)
            {
                /* code */
                temp = temp->next;
            }
            temp->next = malloc(sizeof(contact));
            memcpy(temp->next, &loaded, sizeof(contact));
            temp->next->next = NULL;
            // printf("\n%d, %s, %s, %s\n", hash_value, temp->next->name, temp->next->number, temp->next->email);
         }
         
        
    }
    // search
    char search[100];
    fflush(stdin);
    // get contact
    printf("Search for contact: ");
    fgets(search, sizeof(search), stdin);
    // set last character to null
    search[strcspn(search, "\n")] = '\0';
    // get hash value
    int search_value = hash_checksum(search);
    // print values in search value index array
    contact *search_tmp = array[search_value];
    if (search_tmp == NULL)
    {
        printf("Contact Not Found.\n");
        adv_search(search, fptr);
    }else{
        while (search_tmp != NULL)
        {
            printf("Name: %s\n", search_tmp->name);
            printf("Number: %s\n", search_tmp->number);
            printf("Email: %s\n", search_tmp->email);
            printf("\n");
            search_tmp = search_tmp->next;
        }
    }

    for (size_t i = 0; i < MAX_PERSONS; i++)
    {
        if (array[i] != NULL)
        {
            contact *search_tmp = array[i];
            contact *next;

            while (search_tmp != NULL)
            {
                next = search_tmp->next;
                free(search_tmp);
                search_tmp = next;
            }

            array[i] = NULL;  // Set the array element to NULL after freeing the linked list
        }
    }

    free(line_ptr);
}

int hash_checksum(char *name){
    // check name
    if (name == NULL)
    {
        /* code */
        printf("Name cannot be NULL.\n");
        return(1);
    }
    int hash_value = 0;
    for (size_t i = 0; i < strlen(name); i++)
    {
        /* code */
        hash_value += 37 * name[i] % MAX_PERSONS;
    }
    return(hash_value);
}
