#include "mainform.h"
#include "config.h"
#include "client.h"

MainForm::MainForm( QWidget* parent )
  : QWidget( parent ), server( new QTcpServer( this ) )
{
  setupUi( this );
  server = new QTcpServer( this );
  if( !server->listen( QHostAddress::Any, USED_PORT ) )
  {
    QMessageBox::critical( this, "Netzwerkfehler",
                           QString( "Port %1 konnte nicht geöffnet werden!" )
                             .arg( USED_PORT ),
                           "Programm&ende"
                         );
    exit( 1 );
  }
  connect( server, SIGNAL( newConnection() ),
           this, SLOT( newConnectionSlot() ) );
}

void MainForm::newConnectionSlot( void )
{
  QTcpSocket* newClient = server->nextPendingConnection();

  connect( newClient, SIGNAL( disconnected() ),
           newClient, SLOT( deleteLater() ) );

  tidyUpClientList();
  if( clients.count() < MAX_CLIENT_COUNT ) acceptClient( newClient );
  else                                     rejectClient( newClient );
}

void MainForm::tidyUpClientList( void )
{
  int oldClientCount = clients.count();
  clients.removeAll( nullptr );
  if( clients.count() != oldClientCount ) fillGuiTable();
}

void MainForm::acceptClient( QTcpSocket* newClient )
{
  tidyUpClientList();
  Client* client = new Client( newClient, this );
  clients << client;
  connect( client, SIGNAL( nameChangeRequest( Client*, const QString& ) ),
           this, SLOT( nameChangeRequestSlot( Client*, const QString& ) ) );
  connect( client, SIGNAL( nameChanged( Client*, const QString& ) ),
           this, SLOT( nameChangedSlot( Client*, const QString& ) ) );
  connect( client, SIGNAL( chatReceived( Client*, const QString& ) ),
           this, SLOT( chatReceivedSlot( Client*, const QString& ) ) );
  connect( client, SIGNAL( clientDisconnected() ),
           this, SLOT( fillGuiTable() ) );
  fillGuiTable();
}

void MainForm::rejectClient( QTcpSocket* newClient )
{
  QString msg = QString( "%2 %3%1" )
                  .arg( PROTOCOL_SUFFIX )
                  .arg( Client::getDelimitedText( KEY_ERROR ) )
                  .arg( ERR_SERVER_FULL );
  newClient->write( msg.toUtf8() );
  newClient->disconnectFromHost();
}

void MainForm::fillGuiTable( void )
{
  tidyUpClientList();
  twClients->setRowCount( 0 );
  for( int i = 0; i < clients.count(); i++ )
    if( !clients[ i ]->isAboutToLeave() )
      addClientToGuiTable( clients[ i ] );
  twClients->resizeColumnsToContents();
}

QString MainForm::getSimilarName( const QString& name ) const
{
  const QString lowerCaseName = name.toLower();
  for( int i = 0; i < clients.count(); i++ )
    if( clients[ i ]->getName().toLower() == lowerCaseName )
      return clients[ i ]->getName();
  return QString();
}

void MainForm::rejectNameChange( Client* client, const QString& newName )
{
  QString msg = QString( "%1%2%3" )
                  .arg( Client::getDelimitedText( KEY_ERROR ) )
                  .arg( Client::getDelimitedText( ERR_USED_NAME ) )
                  .arg( newName );
  client->sendMessage( msg );
}

void MainForm::sendCurrentClientList( Client* client )
{
  tidyUpClientList();
  QString msg = QString( "%1%2" ).arg( Client::getDelimitedText( KEY_NEW_USER ) );
  for( int i = 0; i < clients.count(); i++ )
    if( clients[ i ] != client ) client->sendMessage( msg.arg( clients[ i ]->getName() ) );
}

void MainForm::nameChangeRequestSlot( Client* client, const QString& newName )
{
  tidyUpClientList();
  QString similarName = getSimilarName( newName );
  if( similarName.isEmpty() ) client->setName( newName );
  else                        rejectNameChange( client, similarName );
}

void MainForm::nameChangedSlot( Client* client, const QString& oldName )
{
  bool isNewUser = oldName == UNREGISTERED_NAME;
  QString msg = QString( "%1%2%3" )
                .arg( Client::getDelimitedText( isNewUser ? KEY_NEW_USER : KEY_NAME_CHANGE ) )
                .arg( isNewUser ? QString() : Client::getDelimitedText( oldName ) )
                .arg( client->getName() );
  sendMessageToAllClientsExceptOne( msg, isNewUser ? client : nullptr );
  if( isNewUser ) sendCurrentClientList( client );
  fillGuiTable();
}

void MainForm::chatReceivedSlot( Client* sendingClient, const QString& chatMsg )
{
  QString msg = QString( "%1%2%3" )
                  .arg( Client::getDelimitedText( KEY_CHAT ) )
                  .arg( Client::getDelimitedText( sendingClient->getName() ) )
                  .arg( chatMsg );
  sendMessageToAllClientsExceptOne( msg );
}

void MainForm::addClientToGuiTable( Client* client )
{
  int row = twClients->rowCount();
  twClients->setRowCount( row + 1 );
  QTableWidgetItem* item = new QTableWidgetItem( client->getIpAddressAsString() );
  twClients->setItem( row, 0, item );
  item = new QTableWidgetItem( client->getName() );
  twClients->setItem( row, 1, item );
}

void MainForm::sendMessageToAllClientsExceptOne( const QString& msg , Client* except )
{
  tidyUpClientList();
  for( int i = 0; i < clients.count(); i++ )
    if( clients[ i ] != except ) clients[ i ]->sendMessage( msg );
}
