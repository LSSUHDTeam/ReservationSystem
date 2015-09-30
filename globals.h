#ifndef GLOBALS_H
#define GLOBALS_H


// Formats and Delemeters
#define EMPTY_DB_ENTRY "NA"
#define LINKED_INFORMATION_DELIMETER ","
#define EMPTY_FIELD_RETURNED "None"
#define SERVER_SEND_DELIMITER "~"
#define SERVER_RECV_DELIMITER '\n'
#define RESULT_MEMBER_DELIMETER "|"
#define INCOMING_RESULT_DELIMETER "%"
#define PACKAGE_DELIMETER "#"
#define DEVICE_LIST_DELIMETER ", "
#define QUERY_DELIMETER "^"
#define ITEM_NEEDS_REFACTORING "REFACTOR"
#define ITEM_REMOVED "REMOVED"
#define MAX_DAILY_DELIVERIES 2000
#define MAX_DATES_UNAVAILABLE 365
#define STANDARD_UPDATE_TIME 600000 // 10 Minutes
#define STANDARD_DATE_FORMAT "yyyy-MM-dd hh:mm:ss"
#define STANDARD_DATE_FORMAT_2 "yyyy-MM-dd"
#define STANDARD_TIME_FORMAT "hh:mm:ss"
#define BLANK_TIME_ENTRY  "0000-00-00 00:00:00"
#define ALERT_TIMER 25000 // 25 seconds

// Limits
#define MAX_FUTURE_DELIVERIES 2000
#define MAX_INVENTORY_ITEMS 2000
#define MAX_LINKS 2000

// Errors
#define ERR_INCOMPLETE_FORM "Incomplete Form Submission"
#define ERR_INCORRECT_TIME_DESIGNATION "Incorrect Time Setup"
#define ERR_CONFLICT_ON_REMOVAL "Removing this item will cause a conflict"

// Files
#define ALERT_SOUND "C:\\Program Files\\JBReservationSystem\\Sounds\\alert.wav"
#define LOG_FILE_SOCKETS "C:\\Program Files\\JBReservationSystem\\Logs\\sockets.csv"




#endif // GLOBALS_H
