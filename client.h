#ifndef CLIENT_H
#define CLIENT_H

#include <QtCore>
#include <QtNetwork>

class Client : public QObject
{
  Q_OBJECT
  private:
    QPointer< QTcpSocket > clientSocket;
    QString                name;
    bool                   aboutToLeave;

    void                   analyseOneMessage( void );
    void                   sendTime( void ) const;
    void                   sendDate( void ) const;
    void                   sendErrorMessage( const QString& errMsg ) const;
    void                   sendUnknownCommandError( const QString& cmd ) const;
    void                   nameChangeReceived( const QString& receivedLine );
    void                   chatReceived( const QString& receivedLine );
    void                   sendUnregisteredChatError( void ) const;
    void                   sendInvalidNameError( const QString& invalidName ) const;
    void                   sendName( void ) const;

  public:
    explicit               Client( QTcpSocket* socket, QObject* parent = nullptr );
    static QString         getDelimitedText( const QString& text );
    static QString         getTrimmedTextBehindDelimiter( const QString& delimitedText );
    static bool            isNameInvalid( const QString& nameToTest );
    QString                getIpAddressAsString( void ) const;
    QString                getName( void ) const;
    void                   setName( const QString& newName );
    void                   sendMessage( const QString& msg ) const;
    bool                   isAboutToLeave( void ) const;

  private slots:
    void                   clientDisconnectedSlot( void );
    void                   readyReadSlot( void );

  signals:
    void                   nameChangeRequest( Client* client, const QString& newName );
    void                   nameChanged( Client* client, const QString& oldName );
    void                   chatReceived( Client* sendingClient, const QString& chatMsg );
    void                   clientDisconnected( void );
};


#endif // CLIENT_H
