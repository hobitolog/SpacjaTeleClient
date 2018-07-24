#include "spacjateleclient.h"
#include <stdlib.h>
#include <math.h>

#include <QDateTime>
#include <QDebug>
#include <QVBoxLayout>
#include <QAudioDeviceInfo>
#include <qendian.h>

#include <QAudioOutput>
#include <qmath.h>

#include <QtWidgets>
#include <QtNetwork>

static const int TotalBytes = 100;


SpacjaTeleClient::SpacjaTeleClient(QWidget *parent)
	: QMainWindow(parent)
{

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

	ch1 = "192.168.0.104";
	ch2 = "127.0.0.1";
	ch3 = "192.168.1.10";
	ch4 = "192.168.0.104";
	portAudioOut1=8080;
	portAudioOut2=8082;
	portAudioOut3=8084;
	portAudioOut4=8086;
	portAudioIn=8082;


	QPixmap pixmap("./mic_on.png");
	QIcon ButtonIcon(pixmap);
	ui.MicSig->setIcon(ButtonIcon);
	ui.MicSig->setIconSize(pixmap.rect().size());
	ui.MicSig->setFixedSize(pixmap.rect().size());
	connect(ui.MicSig, SIGNAL(clicked()), this, SLOT(muteAudioOut()));

#ifndef QT_NO_CURSOR
	QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

	while (!tcpServer.isListening() && !tcpServer.listen(QHostAddress::Any, 6060)) {
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

	onChangeChannel();
	rtpInit();
	audioInit(QAudioDeviceInfo::defaultInputDevice(), QAudioDeviceInfo::defaultOutputDevice());
	rtpServInit();

	sendAudio();

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
	ui.liveSig->setText((ui.liveSig->text()=="") ? "ON AIR" : "");
}

void SpacjaTeleClient::changeChannel(int i)
{
	m_audioInput->suspend();
	m_audioOutput->suspend();
	sess.BYEDestroy(jrtplib::RTPTime(10, 0), 0, 0);
	sessServ.BYEDestroy(jrtplib::RTPTime(10, 0), 0, 0);
#ifdef RTP_SOCKETTYPE_WINSOCK
	WSACleanup();
#endif // RTP_SOCKETTYPE_WINSOCK

	switch (i)
	{
	case 1:
		rtpInit(ch1, 1234, portAudioOut1);
		rtpServInit(portAudioIn);
		break;
	case 2:
		rtpInit(ch2, 1234, portAudioOut2);
		rtpServInit(portAudioIn);
		break;
	case 3:
		rtpInit(ch3, 1234, portAudioOut3);
		rtpServInit(portAudioIn);
		break;
	case 4:
		rtpInit(ch4, 1234, portAudioOut4);
		rtpServInit(portAudioIn);
		break;
	}
	sessServ.arrayBuff.clear();
	m_audioInput->resume();
	m_audioOutput->resume();
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
	m_audioInfo->stop();
	m_audioOutput->stop();


	sess.BYEDestroy(jrtplib::RTPTime(10, 0), 0, 0);
	sessServ.BYEDestroy(jrtplib::RTPTime(10, 0), 0, 0);

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
	format.setSampleType(QAudioFormat::SignedInt);
	format.setByteOrder(QAudioFormat::LittleEndian);
	format.setCodec("audio/pcm");

	if (!deviceInfoIn.isFormatSupported(format)) {
		qWarning() << "Default format not supported - trying to use nearest";
		format = deviceInfoIn.nearestFormat(format);
	}
	if (!deviceInfoOut.isFormatSupported(format)) {
		qWarning() << "Default format not supported - trying to use nearest";
		format = deviceInfoOut.nearestFormat(format);
	}
	m_audioInfo.reset(new AudioInfo(format));//nadawanie
	m_audioInput.reset(new QAudioInput(deviceInfoIn, format));//nadawanie
	m_audioInfo->start();//nadawanie
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
			status = sess.SendPacket((void *)buffer.data(), buffer.length(), 0, false, 1);
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
			const qint64 len = sessServ.arrayBuff.length();
			if (len)				
				io->write(sessServ.arrayBuff.data(), len);
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
		QPixmap pixmap("./mic_on.png");
		QIcon ButtonIcon(pixmap);
		ui.MicSig->setIcon(ButtonIcon);
		ui.MicSig->setIconSize(pixmap.rect().size());
		ui.MicSig->setFixedSize(pixmap.rect().size());
	}
	else if (m_audioInput->state() == QAudio::ActiveState) {
		m_audioInput->suspend();
		QPixmap pixmap("./mic_off.png");
		QIcon ButtonIcon(pixmap);
		ui.MicSig->setIcon(ButtonIcon);
		ui.MicSig->setIconSize(pixmap.rect().size());
		ui.MicSig->setFixedSize(pixmap.rect().size());
		sessServ.arrayBuff.clear();
	}
	else if (m_audioInput->state() == QAudio::IdleState) {
		// no-op
	}
}

void SpacjaTeleClient::rtpInit(std::string ipString, uint16_t portIn, uint16_t potDes)
{
	WSAStartup(MAKEWORD(2, 2), &dat);
	portbase = portIn;

	ipstr = ipString;
	destip = inet_addr(ipstr.c_str());
	if (destip == INADDR_NONE)
	{
		std::cerr << "Bad IP address specified" << std::endl;
	}
	destip = ntohl(destip);
	destport = potDes;

	sessparams.SetOwnTimestampUnit(1.0 / 8000.0);

	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(portbase);
	status = sess.Create(sessparams, &transparams);
	SpacjaTeleClient::checkerror(status);

	jrtplib::RTPIPv4Address addr(destip, destport);

	status = sess.AddDestination(addr);
	SpacjaTeleClient::checkerror(status);
}

void SpacjaTeleClient::rtpServInit(uint16_t portIn)
{

#ifdef RTP_SOCKETTYPE_WINSOCK
	WSAStartup(MAKEWORD(2, 2), &datServ);
#endif // RTP_SOCKETTYPE_WINSOCK

	portbaseServ = portIn;
	sessparamsServ.SetOwnTimestampUnit(1.0 / 8000.0);
	transparamsServ.SetPortbase(portbaseServ);
	statusServ = sessServ.Create(sessparamsServ, &transparamsServ);
	checkerror(statusServ);
	getAudio();
}

AudioInfo::AudioInfo(const QAudioFormat &format)
	: m_format(format)
{
	switch (m_format.sampleSize()) {
	case 8:
		switch (m_format.sampleType()) {
		case QAudioFormat::UnSignedInt:
			m_maxAmplitude = 255;
			break;
		case QAudioFormat::SignedInt:
			m_maxAmplitude = 127;
			break;
		default:
			break;
		}
		break;
	case 16:
		switch (m_format.sampleType()) {
		case QAudioFormat::UnSignedInt:
			m_maxAmplitude = 65535;
			break;
		case QAudioFormat::SignedInt:
			m_maxAmplitude = 32767;
			break;
		default:
			break;
		}
		break;

	case 32:
		switch (m_format.sampleType()) {
		case QAudioFormat::UnSignedInt:
			m_maxAmplitude = 0xffffffff;
			break;
		case QAudioFormat::SignedInt:
			m_maxAmplitude = 0x7fffffff;
			break;
		case QAudioFormat::Float:
			m_maxAmplitude = 0x7fffffff; // Kind of
		default:
			break;
		}
		break;

	default:
		break;
	}
}

void AudioInfo::start()
{
	open(QIODevice::WriteOnly);
}

void AudioInfo::stop()
{
	close();
}

qint64 AudioInfo::readData(char *data, qint64 maxlen)
{
	Q_UNUSED(data)
		Q_UNUSED(maxlen)

		return 0;
}

qint64 AudioInfo::writeData(const char *data, qint64 len)
{
	if (m_maxAmplitude) {
		Q_ASSERT(m_format.sampleSize() % 8 == 0);
		const int channelBytes = m_format.sampleSize() / 8;
		const int sampleBytes = m_format.channelCount() * channelBytes;
		Q_ASSERT(len % sampleBytes == 0);
		const int numSamples = len / sampleBytes;

		quint32 maxValue = 0;
		const unsigned char *ptr = reinterpret_cast<const unsigned char *>(data);

		for (int i = 0; i < numSamples; ++i) {
			for (int j = 0; j < m_format.channelCount(); ++j) {
				quint32 value = 0;

				if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
					value = *reinterpret_cast<const quint8*>(ptr);
				}
				else if (m_format.sampleSize() == 8 && m_format.sampleType() == QAudioFormat::SignedInt) {
					value = qAbs(*reinterpret_cast<const qint8*>(ptr));
				}
				else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
					if (m_format.byteOrder() == QAudioFormat::LittleEndian)
						value = qFromLittleEndian<quint16>(ptr);
					else
						value = qFromBigEndian<quint16>(ptr);
				}
				else if (m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::SignedInt) {
					if (m_format.byteOrder() == QAudioFormat::LittleEndian)
						value = qAbs(qFromLittleEndian<qint16>(ptr));
					else
						value = qAbs(qFromBigEndian<qint16>(ptr));
				}
				else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::UnSignedInt) {
					if (m_format.byteOrder() == QAudioFormat::LittleEndian)
						value = qFromLittleEndian<quint32>(ptr);
					else
						value = qFromBigEndian<quint32>(ptr);
				}
				else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::SignedInt) {
					if (m_format.byteOrder() == QAudioFormat::LittleEndian)
						value = qAbs(qFromLittleEndian<qint32>(ptr));
					else
						value = qAbs(qFromBigEndian<qint32>(ptr));
				}
				else if (m_format.sampleSize() == 32 && m_format.sampleType() == QAudioFormat::Float) {
					value = qAbs(*reinterpret_cast<const float*>(ptr) * 0x7fffffff); // assumes 0-1.0
				}

				maxValue = qMax(value, maxValue);
				ptr += channelBytes;
			}
		}

		maxValue = qMin(maxValue, m_maxAmplitude);
		m_level = qreal(maxValue) / m_maxAmplitude;
	}

	emit update();
	return len;
}

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

void SpacjaTeleClient::updateServerProgress()
{
	bytesReceived += (int)tcpServerConnection->bytesAvailable();
	QByteArray abc(100, '_');
	tcpServerConnection->read(abc.data(), 100);
	std::string input = abc.data();
	std::string delimiter = " ";

	size_t pos = 0;
	std::string token;
	while ((pos = input.find(delimiter)) != std::string::npos) {
		token = input.substr(0, pos);
		input.erase(0, pos + delimiter.length());

		if (token == "name")// name newName
		{
			ui.ClientName->setText(QString::fromStdString(input));
		}
		else if (token == "beReady")// ready INT_time
		{
			beReadySig(std::atoi(input.c_str()));

		}
		else if (token == "live")// live
		{
			beLiveSig();
		}
		else if (token == "ch1")// ch1
		{
			ch1 = input;
		}
		else if (token == "ch2")// ch2
		{
			ch2 = input;
		}
		else if (token == "ch3")// ch3
		{
			ch3 = input;
		}
		else if (token == "ch4")// ch4
		{
			ch4 = input;
		}
		else if (token == "PO1")// PO1
		{
			portAudioOut1 = std::atoi(input.c_str());
		}
		else if (token == "PO2")// PO2
		{
			portAudioOut2 = std::atoi(input.c_str());
		}
		else if (token == "PO3")// PO3
		{
			portAudioOut3 = std::atoi(input.c_str());
		}
		else if (token == "PO4")// PO4
		{
			portAudioOut4 = std::atoi(input.c_str());
		}
		else if (token == "PI")// PI
		{
			portAudioIn = std::atoi(input.c_str());
		}
	}

	if (bytesReceived == TotalBytes) {
		tcpServerConnection->close();
#ifndef QT_NO_CURSOR
		QApplication::restoreOverrideCursor();
#endif
	}
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