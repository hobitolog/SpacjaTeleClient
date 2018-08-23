#include "spacjateleclient.h"
#include <stdlib.h>
#include <math.h>
#include <QNetworkInterface>
#include <QMessageBox>
#include <iostream>

static const int TotalBytes = 100;


SpacjaTeleClient::SpacjaTeleClient(QWidget *parent)
	: QMainWindow(parent)
{

#ifdef RTP_SOCKETTYPE_WINSOCK
	WSAStartup(MAKEWORD(2, 2), &dat);
#endif // RTP_SOCKETTYPE_WINSOCK
	ui.setupUi(this);
	m_pushTimer = new QTimer(this);
	timerSig = new QTimer();
	timerSig->setInterval(1000);
	toLive = 1;
	connect(timerSig, SIGNAL(timeout()), this, SLOT(decrementSig()));


	for (const QHostAddress &address : QNetworkInterface::allAddresses())
	{
		if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
		{
			ui.IpAddr->setText(address.toString());
		}
	}

	action1Button = new QAction();
	action1Button->setShortcut(Qt::Key_F1);
	action2Button = new QAction();
	action2Button->setShortcut(Qt::Key_F2);
	action3Button = new QAction();
	action3Button->setShortcut(Qt::Key_F3);


	connect(action1Button, SIGNAL(triggered()), this, SLOT(muteAudioOut()));
	connect(action2Button, SIGNAL(triggered()), this, SLOT(channelSpinUp()));
	connect(action3Button, SIGNAL(triggered()), this, SLOT(channelSpinDown()));
	addAction(action1Button);
	addAction(action2Button);
	addAction(action3Button);




	QPixmap pixmap("./mic_on.png");
	QIcon ButtonIcon(pixmap);
	ui.MicSig->setIcon(ButtonIcon);
	ui.MicSig->setIconSize(pixmap.rect().size());
	ui.MicSig->setFixedSize(pixmap.rect().size());
	connect(ui.MicSig, SIGNAL(clicked()), this, SLOT(muteAudioOut()));

#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

	while (!tcpServer.isListening() && !tcpServer.listen(QHostAddress::Any, 6061)) {
		QMessageBox::StandardButton ret = QMessageBox::critical(this,
			tr("TcpServer"),
			tr("Unable to start: %1.")
			.arg(tcpServer.errorString()),
			QMessageBox::Retry
			| QMessageBox::Cancel);
		if (ret == QMessageBox::Cancel)
			return;
	}
	bytesReceived = 0;

	connect(&tcpServer, SIGNAL(newConnection()), this, SLOT(acceptConnection()));

	ui.ChannelSpin->setMaximum(0);
	ui.ChannelSpin->setMinimum(1);
	QUdpSocket udpClient;
	udpClient.bind(QHostAddress(QHostAddress::AnyIPv4), 0);
	udpClient.writeDatagram("DISC/", QHostAddress::Broadcast, 6060);

	ChannelInfo channelInfo;
	channelInfo.portAudioIn = 6070;
	channelInfo.nameOnChannel = "LOCAL";
	Host localhost;
	localhost.hostAddress = QHostAddress::LocalHost;
	localhost.hostPort = 6070;
	channelInfo.hosts.push_back(localhost);
	currentChannel = &channelInfo;

	updateChannel();
	audioInit(QAudioDeviceInfo::defaultInputDevice(), QAudioDeviceInfo::defaultOutputDevice());
	rtpInit();

	onChangeChannel();

	sendAudio();
}

void SpacjaTeleClient::channelSpinUp()
{
	ui.ChannelSpin->stepBy(1);
}

void SpacjaTeleClient::channelSpinDown()
{
	ui.ChannelSpin->stepBy(-1);
}

void SpacjaTeleClient::decrementSig()
{	
	ui.ReadySig->setText(QString::number(toLive));

	if (toLive <= 0)
	{
		timerSig->stop();
		beLiveSig();
	}
	toLive--;

}

//if you want to be instant live time==0;
void SpacjaTeleClient::beReadySig(int time)
{
	if (time == 0)
	{
		toLive = time;
		timerSig->stop();
		beLiveSig();
	}
	else
	{
		toLive = time;
		timerSig->start();
	}
}

void SpacjaTeleClient::beLiveSig()
{
	ui.ReadySig->setText("");
	if ((ui.liveSig->text() == ""))
	{
		ui.liveSig->setText("ON AIR");

		tcpServerConnection->write("LACK/");
	}
	else
	{
		ui.liveSig->setText("");
		tcpServerConnection->write("LACK/");
	}

}

void SpacjaTeleClient::changeChannel(int channel)
{
	int i = channel - 1;
	currentChannel = &channelInfos[i];
	ui.ClientName->setText(QString::fromStdString(currentChannel->nameOnChannel));
	m_audioInput->suspend();
	m_audioOutput->suspend();

	sess.BYEDestroy(jrtplib::RTPTime(10, 0), 0, 0);
	rtpInit();


	sess.arrayBuff.clear();
	m_audioInput->resume();
	m_audioOutput->resume();
}

void SpacjaTeleClient::updateChannel()
{
	ui.ClientName->setText(QString::fromStdString(currentChannel->nameOnChannel));
	updateDestinations();
}

void SpacjaTeleClient::updateDestinations()
{
	sess.ClearDestinations();
	for (Host host : currentChannel->hosts)
	{
		destip = host.hostAddress.toIPv4Address();
		destport = host.hostPort;
		jrtplib::RTPIPv4Address addr(destip, destport);

		status = sess.AddDestination(addr);
		SpacjaTeleClient::checkerror(status);
	}
}

void SpacjaTeleClient::onChangeChannel() 
{
	connect(ui.ChannelSpin, QOverload<int>::of(&QSpinBox::valueChanged),
		[=](int i) { SpacjaTeleClient::changeChannel(i); });
}

void SpacjaTeleClient::checkerror(int rtperr)
{
	if (rtperr < 0)
	{
		std::string tmp = jrtplib::RTPGetErrorString(rtperr);
		std::cout << "ERROR: " << jrtplib::RTPGetErrorString(rtperr) << std::endl;
		qDebug(tmp.c_str());
	}
}

SpacjaTeleClient::~SpacjaTeleClient()
{
	disconnectAll();
}

void SpacjaTeleClient::disconnectAll()
{
	m_audioInput->stop();
	m_audioOutput->stop();
	sess.BYEDestroy(jrtplib::RTPTime(10, 0), 0, 0);

#ifdef RTP_SOCKETTYPE_WINSOCK
	WSACleanup();
#endif // RTP_SOCKETTYPE_WINSOCK
}

void SpacjaTeleClient::audioInit(const QAudioDeviceInfo &deviceInfoIn, const QAudioDeviceInfo &deviceInfoOut)
{
	QAudioFormat format;
	format.setSampleRate(8000);
	format.setChannelCount(1);
	format.setSampleSize(8);
	format.setSampleType(QAudioFormat::UnSignedInt);
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setCodec("audio/pcm");
	auto x = format.codec(); 

	if (!deviceInfoIn.isFormatSupported(format)) {
		qWarning() << "Default format not supported - trying to use nearest";
		format = deviceInfoIn.nearestFormat(format);
	}
	if (!deviceInfoOut.isFormatSupported(format)) {
		qWarning() << "Default format not supported - trying to use nearest";
		format = deviceInfoOut.nearestFormat(format);
	}

	m_audioInput.reset(new QAudioInput(deviceInfoIn, format));//nadawanie
	m_audioOutput.reset(new QAudioOutput(deviceInfoOut, format));
}

void SpacjaTeleClient::sendAudio()
{
	m_audioInput->stop();

	auto io = m_audioInput->start();
	connect(io, &QIODevice::readyRead,
		[&, io]() {
		qint64 len = m_audioInput->bytesReady();
		const int BufferSize = 4096;
		if (len > BufferSize)
			len = BufferSize;

		QByteArray buffer(len, 0);
		qint64 l = io->read(buffer.data(), len);
		if (l > 0)
		{
			std::cout << buffer.data();
			status = sess.SendPacket((void *)buffer.data(), buffer.length(), 96, true, 1);
			SpacjaTeleClient::checkerror(status);
		}
	});
}

void SpacjaTeleClient::getAudio()
{
	m_pushTimer->stop();
	m_audioOutput->stop();

	auto io = m_audioOutput->start();
	m_pushTimer->disconnect();

	connect(m_pushTimer, &QTimer::timeout, [this, io]() {

		int chunks = m_audioOutput->bytesFree() / m_audioOutput->periodSize();
		while (chunks) {
			const qint64 len = sess.arrayBuff.length();
			if (len)				
				io->write(sess.arrayBuff.data(), len);
			if (len != m_audioOutput->periodSize())
				break;
			--chunks;
		}
	});

	m_pushTimer->start(20);
}

void SpacjaTeleClient::muteAudioOut()
{
	// toggle suspend/resume
	if (m_audioInput->state() == QAudio::SuspendedState || m_audioInput->state() == QAudio::StoppedState) {
		m_audioInput->resume();
		ui.MicLabel->setText("Mic is on");
		QPixmap pixmap("./mic_on.png");
		QIcon ButtonIcon(pixmap);
		ui.MicSig->setIcon(ButtonIcon);
		ui.MicSig->setIconSize(pixmap.rect().size());
		ui.MicSig->setFixedSize(pixmap.rect().size());
	}
	else if (m_audioInput->state() == QAudio::ActiveState) {
		m_audioInput->suspend();
		ui.MicLabel->setText("Mic is off");
		QPixmap pixmap("./mic_off.png");
		QIcon ButtonIcon(pixmap);
		ui.MicSig->setIcon(ButtonIcon);
		ui.MicSig->setIconSize(pixmap.rect().size());
		ui.MicSig->setFixedSize(pixmap.rect().size());
		sess.arrayBuff.clear();
	}
	else if (m_audioInput->state() == QAudio::IdleState) {
		// no-op
	}
}

void SpacjaTeleClient::rtpInit()
{

	portbase = currentChannel->portAudioIn;
	sessparams.SetOwnTimestampUnit(1.0 / 8000.0);

	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(portbase);
	status = sess.Create(sessparams, &transparams);
	SpacjaTeleClient::checkerror(status);

	updateDestinations();
	getAudio();
}

//void SpacjaTeleClient::rtpServInit(uint16_t portIn)
//{
//
//#ifdef RTP_SOCKETTYPE_WINSOCK
//	WSAStartup(MAKEWORD(2, 2), &datServ);
//#endif // RTP_SOCKETTYPE_WINSOCK
//
//	portbaseServ = portIn;
//	sessparamsServ.SetOwnTimestampUnit(1.0 / 8000.0);
//	transparamsServ.SetPortbase(portbaseServ);
//	statusServ = sessServ.Create(sessparamsServ, &transparamsServ);
//	checkerror(statusServ);
//	getAudio();
//}


void MyRTPSession::OnPollThreadStep()
{
	BeginDataAccess();

	// check incoming packets
	if (GotoFirstSourceWithData())
	{
		do
		{
			jrtplib::RTPPacket *pack;
			jrtplib::RTPSourceData *srcdat;

			srcdat = GetCurrentSourceInfo();

			while ((pack = GetNextPacket()) != NULL)
			{
				ProcessRTPPacket(*srcdat, *pack);
				DeletePacket(pack);
			}
		} while (GotoNextSourceWithData());
	}
	EndDataAccess();
}

MyRTPSession::MyRTPSession()
{
	arrayBuff = QByteArray(32768, 0);
}

void MyRTPSession::ProcessRTPPacket(const jrtplib::RTPSourceData &srcdat, const jrtplib::RTPPacket &rtppack)
{
	arrayBuff = reinterpret_cast< char*>( rtppack.GetPayloadData());
}

void SpacjaTeleClient::acceptConnection()
{
	tcpServerConnection = tcpServer.nextPendingConnection();
	connect(tcpServerConnection, SIGNAL(readyRead()),
		this, SLOT(updateServerProgress()));
	connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(displayError(QAbstractSocket::SocketError)));
}


bool operator==(const Host& lhs, const Host& rhs)
{
	return lhs.hostAddress == rhs.hostAddress && lhs.hostPort == rhs.hostPort;
}

void SpacjaTeleClient::updateServerProgress()
{
	bytesReceived += static_cast<int>(tcpServerConnection->bytesAvailable());
	Host server;
	server.hostAddress = tcpServerConnection->peerAddress();
	server.hostPort = tcpServerConnection->peerPort();

	QByteArray buffer(20, ' ');
	tcpServerConnection->read(buffer.data(), 20);
	std::string input = buffer.data();
	std::string delimiter = "/";

	size_t pos = 0;



	while ((pos = input.find(delimiter)) != std::string::npos) 
	{
		const auto token = input.substr(0, pos);
		input.erase(0, pos + delimiter.length());
		bool operationComplite = false;

		if (currentChannel->server == server)
		{
			if (token == "BRDY")// ready INT_time
			{
				beReadySig(stoi(input));
				operationComplite = true;
			}

			else if (token == "LIVE")// live
			{
				beLiveSig();
				operationComplite = true;
			}
		}

		ChannelInfo* channelInfo = getChannelInfo(server);
		if(!channelInfo)
		{

			ChannelInfo newChannelInfo;
			newChannelInfo.server = server;
			channelInfos.push_back(newChannelInfo);
			channelInfo = &channelInfos.back();
			{
				ui.ChannelSpin->setMaximum(channelInfos.size());
			}
		}
		if (token == "NAME")// name newName
		{
			channelInfo->nameOnChannel = input;
			operationComplite = true;
			
		}

		else if (token == "AUIN")// 
		{
			channelInfo->portAudioIn = stoi(input);
			operationComplite = true;
		}
		else if (token == "ADDHOST")
		{
			delimiter = ":";
			if((pos = input.find(delimiter)) != std::string::npos)
			{
				std::string hostname = input.substr(0, pos);
				input.erase(0, pos + delimiter.length());

				Host newHost;
				newHost.hostAddress = QHostAddress(QString::fromStdString(hostname));
				newHost.hostPort = stoi(input);

				channelInfo->hosts.push_back(newHost);
				operationComplite = true;
			}
		}

		if(firstConfiguration)
		{
			changeChannel(1);
			firstConfiguration = false;
		}

		if(operationComplite)
		{
			updateChannel();
			tcpServerConnection->write("ACK/");
		}
	}

	if (bytesReceived == TotalBytes) {
		tcpServerConnection->close();
#ifndef QT_NO_CURSOR
		QApplication::restoreOverrideCursor();
#endif
	}
}

bool SpacjaTeleClient::isInChannelList(Host& server)
{
	for (auto channelInfo : channelInfos)
	{
		if(channelInfo.server == server)
		{
			return true;
		}
	}
	return false;
}

ChannelInfo* SpacjaTeleClient::getChannelInfo(Host& server)
{
	for (ChannelInfo &channelInfo : channelInfos)
	{
		if (channelInfo.server == server)
		{
			return &channelInfo;
		}
	}
	return nullptr;
}

void SpacjaTeleClient::displayError(QAbstractSocket::SocketError socketError)
{
	if (socketError == QTcpSocket::RemoteHostClosedError)
		return;
	tcpServer.close();
#ifndef QT_NO_CURSOR
	QApplication::restoreOverrideCursor();
#endif
}