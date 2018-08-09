#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QAction>
#include <QAudioInput>
#include <QByteArray>
#include <QMainWindow>
#include <QObject>
#include <QScopedPointer>

#include "ui_spacjateleclient.h"
#include <string>
#include "rtpsession.h"
#include "rtppacket.h"
#include "rtpudpv4transmitter.h"
#include "rtpsessionparams.h"

#include <QAudioOutput>
#include <QTimer>

#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>

struct Host
{
	QHostAddress hostAddress;
	int hostPort;
};

struct ChannelInfo
{
	std::string nameOnChannel = "";
	Host server;
	std::vector<Host> hosts;
	int portAudioIn = 6070;
	bool channelConfigurated;
};


class QTcpServer;
class QTcpSocket;
class QAction;

class MyRTPSession : public jrtplib::RTPSession
{
public:
	MyRTPSession();
	void OnPollThreadStep();
	void ProcessRTPPacket(const jrtplib::RTPSourceData &srcdat, const jrtplib::RTPPacket &rtppack);
	QByteArray arrayBuff;
};



class SpacjaTeleClient : public QMainWindow
{
	Q_OBJECT

public:
	SpacjaTeleClient(QWidget *parent = Q_NULLPTR);
	~SpacjaTeleClient();


public slots:
	void sendAudio();
	void muteAudioOut();
	void getAudio();
	void decrementSig();

	void acceptConnection();
	void updateServerProgress();
	void displayError(QAbstractSocket::SocketError socketError);
	void channelSpinUp();
	void channelSpinDown();

private:
	const int serverTcpPort = 6060;

	QAction * action1Button;
	QAction * action2Button;
	QAction * action3Button;

	Ui::SpacjaTeleClientClass ui;
	QScopedPointer<QAudioInput> m_audioInput;
	QScopedPointer<QAudioOutput> m_audioOutput;

	QTimer * m_pushTimer = nullptr;

	void audioInit(const QAudioDeviceInfo &deviceInfoIn, const QAudioDeviceInfo &deviceInfoOut);
	void rtpInit();
	void disconnectAll();
	void changeChannel(int num);
	void updateChannel();
	void updateDestinations();
	void onChangeChannel();
	void checkerror(int rtperr);
	bool isInChannelList(Host& server);
	ChannelInfo* SpacjaTeleClient::getChannelInfo(Host& server);

	std::vector<ChannelInfo> channelInfos;
	ChannelInfo* currentChannel;

	bool firstConfiguration = true;

	WSADATA dat;
	jrtplib::RTPUDPv4TransmissionParams transparams;
	jrtplib::RTPSessionParams sessparams;
	uint16_t portbase;
	uint16_t destport;
	uint32_t destip;
	std::string ipstr;
	int status;

	MyRTPSession sess;
	
	QUdpSocket udpSocket;
	QTcpServer tcpServer;
	QTcpSocket *tcpServerConnection;
	int bytesReceived;

	QTimer *timerSig;
	int toLive;
	void beReadySig(int time);
	void beLiveSig();

};
