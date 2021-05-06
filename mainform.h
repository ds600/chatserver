#ifndef MAINFORM_H
#define MAINFORM_H

#include "ui_mainform.h"
#include <QtWidgets>
#include <QtNetwork>

class Client;

class MainForm : public QWidget, private Ui::MainForm
{
  Q_OBJECT
  private:
    QTcpServer*                 server;
    QList< QPointer< Client > > clients;

    void                        tidyUpClientList( void );
    void                        acceptClient( QTcpSocket* newClient );
    void                        rejectClient( QTcpSocket* newClient );
    void                        addClientToGuiTable( Client* client );
    void                        sendMessageToAllClientsExceptOne( const QString& msg, Client* except = nullptr );
    QString                     getSimilarName( const QString& name ) const;
    void                        rejectNameChange( Client* client, const QString& newName );
    void                        sendCurrentClientList( Client* client );

  public:
    explicit                    MainForm( QWidget* parent = nullptr );

  private slots:
    void                        newConnectionSlot( void );
    void                        fillGuiTable( void );
    void                        nameChangeRequestSlot( Client* client, const QString& newName );
    void                        nameChangedSlot( Client* client, const QString& oldName );
    void                        chatReceivedSlot( Client* sendingClient, const QString& chatMsg );
};

#endif // MAINFORM_H
