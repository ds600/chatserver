#include "client.h"
#include "config.h"

Client::Client( QTcpSocket* socket, QObject* parent )
  : QObject( parent ), clientSocket( socket ), aboutToLeave( false )
{
  connect( clientSocket, SIGNAL( readyRead() ),
           this, SLOT( readyReadSlot() ) );
  connect( clientSocket, SIGNAL( disconnected() ),
           this, SLOT( clientDisconnectedSlot() ) );
  sendName();
}

QString Client::getDelimitedText( const QString& text )
{
  return QString( "%2%1 " ).arg( PROTOCOL_DELIMITER ).arg( text );
}

QString Client::getTrimmedTextBehindDelimiter( const QString& delimitedText )
{
  return delimitedText.section( PROTOCOL_DELIMITER, 1 ).trimmed();
}

bool Client::isNameInvalid( const QString& nameToTest )
{
  if( nameToTest.isEmpty() || nameToTest.toLower() == UNREGISTERED_NAME ) return true;
  QStringList invalidSymbols = INVALID_NAME_SYMBOLS;
  for( int i = 0; i < invalidSymbols.count(); i++ )
    if( nameToTest.contains( invalidSymbols[ i ] ) ) return true;
  return false;
}

QString Client::getIpAddressAsString( void ) const
{
  return clientSocket->peerAddress().toString();
}

void Client::clientDisconnectedSlot( void )
{
  aboutToLeave = true;
  emit clientDisconnected();
  deleteLater();
}

void Client::readyReadSlot( void )
{
  while( clientSocket->canReadLine() ) analyseOneMessage();
}

void Client::analyseOneMessage( void )
{
  QString oneLine   = QString::fromUtf8( clientSocket->readLine() ).trimmed();
  QString lowerLine = oneLine.toLower();
  if( lowerLine == KEY_TIME )
    sendTime();
  else if( lowerLine == KEY_DATE )
    sendDate();
  else if( lowerLine.startsWith( getDelimitedText( KEY_NAME ) ) )
    nameChangeReceived( oneLine );
  else if( lowerLine.startsWith( getDelimitedText( KEY_CHAT ) ) )
    chatReceived( oneLine );
  else if( KEYS_QUIT.contains( lowerLine ) )
    clientSocket->disconnectFromHost();
  else
    sendUnknownCommandError( oneLine );
}

QString Client::getName( void ) const
{
  if( name.isEmpty() ) return UNREGISTERED_NAME;
  return name;
}

void Client::sendMessage( const QString& msg ) const
{
  clientSocket->write( QString( "%2%1" ).arg( PROTOCOL_SUFFIX ).arg( msg ).toUtf8() );
}

bool Client::isAboutToLeave( void ) const
{
  return aboutToLeave;
}

void Client::setName( const QString& newName )
{
  const QString oldName = getName();
  if( oldName == newName ) return;
  name = newName;
  sendName();
  emit nameChanged( this, oldName );
}

void Client::nameChangeReceived( const QString& receivedLine )
{
  QString newName = getTrimmedTextBehindDelimiter( receivedLine );
  if( isNameInvalid( newName ) )
    sendInvalidNameError( newName );
  else if( getName() == newName )
    sendName();
  else if( name.toLower() != newName.toLower() )
    emit nameChangeRequest( this, newName );
  else
    setName( newName );
}

void Client::chatReceived( const QString& receivedLine )
{
  if( name.isEmpty() ) sendUnregisteredChatError();
  else                 emit chatReceived( this, getTrimmedTextBehindDelimiter( receivedLine ) );
}

void Client::sendUnregisteredChatError( void ) const
{
  sendErrorMessage( ERR_UNREGISTERED_CHAT );
}

void Client::sendInvalidNameError( const QString& invalidName ) const
{
  QString errMsg = QString( "%1%2" )
                    .arg( getDelimitedText( ERR_INVALID_NAME ) )
                    .arg( invalidName );
  sendErrorMessage( errMsg );
}

void Client::sendName( void ) const
{
  QString msg = QString( "%1%2" )
                  .arg( getDelimitedText( KEY_NAME ) )
                  .arg( getName() );
  sendMessage( msg );
}

void Client::sendTime( void ) const
{
  QTime   now = QTime::currentTime();
  QString msg = QString( "%1%2" )
                  .arg( getDelimitedText( KEY_TIME ) )
                  .arg( now.toString( FORMAT_TIME ) );
  sendMessage( msg );
}

void Client::sendDate( void ) const
{
  QDate   now = QDate::currentDate();
  QString msg = QString( "%1%2" )
                  .arg( getDelimitedText( KEY_DATE ) )
                  .arg( now.toString( FORMAT_DATE ) );
  sendMessage( msg );
}

void Client::sendErrorMessage( const QString& errMsg ) const
{
  QString msg = QString( "%1%2" ).arg( getDelimitedText( KEY_ERROR ) ).arg( errMsg );
  sendMessage( msg );
}

void Client::sendUnknownCommandError( const QString& cmd ) const
{
  QString errMsg =QString( "%1%2" )
                   .arg( getDelimitedText( ERR_UKNOWN_CMD ) )
                   .arg( cmd );
  sendErrorMessage( errMsg );
}
