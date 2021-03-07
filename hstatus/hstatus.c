#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int nvidia_gpu = 1;
static const char* onstart_command = "xrandr --output DVI-D-0 --mode 2560x1080 --pos 2560x0 --rotate normal --output HDMI-0 --primary --mode 2560x1080 --pos 0x0 --rotate normal --output DP-0 --off --output DP-1 --mode 1920x1080 --pos 5120x0 --rotate normal";

void init() {
    if( access( "/usr/bin/nvidia-smi", F_OK ) == 0 )
        nvidia_gpu = 1;
    else 
        nvidia_gpu = 0;

    system(onstart_command);
}

char* int_to_str(int i) {
    int length = snprintf(NULL, 0, "%d", i);
    char* str = malloc( length + 1 );
    snprintf( str, length + 1, "%d", i );
    return str;
}


char* getoutput( char command[] )
{

    FILE *fp;
    char path[1035];
    char* buf = malloc(sizeof(char) * 4096);
    strcpy(buf, "");
    /* Open the command for reading. */
    fp = popen(command, "r");
    if (fp == NULL) {
        return "";
    }

    /* Read the output a line at a time - output it. */
    while (fgets(path, sizeof(path), fp) != NULL) {
        strcat(buf, path);
    }

    /* close */
    pclose(fp);

    return buf;
}


char* month_str(int month_num) {
    switch (month_num) {
        case 1:
            return "January";
            
        case 2:
            return "February";
            
        case 3:
            return "March";
            
        case 4:
            return "April";
            
        case 5:
            return "May";
            
        case 6:
            return "June";
            
        case 7:
            return "July";
            
        case 8:
            return "August";
            
        case 9:
            return "September";
            
        case 10:
            return "October";
            
        case 11:
            return "November";
            
        case 12:
            return "December";

        default:
            return "";
            
    }
}
char* day_str(int day_num) {
    switch (day_num) {
        case 1:
            return "Sunday";
        case 2:
            return "Monday";
        case 3:
            return "Tuesday";
        case 4:
            return "Wedndesday";
        case 5:
            return "Thursday";
        case 6:
            return "Friday";
        case 7:
            return "Saturday";
        default:
            return "";
    }
}

char* get_hour(int hour) {
    char* hourstr = malloc(sizeof(char) * 10);
    strcpy(hourstr, "");

    if (hour > 12)
        strcat(hourstr, int_to_str(hour - 12));
    else if (hour == 0)
        strcat(hourstr, int_to_str(12));
    else
        strcat(hourstr, int_to_str(hour));

    return hourstr;
}

char* get_sec(int sec) {
    char* secstr = malloc(sizeof(char) * 10);
    strcpy(secstr, "");
    if (sec < 10) 
        strcat(secstr, "0");
    
    strcat(secstr, int_to_str(sec));
    return secstr;
}

char* get_min(int min) {
    char* minstr = malloc(sizeof(char) * 10);
    strcpy(minstr, "");
    if (min < 10) 
        strcat(minstr, "0");
    
    strcat(minstr, int_to_str(min));
    return minstr;
}


char* get_day(int day) {
    char* daystr = malloc(sizeof(char) * 40);
    strcpy(daystr, int_to_str(day));

    char* dstr = int_to_str(day);
    int daystrl = strlen(dstr);

    char last_char = daystr[daystrl-1];

    switch (last_char) {
        case '1':
            strcat(daystr, "st");
            break;
        case '2':
            strcat(daystr, "nd");
            break;
        case '3':
            strcat(daystr, "rd");
            break;
        default:
            strcat(daystr, "th");
            break;
    }

    return daystr;
}



void get_time(char output[]) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    
    char time_str[1024];

    int year_short = tm.tm_year - 100;
    int year_long  = tm.tm_year + 1900;

    int month = tm.tm_mon + 1;
    int day = tm.tm_mday;
    int hour = tm.tm_hour; 
    int min = tm.tm_min;
    int sec = tm.tm_sec;
    int daynum = tm.tm_wday + 1; // day of the week num
    

    strcat(time_str, day_str(daynum));
    strcat(time_str, " ");
    strcat(time_str, month_str(month));
    strcat(time_str, " ");
    strcat(time_str, get_day(day));
    strcat(time_str, " ");
    strcat(time_str, get_hour(hour));
    strcat(time_str, ":");
    strcat(time_str, get_min(min));
    strcat(time_str, ":");
    strcat(time_str, get_sec(sec)); 
    strcat(time_str, " ");
    
    if (hour > 12) 
        strcat(time_str, "PM");
    else 
        strcat(time_str, "AM");
   
    strcat(output, time_str);
    strcpy(time_str, "");
    
}

void get_gpu(char output[]) {
    char* x = getoutput("echo $(nvidia-smi -q | grep \"Current Temp\" | head -n 1) | sed 's/GPU Current Temp : //g'"); // | sed 's/ C//g'");
}
int main(void) {
    init();

    while (1) {
        char status_str[2048];
        strcpy(status_str, "");
        if (nvidia_gpu)
            get_gpu(status_str);
        get_time(status_str);
        printf("%s\n", status_str);
        strcpy(status_str, "");
        

        sleep(1);
    }
}
