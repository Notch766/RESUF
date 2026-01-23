#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE (4 * 1024 * 1024)

int main(void) {

    char drive[10] = {0};   //Store Device Name
    char iso[100] = {0};    //Store ISO Path

    char *yesno = (char*)malloc(sizeof(char));

    int usb_fd; //File Descriptor for Device
    int iso_fd; //File Descripto for ISO

    printf("\n\nWelcome to Resuf! (REally Smart Usb Flasher).\n"); //Resuf = REally Smart Usb Flasher
    printf("An Open-Source USB ISO Image Flasher!\n\n\n");

    pid_t pid = fork();

    if (pid < 0) {

        perror("FORK FAILED!!!");
        return EXIT_FAILURE;

    }

    if (pid == 0) { //Child Process

        execlp("lsblk", "lsblk", NULL);

    }

    //Parent Process

    wait(NULL); //Waiting for Child to Finish

    do {

        printf("\nPlease, Choose The Drive You Want to Flash an ISO into...\n\n\tHint: Most of The Times its The Last sd.\n\tExample: If You Want to Use sdb, Type /dev/sdb.\n>");
        scanf("%s", drive);

        usb_fd = open(drive, O_WRONLY);

        if (usb_fd < 0) {
            printf("Invalid Drive!");
        }

    } while (usb_fd < 0);

    printf("%s Drive is Selected!\n", drive);

    do {

        printf("Please, Type the Location of the ISO Image File You Want to Burn in the USB\n>");

        scanf("%s", iso);

        iso_fd = open(iso, O_RDONLY);

        if (iso_fd < 0) {
            printf("Invalid ISO Path!\n");
        }

    } while (iso_fd < 0);

    printf("%s ISO Image is Selected!\n", iso);

    printf("ATTENTION!! ONCE THE PROCEDURE STARTS, EVERY SINGLE DATA ON THE SELECTED DISK WILL BE DELETED. ARE YOU SURE YOU WANT TO FORMAT DRIVE: %s? [Y/N]\n>", drive);
    scanf(" %c", yesno);

    if (*yesno != 'Y' && *yesno != 'y') {

        printf("USB FLASH ABORTED. EXITING RESUF. NO DISK WAS TOUCHED\n");
        close(iso_fd);
        close(usb_fd);
        return EXIT_SUCCESS;
    }

    free(yesno);

    printf("BEGINING IMAGE ISO BURN IN DEVICE: %s. DONT TOUCH THE DRIVE...\n", drive);

    void *buffer = malloc(BUFFER_SIZE);

    if(!buffer) {

        perror("Malloc Failed!!!\n");
        printf("USB FLASH ABORTED. EXITING RESUF. NO DISK WAS TOUCHED\n");
        close(usb_fd);
        close(iso_fd);
        return EXIT_FAILURE;

    }

    printf("Writting ISO Image to Device...\n");

    ssize_t bytes_read;
    while ((bytes_read = read(iso_fd, buffer, BUFFER_SIZE)) > 0) {

        ssize_t bytes_written = write(usb_fd, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Write Error!!!\n");
            close(usb_fd);
            close(iso_fd);
            return EXIT_FAILURE;
        }
    }

    if (bytes_read < 0) {
        perror("Read Error!\n");
        close(usb_fd);
        close(iso_fd);
        return EXIT_FAILURE;
    }

    printf("ISO Successfully Written to Device! You may now Remove the USB Device!\n");
    printf("Exiting Resuf\n");

    close(usb_fd);
    close(iso_fd);

    return EXIT_SUCCESS;

}
