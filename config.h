#ifndef CONFIG_H
#define CONFIG_H

#define USED_PORT            51688
#define MAX_CLIENT_COUNT         3

// Protokoll:
#define PROTOCOL_DELIMITER       QString( ":" )
#define PROTOCOL_SUFFIX          QString( "\r\n" )
#define KEY_ERROR                QString( "ERROR" )
#define KEY_TIME                 QString( "time" )
#define KEY_DATE                 QString( "date" )
#define KEY_NAME                 QString( "name" )
#define KEY_NEW_USER             QString( "newuser" )
#define KEY_NAME_CHANGE          QString( "namechanged" )
#define KEY_CHAT                 QString( "chat" )
#define KEYS_QUIT                QString( "bye#exit#quit#logout" ).split( "#" )
#define ERR_SERVER_FULL          QString( "server full, try again later" )
#define ERR_UKNOWN_CMD           QString( "unknown command received" )
#define ERR_UNREGISTERED_CHAT    QString( "no chat command available if unregistered" )
#define ERR_INVALID_NAME         QString( "invalid username" )
#define ERR_USED_NAME            QString( "name already used" )
#define FORMAT_TIME              QString( "hh:mm:ss" )
#define FORMAT_DATE              QString( "yyyy-MM-dd" )
#define INVALID_NAME_SYMBOLS     QString( "%1#%" ).arg( PROTOCOL_DELIMITER ).split( "#" )

#define UNREGISTERED_NAME        QString( "unregistered" )

#endif // CONFIG_H
